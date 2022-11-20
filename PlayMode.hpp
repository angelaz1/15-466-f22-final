#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <chrono>

#include "Dialogue.hpp"
#include "data_path.hpp"
#include "RoomParser.hpp"
#include "Beatmap.hpp"
#include "DialogueManager.hpp"

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
		float timestamp = 0.0f;
	} left, right, down, up, ret;
	float time_since_enter;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	// Working with dialogue
	DialogueManager *dialogue_manager;
	DialogueTree *current_tree;
	size_t current_choice_index = 0;

	// Displaying/moving arrows for our rhythm game
	Beatmap current_beatmap;
	std::chrono::time_point<std::chrono::system_clock> song_start_time;

	// rhythm UI alpha and fading logic
	float rhythm_ui_fade_time = 2.0f;
	float rhythm_ui_alpha = 0.0f;
	float rhythm_ui_fade_elapsed = 0.0f;

	// rhythm game start and end
	void start_level();
	
	// configuration for main text, choices and inputs
	Dialogue current_dialogue;

	// background color:
	glm::vec3 bg_color = glm::vec3(0.3f, 0.3f, 0.3f);

    float time_elapsed;
    float fade_time;

    //lerp copied from https://graphicscompendium.com/opengl/22-interpolation
    static glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return x * (1.f - t) + y * t;
    }

};
