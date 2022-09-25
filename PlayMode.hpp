#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include "TextRendering.hpp"
#include "data_path.hpp"
#include "RoomParser.hpp"

#define MARGIN 5.

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	RoomParser room_parser;

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size, glm::uvec2 const &window_size) override;

	//----- game state -----

    Room current_room;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	// font renderers 
	TextRenderer *roboto_renderer = new TextRenderer(data_path("Roboto-Medium.ttf"), 48);
	TextRenderer *rubik_renderer = new TextRenderer(data_path("RubikDirt-Regular.ttf"), 32);
	TextRenderer *patua_renderer = new TextRenderer(data_path("PatuaOne-Regular.ttf"), 54);
	
	// configuration for main text, choices and inputs
	TextRenderer *main_text_renderer = patua_renderer;
	TextRenderer *choice_renderer = roboto_renderer;
	TextRenderer *input_renderer = rubik_renderer;

    float main_text_size = 0.5f;
    glm::vec3 main_text_color = glm::vec3(1.0f, 1.0f, 1.0f);

	//user input:
	std::string user_input;
	glm::vec3 input = glm::vec3(MARGIN,MARGIN,1.0f); //x pos, y pos, scale
	glm::vec3 input_color = glm::vec3(0.9f, 0.5f, 0.9f); //input text color

    //choices text:
    float choices_text_size = 0.5f;
    glm::vec3 choices_color = glm::vec3(0.6f, 1.0f, 0.7f);

	//checking if there's a match in room map
	std::string check_map(std::string inputString); //if no match, return empty string
	std::string new_room; //store map value of check_map here

	// background color:
	glm::vec3 bg_color = glm::vec3(0.0f, 0.0f, 0.0f);

};
