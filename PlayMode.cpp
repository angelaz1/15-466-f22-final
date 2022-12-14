#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Beatmap.hpp"

#include "Framebuffers.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <fstream>

#include <ctype.h>

#define FONT "Roboto-Medium.ttf"

Sound::Sample violin1 = Sound::Sample(data_path("levels/violin1/allemanda.opus"));
Sound::Sample violin2 = Sound::Sample(data_path("levels/violin2/bourree.opus"));
Sound::Sample violin3 = Sound::Sample(data_path("levels/violin3/double_presto.opus"));

Sound::Sample tutorial1 = Sound::Sample(data_path("levels/tutorial/winter_wind_1.opus"));
Sound::Sample tutorial2 = Sound::Sample(data_path("levels/tutorial/winter_wind_2.opus"));
Sound::Sample tutorial_boss = Sound::Sample(data_path("levels/tutorial/winter_wind_boss.opus"));

Sound::Sample piano1 = Sound::Sample(data_path("levels/piano1/vocalise.opus"));
Sound::Sample piano2 = Sound::Sample(data_path("levels/piano2/mephisto.opus"));
Sound::Sample piano3 = Sound::Sample(data_path("levels/piano3/marcia.opus"));
Sound::Sample piano4 = Sound::Sample(data_path("levels/piano4/sacrificial_dance.opus"));
Sound::Sample piano5 = Sound::Sample(data_path("levels/piano5/waltz1.opus"));
Sound::Sample piano6 = Sound::Sample(data_path("levels/piano6/waltz2.opus"));

Sound::Sample *findSample (std::string name) {
	if (name.find("violin1") != std::string::npos) {
		return &violin1;
	}
	else if (name.find("violin2") != std::string::npos) {
		return &violin2;
	}
	else if (name.find("violin3") != std::string::npos) {
		return &violin3;
	}
	else if (name.find("tutorial") != std::string::npos) {
		if (name.find("boss") != std::string::npos) {
			return &tutorial_boss;
		}
		else if (name.find("1") != std::string::npos) {
			return &tutorial1;
		}
		else if (name.find("2") != std::string::npos) {
			return &tutorial2;
		}
		std::cout << "ERROR: Could not find tutorial sample for "  << name << std::endl;
		return nullptr;
	}
	else if (name.find("piano1") != std::string::npos) {
		return &piano1;
	}
	else if (name.find("piano2") != std::string::npos) {
		return &piano2;
	}
	else if (name.find("piano3") != std::string::npos) {
		return &piano3;
	}
	else if (name.find("piano4") != std::string::npos) {
		return &piano4;
	}
	else if (name.find("piano5") != std::string::npos) {
		return &piano5;
	}
	else if (name.find("piano6") != std::string::npos) {
		return &piano6;
	}

	else {
		std::cout << "ERROR: No sample found for " << name << std::endl;
		return nullptr;
	}
}

void PlayMode::start_level() {
	song_start_time = std::chrono::system_clock::now();
	current_beatmap.start();
	// play music
    current_sample = Sound::play(*(current_beatmap.sample), 1.0f, 0.0f);
}

PlayMode::PlayMode() {
	// set current beatmap
	current_beatmap = Beatmap();

	current_tree = DialogueManager::GetInstance()->get_dialogue_tree("start");
	current_tree->start_tree();

	// load dialogue
	current_dialogue = Dialogue();

	current_dialogue.set_dialogue(current_tree->current_node, false);
}

PlayMode::~PlayMode() {}

static int translate_key(SDL_Keycode key) {
	switch (key) {
		case SDLK_UP: return 0;
		case SDLK_DOWN: return 1;
		case SDLK_LEFT: return 2;
		case SDLK_RIGHT: return 3;
		default: return -1;
	}
}

void PlayMode::set_dialogue_tree(std::string dialogue_name) {
	current_tree = DialogueManager::GetInstance()->get_dialogue_tree(dialogue_name);
	current_tree->start_tree();
	current_dialogue.set_dialogue(current_tree->current_node, false);
}

// handle key presses
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	static std::vector<bool> arrow_pressed = std::vector<bool>(4, false);

	// check key pressed
	if (evt.type == SDL_KEYDOWN) {
		// escape key takes you to main menu
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			// Stop any music from beatmap
			if (current_beatmap.in_progress) {
				current_sample->stop(0.5);
			}
			Mode::set_current(std::make_shared< MainMenuMode >());
		}

		/** FIXME: DEBUG KEY TO JUMP TO BEATMAPS **/
		if (!current_beatmap.started && evt.key.keysym.sym == SDLK_m) {
			current_tree->jump_to_next_beatmap();
			current_dialogue.set_dialogue(current_tree->current_node, false);
		}

		if (current_beatmap.in_progress) {
			// check if key is arrow, score only if it is unpressed 
			SDL_Keycode key_pressed = evt.key.keysym.sym;
			int arrow_index = translate_key(key_pressed);

			if (arrow_index != -1 && !arrow_pressed[arrow_index]) {// valid and unpressed arrow
				// set key_down to true to prevent double counting
				arrow_pressed[arrow_index] = true;

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
			}
		} else if (!current_beatmap.started && evt.key.keysym.sym == SDLK_RETURN) {
			if (!current_dialogue.finished_text_rendering()) {
				current_dialogue.finish_text_rendering();
				// if multiple choices, set a timer so we can't choose a choice immediately
				if (current_dialogue.choices.size() > 1) {
					time_since_enter = 0.0f;
				}
			} else if (time_since_enter > delay_after_enter) {
				// Advance text based on current choice
				// Get the next node to advance to
				if (current_tree->current_node->choices.size() > 0) {
					bool in_beatmap = false;
					if (current_tree->current_node->startBeatmap) {
						std::string beatmapPath = current_tree->current_node->beatmapPath;
						current_beatmap = Beatmap(beatmapPath, findSample(beatmapPath));
						current_beatmap.started = true;
						in_beatmap = true;
					}

					DialogueTree *new_tree = current_tree->choose_choice(current_choice_index);
					if (new_tree != nullptr) {
						current_tree = new_tree;
						current_tree->start_tree();
					}

					current_choice_index = 0;
					current_dialogue.set_choice_selected(current_choice_index);
					current_dialogue.set_dialogue(current_tree->current_node, in_beatmap);
				} else {
					Mode::set_current(std::make_shared< MainMenuMode >());
				}
			}
		} else if (!current_beatmap.started && evt.key.keysym.sym == SDLK_UP) {
			// Change choice selected
			if (current_choice_index != 0) {
				current_choice_index--;
				SFXManager::GetInstance()->play_one_shot("lowblip", 0.1f);
			}
			current_dialogue.set_choice_selected(current_choice_index);
		} else if (!current_beatmap.started && evt.key.keysym.sym == SDLK_DOWN) {
			// Change choice selected
			if (current_choice_index < current_tree->current_node->choices.size() - 1) {
				current_choice_index++;
				SFXManager::GetInstance()->play_one_shot("lowblip", 0.1f);
			}
			current_dialogue.set_choice_selected(current_choice_index);
		}
		return true;
	}
	else if (evt.type == SDL_KEYUP) {
		// reset key down
		SDL_Keycode key_pressed = evt.key.keysym.sym;
		int arrow_index = translate_key(key_pressed);
		if (arrow_index != -1) {// valid arrow
			arrow_pressed[arrow_index] = false;
		}
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
			start_level(); 
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

		// Add relationship score of current beatmap with inaccuracy penalty
		int points = current_tree->current_node->songScoring;
		float penalty = abs((1 - current_beatmap.get_final_score()) * points);
		current_tree->relationship_points += (int)(points - penalty);

		// Get the next node to advance to based on beatmap results
		DialogueTree *new_tree = current_tree->choose_choice(current_beatmap.get_choice());
		if (new_tree != nullptr) {
			current_tree = new_tree;
			current_tree->start_tree();
		}

		current_dialogue.set_dialogue(current_tree->current_node, false);

		// Reset the beatmap
		current_beatmap = Beatmap();
	}

	current_dialogue.update_dialogue_box(elapsed);

	// update time elapsed since we last pressed enter; used for delay when a choice appears
	time_since_enter += elapsed;
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
		if (current_beatmap.in_progress) {
			// if current beatmap playing, change the emotion based on score
			float non_choice_score = current_beatmap.get_non_choice_score();
			if (non_choice_score < 0) {
				current_dialogue.set_dialogue_emotion(DialogueNode::NEUTRAL);
			}
			else if (non_choice_score < FAIL_HIT) {
				current_dialogue.set_dialogue_emotion(DialogueNode::ANGRY);
			}
			else if (non_choice_score < GOOD_HIT) {
				current_dialogue.set_dialogue_emotion(DialogueNode::SAD);
			}
			else if (non_choice_score < GREAT_HIT) {
				current_dialogue.set_dialogue_emotion(DialogueNode::NEUTRAL);
			}
			else {
				current_dialogue.set_dialogue_emotion(DialogueNode::SMILE);
			}
		}
		current_dialogue.draw_dialogue_box(window_size);
	}

	{ // render arrows
		auto key_time = std::chrono::system_clock::now();
		float song_time_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();

		if (current_beatmap.in_progress) {
			current_beatmap.draw_arrows(window_size, song_time_elapsed);
			current_beatmap.draw_empty_arrow_glow(window_size, GLOW_COLOR_SOLID);
			current_beatmap.draw_scoring_text(window_size, GLOW_COLOR_SOLID);
		}

		current_beatmap.draw_game_ui(window_size, rhythm_ui_alpha);
	}

	// GL_ERRORS(); silencing errors, TODO: should fix in future
}