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

enum arrowType_t {
    UP_ARROW = 0,
    DOWN_ARROW = 1,
    LEFT_ARROW = 2,
    RIGHT_ARROW = 3,
    UNDEFINED_ARROW = 255
};

enum resultChoice_t {
    RESULT_A = 0,
    RESULT_B = 1,
    RESULT_BOTH = 2,
    RESULT_FAIL = 3
};

struct Beatmap {
    std::vector<float> timestamps;
    std::vector<uint8_t> keys;

    size_t num_notes = 0;
    size_t a_notes = 0;
    size_t b_notes = 0;

    bool started = false;
    bool in_progress = false;
    bool finished = false;

    // quick check for if beatmap is done
    bool beatmap_done() {
        return finished && !in_progress;
    }

    size_t curr_index = 0;
    
    float a_score = 0;
    float b_score = 0;
    float other_score = 0;

    Beatmap();
    Beatmap(std::string fname, uint32_t num_notes);
    ~Beatmap(); 

    // start level
    void start();

    // translates SDL keypress to uint8_t key
    arrowType_t translate_key(SDL_Keycode key); 

    // scores a key press at timestamp
    // returns true if has keys left to score, false if all keys scored
    bool score_key(float key_timestamp, SDL_Keycode sdl_key);

    // returns resulting choice after completion of beatmap
    resultChoice_t get_choice();

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