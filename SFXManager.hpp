#pragma once
#include "Mode.hpp"
#include "Scene.hpp"
#include "Sound.hpp"

#include <unordered_map>
#include <vector>

#include "data_path.hpp"
#include "gl_errors.hpp"
#include "ColorTextureProgram.hpp"

// Singleton SFXManager
class SFXManager {

protected:
    SFXManager();
    static SFXManager* sfxmanager_;

public:
    SFXManager(SFXManager &other) = delete;
    void operator=(const SFXManager &) = delete;
    static SFXManager *GetInstance();

    Sound::Sample* get_sfx(std::string name);
    void play_one_shot(std::string name, float volume = 1.0f);
    void play_looping(std::string name, float volume = 1.0f);
    bool current_sfx_done();
    void stop_current_sfx();

private:
    std::unordered_map<std::string, Sound::Sample*> sfx_map;
    Sound::Sample* read_sfx(std::string sfx_name);

    std::shared_ptr<Sound::PlayingSample> current_sfx = nullptr;
};
