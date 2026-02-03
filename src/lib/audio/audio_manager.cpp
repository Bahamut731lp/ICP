#define MINIAUDIO_IMPLEMENTATION
#include "audio/audio_manager.hpp"

#include <iostream>
#include <algorithm>

static ManagedSound make_managed_sound()
{
    return ManagedSound(new ma_sound());
}

AudioManager::AudioManager()
{
    ma_result result = ma_engine_init(nullptr, &engine);
    if (result != MA_SUCCESS)
    {
        throw std::runtime_error("Failed to initialize AudioManager: " + std::to_string(result));
    }
}

AudioManager::~AudioManager()
{
    // Containers (sound_bank, active_sounds) will clear themselves,
    // triggering MaSoundDeleter for each element before the engine stops.
    ma_engine_uninit(&engine);
}

void AudioManager::load(const std::string &name, const std::filesystem::path &filename,
                        float min_dist, float max_dist, float volume)
{
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "Audio Error: File not found " << std::filesystem::absolute(filename) << "\n";
        return;
    }

    auto sound = make_managed_sound();

    ma_uint32 flags = MA_SOUND_FLAG_DECODE;

    ma_result result = ma_sound_init_from_file(&engine, filename.string().c_str(),
                                               flags, nullptr, nullptr, sound.get());

    if (result != MA_SUCCESS)
    {
        std::cerr << "Audio Error: Failed to load " << name << " (" << result << ")\n";
        if (result == -10)
            std::cerr << "  -> Hint: File format invalid. Try converting to WAV.\n";
        return;
    }

    ma_sound_set_min_distance(sound.get(), min_dist);
    ma_sound_set_max_distance(sound.get(), max_dist);
    ma_sound_set_volume(sound.get(), volume);

    sound_bank[name] = std::move(sound);
}
void AudioManager::load_BGM(const std::string &name, const std::filesystem::path &filename, float volume)
{
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "AUDIO ERROR: File missing at " << std::filesystem::absolute(filename) << "\n";
        return;
    }

    auto sound = make_managed_sound();

    ma_result result = ma_sound_init_from_file(&engine, filename.string().c_str(),
                                               MA_SOUND_FLAG_STREAM, nullptr, nullptr, sound.get());

    if (result != MA_SUCCESS)
    {
        std::cerr << "AUDIO ERROR: Could not load BGM '" << name << "'\n";
        std::cerr << "   Path: " << filename << "\n";
        std::cerr << "   Code: " << result << " (";

        switch (result)
        {
        case -2:
            std::cerr << "Invalid Args - check pointers";
            break;
        case -10:
            std::cerr << "Invalid File - format unsupported or corrupt";
            break;
        case -20:
            std::cerr << "Path not found (internal)";
            break;
        default:
            std::cerr << "Generic/Other";
            break;
        }
        std::cerr << ")\n";
        return;
    }

    ma_sound_set_volume(sound.get(), volume);
    bgm_bank[name] = std::move(sound);
    std::cout << "AUDIO SUCCESS: Loaded BGM '" << name << "'\n";
}

bool AudioManager::play_3D(const std::string &name, float x, float y, float z)
{
    auto it = sound_bank.find(name);
    if (it == sound_bank.end())
        return false;

    auto instance = make_managed_sound();

    if (ma_sound_init_copy(&engine, it->second.get(), 0, nullptr, instance.get()) != MA_SUCCESS)
    {
        return false;
    }

    ma_sound_set_position(instance.get(), x, y, z);
    ma_sound_start(instance.get());

    active_sounds.push_back(std::move(instance));
    return true;
}

bool AudioManager::play_BGM(const std::string &name, float volume)
{
    auto it = bgm_bank.find(name);
    if (it == bgm_bank.end())
        return false;

    if (current_bgm)
    {
        ma_sound_stop(current_bgm.get());
    }

    ma_sound *raw_sound = it->second.get();

    ma_sound_seek_to_pcm_frame(raw_sound, 0);

    ma_sound_set_looping(raw_sound, MA_TRUE);
    ma_sound_set_volume(raw_sound, volume);
    ma_sound_set_spatialization_enabled(raw_sound, MA_FALSE);

    ma_sound_start(raw_sound);

    return true;
}

bool AudioManager::play(const std::string &name)
{
    auto it = sound_bank.find(name);
    if (it == sound_bank.end()) return false;

    auto instance = make_managed_sound();

    if (ma_sound_init_copy(&engine, it->second.get(), 0, nullptr, instance.get()) != MA_SUCCESS) {
        return false;
    }

    ma_sound_set_spatialization_enabled(instance.get(), MA_FALSE);
    
    ma_sound_set_volume(instance.get(), ma_sound_get_volume(it->second.get()));

    ma_sound_start(instance.get());
    active_sounds.push_back(std::move(instance));
    return true;
}
void AudioManager::stop_BGM()
{
    if (current_bgm)
    {
        ma_sound_stop(current_bgm.get());
        current_bgm.reset();
    }
}

void AudioManager::change_volume(float change)
{
    if (!current_bgm)
        return;
    float volume = ma_sound_get_volume(current_bgm.get());
    volume = std::clamp(volume + (change * 0.05f), 0.0f, 1.0f);
    ma_sound_set_volume(current_bgm.get(), volume);
}

void AudioManager::set_listener_position(float x, float y, float z, float dx, float dy, float dz)
{
    ma_engine_listener_set_position(&engine, 0, x, y, z);
    ma_engine_listener_set_direction(&engine, 0, dx, dy, dz);
}

void AudioManager::update()
{
    active_sounds.erase(
        std::remove_if(active_sounds.begin(), active_sounds.end(),
                       [](const ManagedSound &sound)
                       {
                           return !ma_sound_is_playing(sound.get()) || ma_sound_at_end(sound.get());
                       }),
        active_sounds.end());
}