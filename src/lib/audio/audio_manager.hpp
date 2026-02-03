#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include "audio/miniaudio.h"

struct MaSoundDeleter {
    void operator()(ma_sound* s) const {
        if (s) {
            ma_sound_uninit(s);
            delete s;
        }
    }
};

using ManagedSound = std::unique_ptr<ma_sound, MaSoundDeleter>;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    AudioManager(AudioManager&&) noexcept = default;
    AudioManager& operator=(AudioManager&&) noexcept = default;

    void load(const std::string& name, const std::filesystem::path& filename, 
              float min_distance, float max_distance, float volume);
    void load_BGM(const std::string& name, const std::filesystem::path& filename, float volume);
    
    bool play_3D(const std::string& name, float x, float y, float z);
    bool play_BGM(const std::string& name, float volume);
    
    void stop_BGM();
    void change_volume(float change);
    void set_listener_position(float x, float y, float z, float dirX, float dirY, float dirZ);
    
    void update();

private:
    ma_engine engine;

    std::unordered_map<std::string, ManagedSound> sound_bank;
    std::unordered_map<std::string, ManagedSound> bgm_bank;
    std::vector<ManagedSound> active_sounds;
    ManagedSound current_bgm;
};