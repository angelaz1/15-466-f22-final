#include "MainMenuMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Beatmap.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <fstream>

#include <ctype.h>

#define FONT "Roboto-Medium.ttf"

MainMenuMode::MainMenuMode() {
	title = SpriteManager::GetInstance()->get_sprite("menu/title");
	start_button = SpriteManager::GetInstance()->get_sprite("menu/start");
	how_to_play_button = SpriteManager::GetInstance()->get_sprite("menu/how_to_play");
	quit_button = SpriteManager::GetInstance()->get_sprite("menu/quit");
}

MainMenuMode::~MainMenuMode() {}

bool MainMenuMode::check_in_bounds(glm::vec2 mouse_pos, Sprite *sprite, glm::vec2 sprite_pos) {
	glm::vec2 dim = sprite->size;

	if (mouse_pos.x < sprite_pos.x - dim.x / 2 || mouse_pos.x > sprite_pos.x + dim.x / 2
	 || mouse_pos.y < sprite_pos.y - dim.y / 2 || mouse_pos.y > sprite_pos.y + dim.y / 2) 
	{
		return false;
	}

	return true;
}

// handle key presses
bool MainMenuMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	start_button_pos = glm::vec2(window_size.x * button_x_ratio, window_size.y * start_button_y_ratio);
	how_to_play_button_pos = glm::vec2(window_size.x * button_x_ratio, window_size.y * how_to_play_button_y_ratio);
	quit_button_pos = glm::vec2(window_size.x * button_x_ratio, window_size.y * quit_button_y_ratio);

	glm::vec2 mouse_pos = glm::vec2(evt.motion.x, window_size.y - evt.motion.y);

	start_button_color = glm::u8vec4(0xff);
	how_to_play_button_color = glm::u8vec4(0xff);
	quit_button_color = glm::u8vec4(0xff);

	if (check_in_bounds(mouse_pos, start_button, start_button_pos)) {
		if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT) {
			auto playmode = std::make_shared< PlayMode >();
			Mode::set_current(playmode);
			playmode->set_dialogue_tree("violin");
		} else {
			start_button_color = glm::u8vec4(0xe0, 0xe0, 0xe0, 0xff);
		}
		return true;
	} 
	else if (check_in_bounds(mouse_pos, how_to_play_button, how_to_play_button_pos)) {
		if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT) {
			// Show instructions
			auto playmode = std::make_shared< PlayMode >();
			Mode::set_current(playmode);
			playmode->set_dialogue_tree("prototype");
		} else {
			how_to_play_button_color = glm::u8vec4(0xe0, 0xe0, 0xe0, 0xff);
		}
		return true;
	}
	else if (check_in_bounds(mouse_pos, quit_button, quit_button_pos)) {
		if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT) {
			Mode::set_current(nullptr);
		} else {
			quit_button_color = glm::u8vec4(0xe0, 0xe0, 0xe0, 0xff);
		}
		return true;
	}

	return false;
}


void MainMenuMode::update(float elapsed) {
	if (time_elapsed > 1) {
        time_elapsed = 1;
    } else {
        time_elapsed += elapsed;
    }
}

void MainMenuMode::draw(glm::uvec2 const &drawable_size, glm::uvec2 const &window_size) {
	glClearColor(bg_color.r,bg_color.g,bg_color.b,1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); 
	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	{ // draw main menu
		title->set_drawable_size(window_size);
		start_button->set_drawable_size(window_size);
		how_to_play_button->set_drawable_size(window_size);
		quit_button->set_drawable_size(window_size);

		title_pos = glm::vec2(window_size.x * button_x_ratio, window_size.y * title_y_ratio);
		start_button_pos = glm::vec2(window_size.x * button_x_ratio, window_size.y * start_button_y_ratio);
		how_to_play_button_pos = glm::vec2(window_size.x * button_x_ratio, window_size.y * how_to_play_button_y_ratio);
		quit_button_pos = glm::vec2(window_size.x * button_x_ratio, window_size.y * quit_button_y_ratio);

		title->draw(title_pos);
		start_button->draw(start_button_pos, 1.0f, start_button_color);
		how_to_play_button->draw(how_to_play_button_pos, 1.0f, how_to_play_button_color);
		quit_button->draw(quit_button_pos, 1.0f, quit_button_color);
	}
	
	GL_ERRORS();
}