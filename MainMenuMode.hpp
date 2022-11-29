#pragma once
#include "Mode.hpp"
#include "PlayMode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "SpriteManager.hpp"

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

	// Main menu sprites
	Sprite *title;
	Sprite *start_button;
	Sprite *how_to_play_button;
	Sprite *quit_button;

	// Positioning
	const float button_x_ratio = 0.5f;
	const float title_y_ratio = 0.7f;
	const float start_button_y_ratio = 0.45f;
	const float how_to_play_button_y_ratio = 0.325f;
	const float quit_button_y_ratio = 0.2f;

	glm::vec2 title_pos;
	glm::vec2 start_button_pos;
	glm::vec2 how_to_play_button_pos;
	glm::vec2 quit_button_pos;

	glm::u8vec4 start_button_color = glm::u8vec4(0xff);
	glm::u8vec4 how_to_play_button_color = glm::u8vec4(0xff);
	glm::u8vec4 quit_button_color = glm::u8vec4(0xff);

	// background color:
	glm::vec3 bg_color = glm::vec3(0.3f, 0.3f, 0.3f);

    float time_elapsed;
    float fade_time;

	bool check_in_bounds(glm::vec2 mouse_pos, Sprite *sprite, glm::vec2 sprite_pos);

    //lerp copied from https://graphicscompendium.com/opengl/22-interpolation
    static glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return x * (1.f - t) + y * t;
    }
};
