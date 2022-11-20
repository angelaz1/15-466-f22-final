#include "SpriteManager.hpp"

SpriteManager* SpriteManager::spritemanager_ = nullptr;

SpriteManager::SpriteManager() {
    sprite_map = std::unordered_map<std::string, Sprite*>();
}

SpriteManager *SpriteManager::GetInstance() {
    if (spritemanager_ == nullptr) {
        spritemanager_ = new SpriteManager();
    }
    return spritemanager_;
}

Sprite* SpriteManager::read_sprite(std::string sprite_name) {
    std::string file_path = data_path("images/" + sprite_name + ".png");
    if (!std::filesystem::exists(file_path)) {
        return nullptr;
    }

    Sprite* sprite = new Sprite(file_path);
    sprite_map.insert(std::pair(sprite_name, sprite));
    return sprite;
}

Sprite* SpriteManager::get_sprite(std::string sprite_name) {
    auto res = sprite_map.find(sprite_name);
    if (res != sprite_map.end()) {
        return res->second;
    } else {
        return read_sprite(sprite_name);
    }
}
