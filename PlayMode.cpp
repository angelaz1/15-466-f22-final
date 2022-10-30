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

Load< Sound::Sample > proto_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("levels/proto/proto.wav"));
});

void PlayMode::start_level(Load<Sound::Sample> sample) {
	Sound::play(*sample, 1.0f, 0.0f);
	song_start_time = std::chrono::system_clock::now();
	current_beatmap.start();
}

PlayMode::PlayMode() {

	// set current beatmap
	current_beatmap = Beatmap("levels/proto/proto.beatmap", 41);

	// load dialogue
	current_dialogue = Dialogue();
	std::vector<std::string> choices;
	std::string str1("good");
	std::string str2("bad");
	choices.push_back(str1);
	choices.push_back(str2);
	current_dialogue.set_dialogue("Hey, how are you?", choices, true);
}

PlayMode::~PlayMode() {
}

// handle key presses
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	static bool key_down = false;
	// check keys pressed
	if (evt.type == SDL_KEYDOWN) {
		
		// only once per key down
		if (!key_down) {
			// register key press time
			auto key_time = std::chrono::system_clock::now();
			float key_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();

			// check key against beatmap
			if (current_beatmap.finished) { // all keys have been accounted for
				std::cout << "no more keys left; non-choice score = " << current_beatmap.non_choice_score() << std::endl;
				std::cout << "A score: " << current_beatmap.avg_a_score() << std::endl;
				std::cout << "B score: " << current_beatmap.avg_b_score() << std::endl;
			}
			else {
				current_beatmap.score_key(key_elapsed, evt.key.keysym.sym);
			}
			// set key_down to true to prevent double counting
			key_down = true;
		}

		return true;
	}
	else if (evt.type == SDL_KEYUP) {
		// reset key down
		key_down = false;
		return true;
	}

	return false;
}


void PlayMode::update(float elapsed) {
	if(time_elapsed > 1) {
        time_elapsed = 1;
    } else {
        time_elapsed += elapsed;
    }

	// TODO: function to set "started" to true to start the fade in process
	
	// start rhythm level when fade complete
	if (rhythm_ui_alpha < 1.0f && !current_beatmap.started && !current_beatmap.in_progress) {
		// update rhythm ui alpha
		rhythm_ui_fade_elapsed += elapsed;
		rhythm_ui_alpha = rhythm_ui_fade_elapsed / rhythm_ui_fade_time;
		if (rhythm_ui_alpha >= 1.0f) {
			rhythm_ui_alpha = 1.0f;
			start_level(proto_sample);
		}
	}
	// end rhythm level when fade complete
	else if (rhythm_ui_alpha > 0.0f && current_beatmap.finished && current_beatmap.in_progress) {
		
		// update rhythm ui alpha
		rhythm_ui_fade_elapsed += elapsed;
		rhythm_ui_alpha = 1.0f - (rhythm_ui_fade_elapsed / rhythm_ui_fade_time);
		if (rhythm_ui_alpha <= 0.0f) {
			rhythm_ui_alpha = 0.0f;
			current_beatmap.in_progress = false;
		}
	}
	else {
		// reset rhythm ui fade time
		rhythm_ui_fade_elapsed = 0.0f;
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size, glm::uvec2 const &window_size) {

	glClearColor(bg_color.r,bg_color.g,bg_color.b,1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{ // render arrows
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST); 
		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// empty arrows
		current_beatmap.draw_empty_arrows(window_size, rhythm_ui_alpha);

		auto key_time = std::chrono::system_clock::now();
		float song_time_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();

		if (current_beatmap.in_progress) {
			current_beatmap.draw_arrows(window_size, song_time_elapsed);
		}
	}

	current_dialogue.draw_dialogue_box(window_size);
	
	GL_ERRORS();
}