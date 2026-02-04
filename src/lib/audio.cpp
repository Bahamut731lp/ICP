#include "audio.hpp"
#include "logger.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// Define the static members
std::thread Audio::m_worker;
std::mutex Audio::m_mutex;
std::atomic<bool> Audio::m_running{false};
std::queue<AudioRequest> Audio::m_queue;
std::condition_variable Audio::m_cv;
glm::vec3 Audio::m_listenerPos{0.0f};
glm::vec3 Audio::m_listenerForward{0.0f, 0.0f, -1.0f};
ma_engine Audio::engine;

static std::queue<ma_sound*> g_deadQueue;
static std::mutex g_deadMutex;

void on_sound_end(void* pUserData, ma_sound* pSound) {
    std::lock_guard<std::mutex> lock(g_deadMutex);
    g_deadQueue.push(pSound);
}

void Audio::init()
{
    if (m_running)
        return;

    ma_result result = ma_engine_init(nullptr, &Audio::engine);
    if (result != MA_SUCCESS)
    {
        throw std::runtime_error("Failed to initialize AudioManager: " + std::to_string(result));
    }

    m_running = true;
    m_worker = std::thread(&Audio::loop);
    Logger::info("AudioService: Thread started.");
}

void Audio::loop()
{
    while (m_running)
    {
        {
            std::lock_guard<std::mutex> lock(g_deadMutex);
            while (!g_deadQueue.empty()) {
                ma_sound* s = g_deadQueue.front();
                g_deadQueue.pop();
                ma_sound_uninit(s);
                free(s);
            }
        }

        AudioRequest request;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            // Wait with a small timeout so we can still clean up trash 
            // even if no one is playing new sounds.
            if (!m_cv.wait_for(lock, std::chrono::milliseconds(10), 
                [] { return !m_queue.empty() || !m_running; })) continue;

            if (!m_running) break;
            request = m_queue.front();
            m_queue.pop();
        }

        ma_sound *sound = (ma_sound *)malloc(sizeof(ma_sound));
        ma_result res = ma_sound_init_from_file(&engine, request.path.c_str(), MA_SOUND_FLAG_DECODE, nullptr, nullptr, sound);

        if (res != MA_SUCCESS)
        {
            Logger::error("Failed to load: " + request.path);
            return;
        }
        
        ma_sound_set_volume(sound, 1.0f);
        ma_sound_seek_to_pcm_frame(sound, 0);
        ma_sound_set_end_callback(sound, on_sound_end, nullptr);

        if (request.isSpatial)
        {
            Logger::info("Processing spatial audio request: " + request.path);

            ma_sound_set_position(sound, request.position.x, request.position.y, request.position.z);
            ma_sound_set_min_distance(sound, 1.0f);
            ma_sound_set_max_distance(sound, 100.0f);
            ma_sound_set_spatialization_enabled(sound, MA_TRUE);
        } else {
            ma_sound_set_spatialization_enabled(sound, MA_FALSE);
        }

        ma_sound_start(sound);
    }
}

void Audio::play(const std::string &path)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push({path, glm::vec3(0.0f), false});
    }
    m_cv.notify_one();
}

void Audio::play(const std::string &path, glm::vec3 position)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push({path, position, true});
    }
    m_cv.notify_one();
}

void Audio::updateListener(glm::vec3 pos, glm::vec3 forward)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listenerPos = pos;
    m_listenerForward = forward;

    ma_engine_listener_set_position(&engine, 0, m_listenerPos.x, m_listenerPos.y, m_listenerPos.z);
    ma_engine_listener_set_direction(&engine, 0, m_listenerForward.x, m_listenerForward.y, m_listenerForward.z);
    // In a real engine, you'd apply this to the hardware here
}

void Audio::shutdown()
{
    m_running = false;
    m_cv.notify_one();
    if (m_worker.joinable())
    {
        m_worker.join();
    }
    Logger::info("AudioService: Thread joined.");
}