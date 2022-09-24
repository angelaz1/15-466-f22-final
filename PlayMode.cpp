#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <fstream>

#include <ctype.h>

#define TESTSTR "Hello, world"
#define FONT "Roboto-Medium.ttf"

PlayMode::PlayMode() {

	std::cout << data_path(FONT) << std::endl;
	
}

PlayMode::~PlayMode() {
}

// [in] user's keyboard input
// [out] get final user's input in a string
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYUP) {
		if (isalpha(evt.key.keysym.sym)) { //user has typed in char
			user_input.append(SDL_GetKeyName(evt.key.keysym.sym));
			return true;
		} else if (isdigit(evt.key.keysym.sym)) { //user has typed in num
			user_input = SDL_GetKeyName(evt.key.keysym.sym);
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN ||
				   evt.key.keysym.sym == SDLK_RETURN2 ||
				   evt.key.keysym.sym == SDLK_KP_ENTER) { // user has pressed enter (done typing)
			// call checking function here from Will
			std::cout<<"user input: "<<user_input<<std::endl;
			user_input = ""; //clear user input string for next enter
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	text_renderer->renderText("test kjhkshlukh\nlkwejrhgoiuowyps30598720398476\noiufskjhlkjxhcvb", 25.0f, 25.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
	text_renderer->renderLine(user_input, 400.f, 400.f, 1.0f, glm::vec3(0.9f, 0.5f, 0.9f));
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	

	

	



	

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	// glUseProgram(lit_color_texture_program->program);
	// glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	// glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	// glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	// glUseProgram(0);

	// glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	// glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	// { //use DrawLines to overlay some text:
	// 	glDisable(GL_DEPTH_TEST);
	// 	float aspect = float(drawable_size.x) / float(drawable_size.y);
	// 	DrawLines lines(glm::mat4(
	// 		1.0f / aspect, 0.0f, 0.0f, 0.0f,
	// 		0.0f, 1.0f, 0.0f, 0.0f,
	// 		0.0f, 0.0f, 1.0f, 0.0f,
	// 		0.0f, 0.0f, 0.0f, 1.0f
	// 	));

	// }
	GL_ERRORS();
}