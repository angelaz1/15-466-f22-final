#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "data_path.hpp"
#include "Mode.hpp"
#include "Sprite.hpp"

#define FULL_SCORE_THRESHOLD (0.05f * 0.05f)
#define NO_SCORE_THRESHOLD (0.25f * 0.25f)

struct Beatmap {
    std::vector<float> timestamps;
    std::vector<uint8_t> keys;
    size_t num_notes;
    unsigned int curr_index = 0;
    float total_score = 0;

    Beatmap();
    Beatmap(std::string fname, uint32_t num_notes);
    ~Beatmap(); 

    // squared difference used in scoring
    float sqdiff (float a, float b) {
        return (a - b) * (a - b);
    }

    // scores a key press at timestamp
    // returns true if has keys left to score, false if all keys scored
    bool score_key(float key_timestamp, uint8_t key);

    // returns total score divided by number of notes
    float avg_score() {
        return total_score / num_notes;
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
    void draw_arrows(glm::uvec2 const &window_size, float song_time_elapsed);

    // debug purposes
    void print_beatmap();  
};

// translates SDL keypress to uint8_t key
// TODO: incorporate choices somehow?
uint8_t translate_key(SDL_Keycode key); 