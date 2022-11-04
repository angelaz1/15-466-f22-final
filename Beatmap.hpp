#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

#include "constants.hpp"
#include "data_path.hpp"
#include "Mode.hpp"
#include "Sprite.hpp"
#include "Sound.hpp"
#include "Load.hpp"

#define FULL_SCORE_THRESHOLD (0.05f * 0.05f)
#define NO_SCORE_THRESHOLD (0.25f * 0.25f)
#define SCORE_BOUND 0.75f

#define UP_ARROW 0
#define DOWN_ARROW 1
#define LEFT_ARROW 2
#define RIGHT_ARROW 3

#define UNDEFINED_ARROW 255
struct Beatmap {
    std::vector<float> timestamps;
    std::vector<uint8_t> keys;

    size_t num_notes = 0;
    size_t a_notes = 0;
    size_t b_notes = 0;

    bool started = false;
    bool in_progress = false;
    bool finished = false;

    bool beatmap_done() {
        return finished && !in_progress;
    }

    size_t curr_index = 0;
    float total_score = 0;
    float a_score = 0;
    float b_score = 0;

    // arrow position and size
    float x_pos_ratio = 0.05f;
    float arrow_size = 0.125f;

    glm::vec2 up_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.9f);
    glm::vec2 down_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.8f);
    glm::vec2 left_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.7f);
    glm::vec2 right_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.6f);

    Beatmap();
    Beatmap(std::string fname, uint32_t num_notes);
    ~Beatmap(); 

    void start();

    // squared difference used in scoring
    float sqdiff (float a, float b) {
        return (a - b) * (a - b);
    }

    // translates SDL keypress to uint8_t key
    uint8_t translate_key(SDL_Keycode key); 

    // scores a key press at timestamp
    // returns true if has keys left to score, false if all keys scored
    bool score_key(float key_timestamp, SDL_Keycode sdl_key);

    // total score logic
    float avg_a_score() {
        return a_score / a_notes;
    }

    float avg_b_score() {
        return b_score / b_notes;
    }

    float non_choice_score() {
        size_t non_choice_notes = num_notes - a_notes - b_notes;
        return total_score / non_choice_notes;
    }

    // draw arrows associated with beatmap
    Sprite *right_arrow;
	Sprite *right_arrow_empty;
	Sprite *left_arrow;
	Sprite *left_arrow_empty;
	Sprite *up_arrow;
	Sprite *up_arrow_empty;
	Sprite *down_arrow;
	Sprite *down_arrow_empty;

    // empty arrows as part of rhythm game UI
    void draw_empty_arrows(glm::uvec2 const &window_size, float alpha = 1.0f, glm::u8vec4 hue = glm::u8vec4(255, 255, 255, 255));

    void draw_arrows(glm::uvec2 const &window_size, float song_time_elapsed);

    // debug purposes
    void print_beatmap();  
};