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

Load< Sound::Sample > violin1_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("levels/violin1/allemanda.wav"));
});

Load< Sound::Sample > violin2_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("levels/violin2/bourree.wav"));
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
	current_tree = dialogue_manager->get_dialogue_tree("violin");
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
			/** DEBUG KEY TO JUMP TO BEATMAPS **/
			if (!current_beatmap.started && evt.key.keysym.sym == SDLK_m) {
				current_tree->jump_to_next_beatmap();
				current_dialogue.set_dialogue(current_tree->current_node, false);
			}

			if (current_beatmap.in_progress) {
				// register key press time
				auto key_time = std::chrono::system_clock::now();
				float key_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();

				// check key against beatmap
				if (current_beatmap.finished) { // all keys have been accounted for
					std::cout << "no more keys left, finishing level" << std::endl;
				}
				else {
					current_beatmap.score_key(key_elapsed, evt.key.keysym.sym);
				}
				// set key_down to true to prevent double counting
				key_down = true;
			} else if (!current_beatmap.started && evt.key.keysym.sym == SDLK_RETURN) {
				if (!current_dialogue.finished_text_rendering()) {
					current_dialogue.finish_text_rendering();
				} else {
					// Advance text based on current choice
					// Get the next node to advance to
					if (current_tree->current_node->choices.size() > 0) {
						bool in_beatmap = false;
						if (current_tree->current_node->startBeatmap) {
							current_beatmap = Beatmap(current_tree->current_node->beatmapPath);
							current_beatmap.started = true;
							in_beatmap = true;
						}

						current_tree->choose_choice(current_choice_index);
						current_choice_index = 0;
						current_dialogue.set_choice_selected(current_choice_index);
						current_dialogue.set_dialogue(current_tree->current_node, in_beatmap);
					}
				}

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

	// update fade alphas if current beatmap in progress
	if (current_beatmap.in_progress) {
		current_beatmap.update_alphas(elapsed);
	}

	// start rhythm level when fade complete
	if (rhythm_ui_alpha < 1.0f && current_beatmap.started && !current_beatmap.in_progress) {
		// update rhythm ui alpha
		rhythm_ui_fade_elapsed += elapsed;
		rhythm_ui_alpha = rhythm_ui_fade_elapsed / rhythm_ui_fade_time;
		if (rhythm_ui_alpha >= 1.0f) {
			rhythm_ui_alpha = 1.0f;
			start_level(violin1_sample); // TODO: map of beatmap-samples or encode samples into beatmap
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

		// Add to relationship scoring based on accuracy
		current_tree->relationship_points += current_beatmap.get_final_score() * current_tree->current_node->songScoring;

		// Get the next node to advance to based on beatmap results
		current_tree->choose_choice(current_beatmap.get_choice());
		current_dialogue.set_dialogue(current_tree->current_node, false);

		// Reset the beatmap
		current_beatmap = Beatmap();
	}

	current_dialogue.update_dialogue_box(elapsed);
}

void PlayMode::draw(glm::uvec2 const &drawable_size, glm::uvec2 const &window_size) {

	glClearColor(bg_color.r,bg_color.g,bg_color.b,1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); 
	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	{ // render dialogue and background
		current_dialogue.draw_dialogue_box(window_size);
	}

	{ // render arrows
		auto key_time = std::chrono::system_clock::now();
		float song_time_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();

		if (current_beatmap.in_progress) {
			current_beatmap.draw_arrows(window_size, song_time_elapsed);
			current_beatmap.draw_empty_arrow_glow(window_size, GLOW_COLOR_SOLID);
		}

		current_beatmap.draw_game_ui(window_size, rhythm_ui_alpha);
	}

	
	
	GL_ERRORS();
}