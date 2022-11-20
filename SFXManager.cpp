#include "SFXManager.hpp"

SFXManager* SFXManager::sfxmanager_ = nullptr;

SFXManager::SFXManager() {
    sfx_map = std::unordered_map<std::string, Sound::Sample*>();
}

SFXManager *SFXManager::GetInstance() {
    if (sfxmanager_ == nullptr) {
        sfxmanager_ = new SFXManager();
    }
    return sfxmanager_;
}

Sound::Sample* SFXManager::read_sfx(std::string sfx_name) {
    std::string file_path = data_path("sfx/" + sfx_name + ".wav");

    Sound::Sample* sample = new Sound::Sample(file_path);
    sfx_map.insert(std::pair(sfx_name, sample));
    return sample;
}

Sound::Sample* SFXManager::get_sfx(std::string sfx_name) {
    auto res = sfx_map.find(sfx_name);
    if (res != sfx_map.end()) {
        return res->second;
    } else {
        return read_sfx(sfx_name);
    }
}

void SFXManager::play_one_shot(std::string sfx_name, float volume) {
    stop_current_sfx();
    current_sfx = Sound::play(*(get_sfx(sfx_name)), volume, 0.0f);
}

void SFXManager::play_looping(std::string sfx_name, float volume) {
    stop_current_sfx();
    current_sfx = Sound::loop(*(get_sfx(sfx_name)), volume, 0.0f);
}

bool SFXManager::current_sfx_done() {
    if (current_sfx != nullptr) {
        return current_sfx->stopped;
    }
    return true;
}

void SFXManager::stop_current_sfx() {
    if (current_sfx != nullptr) {
        current_sfx->stop();
    }
}