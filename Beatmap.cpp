#include "Beatmap.hpp"

// squared difference used in scoring
float sqdiff (float a, float b) {
    return (a - b) * (a - b);
}

void Beatmap::load_sprites() {
    right_arrow = new Sprite(data_path("images/right.png"));
	right_arrow_empty = new Sprite(data_path("images/right_empty.png"));
	up_arrow = new Sprite(data_path("images/up.png"));
	up_arrow_empty = new Sprite(data_path("images/up_empty.png"));
	left_arrow = new Sprite(data_path("images/left.png"));
	left_arrow_empty = new Sprite(data_path("images/left_empty.png"));
	down_arrow = new Sprite(data_path("images/down.png"));
	down_arrow_empty = new Sprite(data_path("images/down_empty.png"));

    right_arrow_glow = new Sprite(data_path("images/right_glow.png"));
    left_arrow_glow = new Sprite(data_path("images/left_glow.png"));
    up_arrow_glow = new Sprite(data_path("images/up_glow.png"));
    down_arrow_glow = new Sprite(data_path("images/down_glow.png"));

    perfect_text = new Sprite(data_path("images/feedback/Perfect.png"));
    great_text = new Sprite(data_path("images/feedback/Great.png"));
    good_text = new Sprite(data_path("images/feedback/Good.png"));
    miss_text = new Sprite(data_path("images/feedback/Miss.png"));

    // initialize glow fades
    for (size_t i = 0; i < 4; i++) {
        glow_fades.push_back(Fade(GLOW_IN_TIME, GLOW_OUT_TIME, Fade::ONCE, Fade::INVSQ));
        score_text_fades.push_back(Fade(SCORE_TEXT_IN_TIME, SCORE_TEXT_OUT_TIME, Fade::ONCE, Fade::INVSQ));
    }

    // initialize 

    choice_bar_a = new Sprite(data_path("images/choice_bar_a.png"));
    choice_bar_b = new Sprite(data_path("images/choice_bar_b.png"));
    choice_indicator = new Sprite(data_path("images/choice_indicator.png"));
}

Beatmap::Beatmap() {
    num_notes = keys.size();

    // load sprites
    load_sprites();
}

Beatmap::Beatmap(std::string fname, Sound::Sample *sample) {
    // open file
    fname = data_path(fname);
    std::ifstream file(fname, std::ios::in | std::ios::binary);

    // error out if file doesn't exist
    if (!file.is_open()) {
        std::cout << "Error: File " << fname << " does not exist." << std::endl;
        throw std::runtime_error("Exiting with error.");
        return;
    }

    // read int for time and char for key
    uint32_t time_ms;
    uint8_t key;

    // get number of notes first
    file.read((char*)&time_ms, sizeof(uint32_t));
    file.read((char*)&key, sizeof(uint8_t));

    // number of notes stored in key
    num_notes = key;

    for (uint32_t i = 0; i < num_notes; i++) {

        // read int for time and char for key
        file.read((char*)&time_ms, sizeof(uint32_t));
        file.read((char*)&key, sizeof(uint8_t));

        // check if is up or down key, add to a/b notes if so
        if (key == UP_ARROW) {
            a_notes++;
        }
        else if (key == DOWN_ARROW) {
            b_notes++;
        }
        
        // push into array
        timestamps.push_back(time_ms / 1000.0f);
        keys.push_back(key); 
        states.push_back(BASE);
        fades.push_back(Fade(RHYTHM_ARROW_FADE_TIME, RHYTHM_ARROW_FADE_TIME, Fade::OUT_ONLY, Fade::INVSQ));
    }
    // save sound pointer
    this->sample = sample;
    // load sprites
    load_sprites();
}

Beatmap::~Beatmap() {
    glow_fades.clear();
    fades.clear();
    score_text_fades.clear();
    timestamps.clear();
    keys.clear();
    states.clear();
}

void Beatmap::print_beatmap() {
    for (size_t i = 0; i < timestamps.size(); i++) {
        printf("time: %f, key: %u, states: %u\n", timestamps[i], keys[i], states[i]);
    }
}

void Beatmap::start() {
    curr_index = 0;
    curr_draw_index = 0;
    
    a_score = 0;
    b_score = 0;
    other_score = 0;

    in_progress = true;
    started = true;
}

arrowType_t Beatmap::translate_key(SDL_Keycode key) {

    switch (key) {
        case SDLK_UP:
            return UP_ARROW;
        case SDLK_DOWN:
            return DOWN_ARROW;
        case SDLK_LEFT:
            return LEFT_ARROW;
        case SDLK_RIGHT:
            return RIGHT_ARROW;
        default:
            return UNDEFINED_ARROW;
    }
    
}

void Beatmap::trigger_score_text(float score) {
    if (score >= PERFECT_HIT) {
        score_text_fades[0].fade_in();
        score_text_fades[1].disappear();
        score_text_fades[2].disappear();
        score_text_fades[3].disappear();
    }
    else if (score >= GREAT_HIT) {
        score_text_fades[0].disappear();
        score_text_fades[1].fade_in();
        score_text_fades[2].disappear();
        score_text_fades[3].disappear();
    }
    else if (score <= FAIL_HIT) {
        score_text_fades[0].disappear();
        score_text_fades[1].disappear();
        score_text_fades[2].disappear();
        score_text_fades[3].fade_in();
    }
    else { // GOOD HIT
        score_text_fades[0].disappear();
        score_text_fades[1].disappear();
        score_text_fades[2].fade_in();
        score_text_fades[3].disappear();
    }
}


bool Beatmap::score_key(float key_timestamp, SDL_Keycode sdl_key) {
    // translate key
    arrowType_t key = translate_key(sdl_key);

    float curr_timestamp = timestamps[curr_index];
    // temporary debug print
    std::cout << "key " << curr_index << "/" << num_notes << " || correct key " << std::to_string(keys[curr_index]) << " at " << curr_timestamp << "s; hit " << std::to_string(key) << " at " << key_timestamp << std::endl;

    // check if in bounds of the current note
    if (abs(curr_timestamp - key_timestamp) >= SCORING_TIME_RANGE) {
        // early out
        std::cout << "Out of scoring bounds, skipping." << std::endl;
        return true;
    }

    // set total score buffer according to choice
    float *score_buffer;
    size_t *scored_notes;

    // add up arrow to A score, down arrow to B score
    if (keys[curr_index] == UP_ARROW) {
        score_buffer = &a_score;
        scored_notes = &scored_a_notes;
    } else if (keys[curr_index] == DOWN_ARROW) {
        score_buffer = &b_score;
        scored_notes = &scored_b_notes;
    } else {
        score_buffer = &other_score;
        scored_notes = &scored_other_notes;
    }


    // scoring
    float score = 0;

    // check if correct note
    if (key != keys[curr_index]) {
        // wrong note, no score
        states[curr_index] = MISSED;
        std::cout << "Incorrect key" << std::endl;
    }
    else {
        // score current note based on linear interpolation of square difference
        float diff = sqdiff(key_timestamp, curr_timestamp);
        
        // interpolate score
        score = 1.0f - (diff - FULL_SCORE_THRESH) / (NO_SCORE_THRESH - FULL_SCORE_THRESH);
        // apply max and min
        score = std::max(0.0f, score);
        score = std::min(1.0f, score);
        std::cout << "Score: " << score << std::endl;
        
        // add to total score
        *score_buffer += score;

        // animate glow according to score
        glow_fades[keys[curr_index]].fade_in(score * score);

        // set state to hit or low score
        if (score < LEVEL_FAIL_THRESH) {
            states[curr_index] = LOW_SCORE;
        } else {
            states[curr_index] = HIT;
        }   
    }

    // animate reaction according to score
    trigger_score_text(score);

    // increment notes scored
    (*scored_notes)++;
    // increment index to mark current note as scored
    curr_index++;
    

    // return false if end of beatmap
    if (curr_index == num_notes) {
        std::cout << "Beatmap complete!" << std::endl;
        finished = true;
        return false;
    }

    return true;
}

void Beatmap::update_alphas(float elapsed) {
    // update glow fades
    for (size_t i = 0; i < 4; i++) {
        glow_fades[i].update(elapsed);
        score_text_fades[i].update(elapsed);
    }
    for (size_t i = 0; i < num_notes; i++) {
        fades[i].update(elapsed);
    }
}

float Beatmap::get_non_choice_score() {
    // should only be used for setting emotions
    return (scored_other_notes == 0) ? -1 : other_score / scored_other_notes;
}

resultChoice_t Beatmap::get_choice() {
    assert (beatmap_done());
    
    float non_choice_score = other_score / (num_notes - a_notes - b_notes);
    std::cout << "Non-choice score: " << non_choice_score << std::endl;
    // early out if non-choice score is too low
    if (non_choice_score < LEVEL_FAIL_THRESH) {
        return RESULT_FAIL;
    }
    
    // calculate choice scores
    float avg_a_score = a_score / a_notes;
    float avg_b_score = b_score / b_notes;

    std::cout << "A score: " << avg_a_score << std::endl;
    std::cout << "B score: " << avg_b_score << std::endl;

    // early out if both choices were failed
    if (avg_a_score < LEVEL_FAIL_THRESH && avg_b_score < LEVEL_FAIL_THRESH) {
        return RESULT_FAIL;
    }

    // calculate difference
    float ab_diff = avg_a_score - avg_b_score;
    
    if (ab_diff > CHOICE_DIFF_THRESH) { // A has more score
        return RESULT_A;
    }
    else if (ab_diff < -CHOICE_DIFF_THRESH) { // B has more score
        return RESULT_B;
    }
    else { // both choices have same score
        return RESULT_BOTH;
    }

}

float Beatmap::get_final_score() {
    assert(beatmap_done());
    switch (get_choice()) {
        case RESULT_A:
            return (a_score + other_score) / (num_notes - b_notes);
        case RESULT_B:
            return (b_score + other_score) / (num_notes - a_notes);
        case RESULT_BOTH:
            return (a_score + b_score + other_score) / num_notes;
        default:
            return 0.0f;
    }
}

glm::vec2 norm_to_window(glm::vec2 const &normalized_coordinates, glm::uvec2 const &window_size) {
    return glm::vec2(normalized_coordinates.x * window_size.x, normalized_coordinates.y * window_size.y);
}

void Beatmap::draw_empty_arrows(glm::uvec2 const &window_size, float alpha, glm::u8vec4 hue) {
    // set alpha
    hue.a = (uint8_t)(255.0 * alpha);

    up_arrow_empty->set_drawable_size(window_size);
    down_arrow_empty->set_drawable_size(window_size);
    left_arrow_empty->set_drawable_size(window_size);
    right_arrow_empty->set_drawable_size(window_size);

    up_arrow_empty->draw(norm_to_window(up_arrow_destination_norm, window_size), arrow_size, hue);
    down_arrow_empty->draw(norm_to_window(down_arrow_destination_norm, window_size), arrow_size, hue);
    left_arrow_empty->draw(norm_to_window(left_arrow_destination_norm, window_size), arrow_size, hue);
    right_arrow_empty->draw(norm_to_window(right_arrow_destination_norm, window_size), arrow_size, hue);
}

void Beatmap::draw_game_ui(glm::uvec2 const &window_size, float alpha) {
    
    draw_empty_arrows(window_size, alpha, BASE_COLOR_SOLID);
    
    choice_bar_a->set_drawable_size(window_size);
    choice_bar_b->set_drawable_size(window_size);
    choice_indicator->set_drawable_size(window_size);

    glm::u8vec4 a_color = A_CHOICE_COLOR_SOLID;
    a_color.a = (uint8_t)(255.0 * alpha);

    glm::u8vec4 b_color = B_CHOICE_COLOR_SOLID;
    b_color.a = (uint8_t)(255.0 * alpha);

    choice_bar_a->draw(norm_to_window(bar_pos_norm, window_size), indicator_scale, a_color);
    choice_bar_b->draw(norm_to_window(bar_pos_norm, window_size), indicator_scale, b_color);

    // Compute indicator y position from a/b scores
    float non_choice_score = (scored_other_notes == 0) ? 0 : other_score / scored_other_notes;

    float avg_a_score = (scored_a_notes == 0) ? 0 : a_score / scored_a_notes;
    float avg_b_score = (scored_b_notes == 0) ? 0 : b_score / scored_b_notes;

    float ab_diff = (a_score / a_notes) - (b_score / b_notes);
    float indicator_range_ratio = (indicator_scale * indicator_range_pixels) / window_size.y / 2;
    float indicator_y_pos = ab_diff * indicator_range_ratio + choice_bar_y_ratio;

    glm::u8vec4 indicator_color = BASE_COLOR_SOLID;
    indicator_color.a = (uint8_t)(255.0 * alpha);

    choice_indicator->draw(norm_to_window(glm::vec2(choice_bar_x_ratio, indicator_y_pos), window_size), 0.5f, indicator_color);

    // Show scoring
    scoring_text_renderer->set_drawable_size(window_size);

    auto to_percent = [](float val) {
        // https://cplusplus.com/reference/iomanip/setprecision/

		std::stringstream stream;
        stream << std::fixed << std::setprecision(0) << val * 100;
        return stream.str() + "%";
	};

    glm::vec4 scoring_color = glm::vec4(BASE_COLOR_NORM, alpha);

    std::string scoring_text = "Non-Choice Score: " + to_percent(non_choice_score) + " | A Score: " + to_percent(avg_a_score) + " | B Score: " + to_percent(avg_b_score);
    glm::vec2 scoring_pos = norm_to_window(glm::vec2(scoring_x_ratio, scoring_y_ratio), window_size);
    scoring_text_renderer->renderText(scoring_text, scoring_pos.x, scoring_pos.y, 1.0f, scoring_color);
}

void Beatmap::draw_scoring_text(glm::uvec2 const &window_size, glm::u8vec4 hue) {
    // set alpha
    auto hue_perfect = hue;
    auto hue_great = hue;
    auto hue_good = hue;
    auto hue_miss = hue;

    hue_perfect.a = (uint8_t)(255.0 * score_text_fades[0].alpha);
    hue_great.a = (uint8_t)(255.0 * score_text_fades[1].alpha);
    hue_good.a = (uint8_t)(255.0 * score_text_fades[2].alpha);
    hue_miss.a = (uint8_t)(255.0 * score_text_fades[3].alpha);

    perfect_text->set_drawable_size(window_size);
    great_text->set_drawable_size(window_size);
    good_text->set_drawable_size(window_size);
    miss_text->set_drawable_size(window_size);

    perfect_text->draw(norm_to_window(score_text_pos_norm, window_size), 2.0f, hue_perfect);
    great_text->draw(norm_to_window(score_text_pos_norm, window_size), 2.0f, hue_great);
    good_text->draw(norm_to_window(score_text_pos_norm, window_size), 2.0f, hue_good);
    miss_text->draw(norm_to_window(score_text_pos_norm, window_size), 2.0f, hue_miss);
}

void Beatmap::draw_empty_arrow_glow(glm::uvec2 const &window_size, glm::u8vec4 hue) {

    // set alpha
    auto hue_up = hue;
    auto hue_down = hue;
    auto hue_left = hue;
    auto hue_right = hue;

    hue_up.a = (uint8_t)(255.0 * glow_fades[0].alpha);
    hue_down.a = (uint8_t)(255.0 * glow_fades[1].alpha);
    hue_left.a = (uint8_t)(255.0 * glow_fades[2].alpha);
    hue_right.a = (uint8_t)(255.0 * glow_fades[3].alpha);

    up_arrow_glow->set_drawable_size(window_size);
    down_arrow_glow->set_drawable_size(window_size);
    left_arrow_glow->set_drawable_size(window_size);
    right_arrow_glow->set_drawable_size(window_size);

    up_arrow_glow->draw(norm_to_window(up_arrow_destination_norm, window_size), arrow_size, hue_up);
    down_arrow_glow->draw(norm_to_window(down_arrow_destination_norm, window_size), arrow_size, hue_down);
    left_arrow_glow->draw(norm_to_window(left_arrow_destination_norm, window_size), arrow_size, hue_left);
    right_arrow_glow->draw(norm_to_window(right_arrow_destination_norm, window_size), arrow_size, hue_right);
}

void Beatmap::draw_arrows(glm::uvec2 const &window_size, float song_time_elapsed) {

    up_arrow->set_drawable_size(window_size);
    down_arrow->set_drawable_size(window_size);
    left_arrow->set_drawable_size(window_size);
    right_arrow->set_drawable_size(window_size);

    // render arrows from beatmap
    const float arrow_speed = 220.0f / window_size.x;

    // Start loop at curr_index, because only draw arrows we haven't scored
    for (size_t i = curr_draw_index; i < num_notes; i++) {
        float arrow_x_pos = x_pos_ratio + (timestamps[i] - song_time_elapsed) * arrow_speed;

        // early out if arrow is off screen, later arrows also off screen
        if (arrow_x_pos > 1.0f) {
            break;
        }

        if (states[i] == HIT) {
            // We've hit the note, so we disappear it
            fades[i].disappear();
            states[i] = HIT_FADE;
        }
        else if (states[i] == MISSED) {
            // fade note out since it's a miss
            fades[i].fade_out();
            states[i] = MISS_FADE;
        }
        else if (states[i] == LOW_SCORE) {
            // disappear since it's hit
            fades[i].disappear();
            states[i] = LOW_SCORE_FADE;
        }

        // Don't draw arrows that are off-screen
        if (arrow_x_pos < 0.01f) {
            if (states[i] == BASE) {
                // If arrow is on the left side of screen and hasn't been hit/missed, means we missed hitting it
                score_key(song_time_elapsed, SDLK_UNKNOWN); // score using definitely the wrong key
            }
            curr_draw_index++;
            continue;
        }

        if (states[i] == HIDE) continue; // arrow hidden

        // arrow drawing logic
        arrowState_t state = states[i];
        glm::uvec4 arrow_color;
        switch (state) {
            case HIT:
            case HIT_FADE:
                arrow_color = HIT_COLOR_SOLID;
                break;
            case MISSED:
            case MISS_FADE:
                arrow_color = MISS_COLOR_SOLID;
                break;
            case LOW_SCORE:
            case LOW_SCORE_FADE:
                arrow_color = LOW_SCORE_COLOR_SOLID;
                break;
            default:
                arrow_color = BASE_COLOR_SOLID;
                break;
        }

        glm::vec2 arrow_pos;
        arrowType_t arrow = (arrowType_t)keys[i];
        switch (arrow) {
            case UP_ARROW:
                arrow_pos = glm::vec2(arrow_x_pos, up_arrow_destination_norm.y);
                if (state == BASE) arrow_color = A_CHOICE_COLOR_SOLID;
                arrow_color.a = (uint8_t)(255.0 * fades[i].alpha);
                up_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, arrow_color);
                break;
            case DOWN_ARROW:
                arrow_pos = glm::vec2(arrow_x_pos, down_arrow_destination_norm.y);
                if (state == BASE) arrow_color = B_CHOICE_COLOR_SOLID;
                arrow_color.a = (uint8_t)(255.0 * fades[i].alpha);
                down_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, arrow_color);
                break;
            case LEFT_ARROW:
                arrow_pos = glm::vec2(arrow_x_pos, left_arrow_destination_norm.y);
                arrow_color.a = (uint8_t)(255.0 * fades[i].alpha);
                left_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, arrow_color);
                break;
            case RIGHT_ARROW:
                arrow_pos = glm::vec2(arrow_x_pos, right_arrow_destination_norm.y);
                arrow_color.a = (uint8_t)(255.0 * fades[i].alpha);
                right_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, arrow_color);
                break;
            default:
                std::cout << "Invalid key provided for beatmap." << std::endl;
                break;
        }
    }
}