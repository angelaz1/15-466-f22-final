#pragma once

#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>  // for std::setprecision
#include <sstream>
#include <vector>

#include "constants.hpp"
#include "data_path.hpp"
#include "Mode.hpp"
#include "Sprite.hpp"
#include "Sound.hpp"
#include "Load.hpp"
#include "TextRendering.hpp"

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

enum arrowState_t {
    BASE, HIT, MISSED, HIDE, HIT_FADE, MISS_FADE, LOW_SCORE, LOW_SCORE_FADE
};

struct Fade {
    float alpha = 0.0f;
    float max_alpha = 1.0f;
    float residual_alpha = 0.0f;

    float in_time = 0.0f;
    float out_time = 0.0f;

    float elapsed = 0.0f;

    enum mode {IN, OUT, HOLD} mode = HOLD;
    enum FadeType {ONCE, SUSTAIN, OUT_ONLY} fade_type = SUSTAIN;
    enum FadeCurve {LINEAR, INVSQ} fade_curve = LINEAR;

    Fade() = default;
    Fade(float in_time, float out_time, FadeType fade_type, FadeCurve fade_curve) {
        this->in_time = in_time;
        this->out_time = out_time;
        this->fade_curve = fade_curve;
        if (fade_type == OUT_ONLY) {
            this->fade_type = ONCE;
            solid();
        }
        else {
            this->fade_type = fade_type;
        }
    };

    ~Fade() = default;

    // activates fade
    void fade_in(float max_a = 1.0f) { 
        mode = IN;
        max_alpha = max_a;
        residual_alpha = alpha;
        elapsed = 0.0f;
    }

    // updates using elapsed time
    // returns true if fade complete
    bool update(float delta) {
        elapsed += delta;

        if (mode == IN) {
            if (elapsed >= in_time) {
                alpha = max_alpha;
                mode = HOLD; 
                if (fade_type == ONCE) {
                    fade_out(); // immediately fades out
                }
                return true;
            }
            else {
                if (fade_curve == LINEAR) {
                    alpha = residual_alpha + max_alpha * (elapsed / in_time);
                }
                else if (fade_curve == INVSQ) {
                    alpha = residual_alpha + max_alpha * (1.0f - (1.0f - (elapsed / in_time)) * (1.0f - (elapsed / in_time)));
                }
                return false;
            }
            
        } else if (mode == OUT) {
            if (elapsed >= out_time) {
                alpha = 0.0f;
                mode = HOLD; // does not fade back in
                return true;
            }
            else {
                if (fade_curve == LINEAR) {
                    alpha = max_alpha * (1.0f - (elapsed / out_time));
                }
                else if (fade_curve == INVSQ) {
                    alpha = max_alpha * ((1.0f - (elapsed / out_time)) * (1.0f - (elapsed / out_time)));
                }
                return false;
            }
        }
        else {
            return false; // holding
        }

    }
    void solid() {
        mode = HOLD;
        alpha = max_alpha;
    }

    void fade_out () {
        mode = OUT;
        elapsed = 0.0f;
    }

};

struct Beatmap {
    std::vector<float> timestamps;
    std::vector<uint8_t> keys;
    std::vector<arrowState_t> states;
    std::vector<Fade> fades;

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
    size_t curr_draw_index = 0;
    
    float a_score = 0;
    float b_score = 0;
    float other_score = 0;

    size_t scored_a_notes = 0;
    size_t scored_b_notes = 0;
    size_t scored_other_notes = 0;

    // alphas for fading in/out
    std::vector<Fade> glow_fades;

    Beatmap();
    Beatmap(std::string fname, uint32_t num_notes);
    ~Beatmap(); 

    void load_sprites();

    // start level
    void start();

    // translates SDL keypress to uint8_t key
    arrowType_t translate_key(SDL_Keycode key); 

    // scores a key press at timestamp
    // returns true if has keys left to score, false if all keys scored
    bool score_key(float key_timestamp, SDL_Keycode sdl_key);

    // update fade alphas
    void update_alphas(float elapsed);

    // returns resulting choice after completion of beatmap
    resultChoice_t get_choice();
    // returns resulting score after completion of beatmap
    float get_final_score();

    // draw arrows associated with beatmap
    Sprite *right_arrow;
	Sprite *right_arrow_empty;
	Sprite *left_arrow;
	Sprite *left_arrow_empty;
	Sprite *up_arrow;
	Sprite *up_arrow_empty;
	Sprite *down_arrow;
	Sprite *down_arrow_empty;

    // drawing rhythm game ui
    Sprite *choice_bar_a;
    Sprite *choice_bar_b;
    Sprite *choice_indicator;

    // draw beatmap arrows
    Sprite *right_arrow_glow;
    Sprite *left_arrow_glow;
    Sprite *up_arrow_glow;
    Sprite *down_arrow_glow;

    // UI text renderer
    TextRenderer *vt323_renderer = new TextRenderer(data_path("fonts/VT323-Regular.ttf"), 20);

    // Configuration for main text, choices and inputs
	TextRenderer *scoring_text_renderer = vt323_renderer;

    // all other game ui
    void draw_game_ui(glm::uvec2 const &window_size, float alpha = 1.0f);

    // empty arrows as part of rhythm game UI
    void draw_empty_arrows(glm::uvec2 const &window_size, float alpha = 1.0f, glm::u8vec4 hue = glm::u8vec4(255, 255, 255, 255));
    void draw_empty_arrow_glow(glm::uvec2 const &window_size, glm::u8vec4 hue = glm::u8vec4(255, 255, 255, 255));
    
    void draw_arrows(glm::uvec2 const &window_size, float song_time_elapsed);

    // debug purposes
    void print_beatmap();  
};