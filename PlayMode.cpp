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
	return new Sound::Sample(data_path("levels/proto/bourree.wav"));
});

void PlayMode::start_level(Load<Sound::Sample> sample) {
	Sound::play(*sample, 1.0f, 0.0f);
	song_start_time = std::chrono::system_clock::now();
	current_beatmap.start();
}

PlayMode::PlayMode() {
	// set current beatmap
	current_beatmap = Beatmap();

	dialogue_manager = new DialogueManager();
	current_tree = dialogue_manager->get_dialogue_tree("prototype");
	current_tree->start_tree();

	// load dialogue
	current_dialogue = Dialogue();

	current_dialogue.set_dialogue(current_tree->current_node, false);
}

PlayMode::~PlayMode() {}

// handle key presses
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	static bool key_down = false;
	// check keys pressed
	if (evt.type == SDL_KEYDOWN) {
		// only once per key down
		if (!key_down) {
			if (current_beatmap.in_progress) {
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
			} else if (!current_beatmap.started && evt.key.keysym.sym == SDLK_RETURN) {
				// Advance text based on current choice
				// Get the next node to advance to
				bool in_beatmap = false;
				if (current_tree->current_node->startBeatmap) {
					current_beatmap = Beatmap(current_tree->current_node->beatmapPath, 41);
					current_beatmap.started = true;
					in_beatmap = true;
				}

				current_tree->choose_choice(current_choice_index);
				current_choice_index = 0;
				current_dialogue.set_choice_selected(current_choice_index);
				current_dialogue.set_dialogue(current_tree->current_node, in_beatmap);
			} else if (!current_beatmap.started && evt.key.keysym.sym == SDLK_UP) {
				// Change choice selected
				if (current_choice_index != 0) current_choice_index--;
				current_dialogue.set_choice_selected(current_choice_index);
			} else if (!current_beatmap.started && evt.key.keysym.sym == SDLK_DOWN) {
				// Change choice selected
				if (current_choice_index < current_tree->current_node->choices.size() - 1) current_choice_index++;
				current_dialogue.set_choice_selected(current_choice_index);
			}
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
	if (rhythm_ui_alpha < 1.0f && current_beatmap.started && !current_beatmap.in_progress) {
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

	if (current_beatmap.beatmap_done()) {
		// Everything is done for the beatmap
		// Progress based on a_score/b_score to next dialogue option

		std::cout << "A score for choice: " << current_beatmap.avg_a_score() << std::endl;
		std::cout << "B score for choice: " << current_beatmap.avg_b_score() << std::endl;
		std::cout << "Non choice score: " << current_beatmap.non_choice_score() << std::endl;
		// Get the next node to advance to
		// TODO: revamp to include hesitation option and failure option
		if (current_beatmap.avg_a_score() > current_beatmap.avg_b_score()) {
			current_tree->choose_choice(0);
		} else {
			current_tree->choose_choice(1);
		}

		current_dialogue.set_dialogue(current_tree->current_node, false);

		// Reset the beatmap
		current_beatmap = Beatmap();
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