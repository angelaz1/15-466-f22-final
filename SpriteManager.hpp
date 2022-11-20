#pragma once
#include "Mode.hpp"
#include "Scene.hpp"
#include "Sprite.hpp"

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "data_path.hpp"
#include "gl_errors.hpp"
#include "ColorTextureProgram.hpp"

// Singleton SpriteManager
class SpriteManager {

protected:
    SpriteManager();
    static SpriteManager* spritemanager_;

public:
    SpriteManager(SpriteManager &other) = delete;
    void operator=(const SpriteManager &) = delete;
    static SpriteManager *GetInstance();

    Sprite* get_sprite(std::string name);

private:
    std::unordered_map<std::string, Sprite*> sprite_map;
    Sprite* read_sprite(std::string sprite_name);
};
