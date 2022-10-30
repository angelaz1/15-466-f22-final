#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "RoomParser.hpp"
#include "Beatmap.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <fstream>

#include <ctype.h>

#define FONT "Roboto-Medium.ttf"

PlayMode::PlayMode() {
    current_room = room_parser.parse_room("room0.txt");

	right_arrow = new Sprite("images/right.png");
	right_arrow_empty = new Sprite("images/right_empty.png");
	up_arrow = new Sprite("images/up.png");
	up_arrow_empty = new Sprite("images/up_empty.png");
	left_arrow = new Sprite("images/left.png");
	left_arrow_empty = new Sprite("images/left_empty.png");
	down_arrow = new Sprite("images/down.png");
	down_arrow_empty = new Sprite("images/down_empty.png");

	// load beatmap
	current_beatmap = Beatmap("levels/proto/proto.beatmap", 41);
	current_beatmap.print_beatmap();
}

PlayMode::~PlayMode() {
}

// [in] user's keyboard input
// [out] get final user's input in a string
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYUP) {
		if (isalpha(evt.key.keysym.sym)) { //user has typed in char
			if (std::isdigit((user_input.c_str())[0])) user_input = ""; //clear user input if number
			user_input.append(SDL_GetKeyName(evt.key.keysym.sym));
			return true;
		} else if (isdigit(evt.key.keysym.sym)) { //user has typed in num
			user_input = SDL_GetKeyName(evt.key.keysym.sym);
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN ||
				   evt.key.keysym.sym == SDLK_RETURN2 ||
				   evt.key.keysym.sym == SDLK_KP_ENTER) { // user has pressed enter (done typing)
			// call checking function here from Will
			if ((new_room = check_map(user_input)) != "") {
                time_elapsed = 0;
				current_room = room_parser.parse_room(new_room);
			}
			user_input = ""; //clear user input string for next enter
			return true;
		} else if (evt.key.keysym.sym == SDLK_BACKSPACE) { //delete last char
			user_input.pop_back();
			return true;
		} else if (evt.key.keysym.sym == SDLK_KP_SPACE ||
				   evt.key.keysym.sym == SDLK_SPACE) {  // add space
			user_input.append(" ");
			return true;
		}
	}

	return false;
}

std::string PlayMode::check_map(std::string inputString) {
	std::cout<<"checking map with input '"<<inputString<<"'"<<std::endl;
	if (current_room.inputs.find(inputString) != current_room.inputs.end()) {
		std::cout<<"MATCHED with '"<<current_room.inputs.find(inputString)->second<<std::endl;
		return current_room.inputs.find(inputString)->second;
	}
	std::cout<<"NO MATCH FOUND..."<<std::endl;
	return "";
}

void PlayMode::update(float elapsed) {
	if(time_elapsed > 1) {
        time_elapsed = 1;
    } else {
        time_elapsed += elapsed;
    }

	song_time_elapsed += elapsed;
}

void PlayMode::draw(glm::uvec2 const &drawable_size, glm::uvec2 const &window_size) {

	glClearColor(bg_color.r,bg_color.g,bg_color.b,1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    { //render text
		// set drawable size to window
		main_text_renderer->set_drawable_size(window_size);
		choice_renderer->set_drawable_size(window_size);
		input_renderer->set_drawable_size(window_size);

        glm::vec3 zero(0,0,0);
        //main text
        main_text_renderer->renderWrappedText(current_room.main_text, 10.0f, main_text_size, lerp(zero, main_text_color, time_elapsed), true);

        std::string choices;
        //choices
        for(uint16_t i = 0; i < current_room.choices.size(); i++) {
            choices.append(current_room.choices.at(i));
            choices.append("\n");
        }
        choice_renderer->renderWrappedText(choices, ((window_size.y / 4.0f) * 3.0f), choices_text_size, lerp(zero, choices_color, time_elapsed), true);

        //user input
        input_renderer->renderLine(user_input, input.x, input.y, input.z, input_color);
    }

	{ // render arrows
		up_arrow->set_drawable_size(window_size);
		up_arrow_empty->set_drawable_size(window_size);
		down_arrow->set_drawable_size(window_size);
		down_arrow_empty->set_drawable_size(window_size);
		left_arrow->set_drawable_size(window_size);
		left_arrow_empty->set_drawable_size(window_size);
		right_arrow->set_drawable_size(window_size);
		right_arrow_empty->set_drawable_size(window_size);
		const float arrow_size = 0.25f;

		// render arrows that are at end of screen as target
		const float x_pos_destination = 70.0f;
		const glm::uvec2 up_arrow_destination = glm::uvec2(x_pos_destination, 4 * window_size.y / 5);
		const glm::uvec2 down_arrow_destination = glm::uvec2(x_pos_destination, 3 * window_size.y / 5);
		const glm::uvec2 left_arrow_destination = glm::uvec2(x_pos_destination, 2 * window_size.y / 5);
		const glm::uvec2 right_arrow_destination = glm::uvec2(x_pos_destination, 1 * window_size.y / 5);
		up_arrow_empty->draw(up_arrow_destination, arrow_size);
		down_arrow_empty->draw(down_arrow_destination, arrow_size);
		left_arrow_empty->draw(left_arrow_destination, arrow_size);
		right_arrow_empty->draw(right_arrow_destination, arrow_size);

		// render arrows from beatmap
		const float arrow_speed = 200.0f;
		for (unsigned int i = 0; i < current_beatmap.keys.size(); i++) {
			float arrow_x_pos = x_pos_destination + (current_beatmap.timestamps[i] / 1000.0f - song_time_elapsed) * arrow_speed;
			switch (current_beatmap.keys[i])
			{
			case 0:
				up_arrow->draw(glm::uvec2(arrow_x_pos, up_arrow_destination.y), arrow_size);
				break;
			case 1:
				down_arrow->draw(glm::uvec2(arrow_x_pos, down_arrow_destination.y), arrow_size);
				break;
			case 2:
				left_arrow->draw(glm::uvec2(arrow_x_pos, left_arrow_destination.y), arrow_size);
				break;
			case 3:
				right_arrow->draw(glm::uvec2(arrow_x_pos, right_arrow_destination.y), arrow_size);
				break;
			case 5:
				up_arrow->draw(glm::uvec2(arrow_x_pos, up_arrow_destination.y), arrow_size);
				left_arrow->draw(glm::uvec2(arrow_x_pos, left_arrow_destination.y), arrow_size);
				break;
			case 6:
				down_arrow->draw(glm::uvec2(arrow_x_pos, down_arrow_destination.y), arrow_size);
				right_arrow->draw(glm::uvec2(arrow_x_pos, right_arrow_destination.y), arrow_size);
				break;
			default:
				std::cout << "Invalid key provided for beatmap." << std::endl;
				break;
			}
		}
	}
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