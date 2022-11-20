#pragma once
#include "Mode.hpp"
#include "PlayMode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <chrono>

#include "data_path.hpp"

#define MARGIN 5.

struct MainMenuMode : Mode {
	MainMenuMode();
	virtual ~MainMenuMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size, glm::uvec2 const &window_size) override;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		bool pressed = false;
		float timestamp = 0.0f;
	} left, right, down, up, ret;
	float time_since_enter;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	// background color:
	glm::vec3 bg_color = glm::vec3(0.3f, 0.3f, 0.3f);

    float time_elapsed;
    float fade_time;

    //lerp copied from https://graphicscompendium.com/opengl/22-interpolation
    static glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return x * (1.f - t) + y * t;
    }
};
