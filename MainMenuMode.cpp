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
	
	GL_ERRORS();
}