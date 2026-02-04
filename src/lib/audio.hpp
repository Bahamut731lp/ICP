#pragma once

#include <glm/glm.hpp>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <unordered_map>
#include "miniaudio.h"

// Simple struct to package the 3D sound data
struct AudioRequest {
    std::string path;
    glm::vec3 position;
    bool isSpatial;
};

class Audio {
public:
    static void init();    
    static void shutdown(); 

    static void updateListener(glm::vec3 pos, glm::vec3 forward);

    static void play(const std::string& path);
    static void play(const std::string& path, glm::vec3 position);

private:
    static void loop(); // The actual thread function

    static ma_engine engine;
    static std::thread m_worker;
    static std::mutex m_mutex;
    static std::atomic<bool> m_running;

    // Thread synchronization
    static std::queue<AudioRequest> m_queue;
    static std::condition_variable m_cv;

    // Listener state (cached to be updated in the audio hardware)
    static glm::vec3 m_listenerPos;
    static glm::vec3 m_listenerForward;
};