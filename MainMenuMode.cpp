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

// handle key presses
bool MainMenuMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	// check key pressed
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_RETURN) {
			Mode::set_current(std::make_shared< PlayMode >());
		} else if (evt.key.keysym.sym == SDLK_UP) {
			// Change choice selected
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			// Change choice selected
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

		glm::vec2 title_pos = glm::vec2(window_size.x * 0.5f, window_size.y * 0.7f);
		glm::vec2 start_button_pos = glm::vec2(window_size.x * 0.5f, window_size.y * 0.45f);
		glm::vec2 how_to_play_button_pos = glm::vec2(window_size.x * 0.5f, window_size.y * 0.325f);
		glm::vec2 quit_button_pos = glm::vec2(window_size.x * 0.5f, window_size.y * 0.2f);

		title->draw(title_pos);
		start_button->draw(start_button_pos);
		how_to_play_button->draw(how_to_play_button_pos);
		quit_button->draw(quit_button_pos);
	}
	
	GL_ERRORS();
}