#include "Beatmap.hpp"

Beatmap::Beatmap() {
    num_notes = keys.size();

    // load arrow sprites
    right_arrow = new Sprite("images/right.png");
	right_arrow_empty = new Sprite("images/right_empty.png");
	up_arrow = new Sprite("images/up.png");
	up_arrow_empty = new Sprite("images/up_empty.png");
	left_arrow = new Sprite("images/left.png");
	left_arrow_empty = new Sprite("images/left_empty.png");
	down_arrow = new Sprite("images/down.png");
	down_arrow_empty = new Sprite("images/down_empty.png");
}

Beatmap::Beatmap(std::string fname, uint32_t num_notes) {
    this->num_notes = num_notes;
    // open file
    fname = data_path(fname);
    std::ifstream file(fname, std::ios::in | std::ios::binary);

    // error out if file doesn't exist
    if (!file.is_open()) {
        std::cout << "Error: File " << fname << " does not exist." << std::endl;
        throw std::runtime_error("Exiting with error.");
        return;
    }
    for (uint32_t i = 0; i < num_notes; i++) {

        // read int for time and char for key
        uint32_t time_ms;
        uint8_t key;

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
    }

    // load arrow sprites
    right_arrow = new Sprite("images/right.png");
	right_arrow_empty = new Sprite("images/right_empty.png");
	up_arrow = new Sprite("images/up.png");
	up_arrow_empty = new Sprite("images/up_empty.png");
	left_arrow = new Sprite("images/left.png");
	left_arrow_empty = new Sprite("images/left_empty.png");
	down_arrow = new Sprite("images/down.png");
	down_arrow_empty = new Sprite("images/down_empty.png");
}

Beatmap::~Beatmap() {
    timestamps.clear();
    keys.clear();
}

void Beatmap::print_beatmap() {
    for (size_t i = 0; i < timestamps.size(); i++) {
        printf("time: %f, key: %u\n", timestamps[i], keys[i]);
    }
}

void Beatmap::start() {
    curr_index = 0;
    total_score = 0;
    a_score = 0;
    b_score = 0;
    in_progress = true;
    started = true;
}

uint8_t Beatmap::translate_key(SDL_Keycode key) {

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

bool Beatmap::score_key(float key_timestamp, SDL_Keycode sdl_key) {
    // translate key
    uint8_t key = translate_key(sdl_key);

    float curr_timestamp = timestamps[curr_index];
    // temporary debug print
    std::cout << "key " << curr_index << "/" << num_notes << " || correct key " << std::to_string(keys[curr_index]) << " at " << curr_timestamp << "s; hit " << std::to_string(key) << " at " << key_timestamp << std::endl;

    // check if in bounds of the current note
    if (abs(curr_timestamp - key_timestamp) >= SCORE_BOUND) {
        // early out
        std::cout << "Out of scoring bounds, skipping." << std::endl;
        return true;
    }

    // set total score buffer according to choice
    float *score_buffer;

    // add up arrow to A score, down arrow to B score
    if (keys[curr_index] == UP_ARROW) {
        score_buffer = &a_score;
    } else if (keys[curr_index] == DOWN_ARROW) {
        score_buffer = &b_score;
    } else {
        score_buffer = &total_score;
    }

    // check if correct note
    if (key != keys[curr_index]) {
        // wrong note, no score
        std::cout << "Incorrect key" << std::endl;
    }
    else {
        // score current note based on linear interpolation of square difference
        float diff = sqdiff(key_timestamp, curr_timestamp);
        
        // interpolate score
        float score = 1.0f - (diff - FULL_SCORE_THRESHOLD) / (NO_SCORE_THRESHOLD - FULL_SCORE_THRESHOLD);
        // apply max and min
        score = std::max(0.0f, score);
        score = std::min(1.0f, score);
        std::cout << "Score: " << score << std::endl;
        
        // add to total score
        *score_buffer += score;
    }

    // increment index to mark current note as hit
    curr_index++;

    // return false if end of beatmap
    if (curr_index == num_notes) {
        std::cout << "Beatmap complete!" << std::endl;
        finished = true;
        return false;
    }

    return true;
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

void Beatmap::draw_arrows(glm::uvec2 const &window_size, float song_time_elapsed) {

    up_arrow->set_drawable_size(window_size);
    down_arrow->set_drawable_size(window_size);
    left_arrow->set_drawable_size(window_size);
    right_arrow->set_drawable_size(window_size);

    // render arrows from beatmap
    const float arrow_speed = 200.0f / window_size.x;

    // Start loop at curr_index, because only draw arrows we haven't scored
    for (size_t i = curr_index; i < num_notes; i++) {
        float arrow_x_pos = x_pos_ratio + (timestamps[i] - song_time_elapsed) * arrow_speed;

        // Don't draw arrows that are off-screen
        if (arrow_x_pos < 0.01f) {
            // If arrow is on the left side of screen, means we missed hitting it
            score_key(song_time_elapsed, SDLK_UNKNOWN); // score using definitely the wrong key
            std::cout << "key " << curr_index << "/" << num_notes << " || correct key " << keys[curr_index] << " at " << timestamps[curr_index] << "s; never hit" << std::endl;
            continue;
        }
        else if (arrow_x_pos > 1.0f) break; // All later notes are also off screen; stop looping

        // arrow drawing logic
        glm::vec2 arrow_pos;
        switch (keys[i]) {
            case 0:
                arrow_pos = glm::vec2(arrow_x_pos, up_arrow_destination_norm.y);
                up_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, A_CHOICE_COLOR_SOLID);
                break;
            case 1:
                arrow_pos = glm::vec2(arrow_x_pos, down_arrow_destination_norm.y);
                down_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, B_CHOICE_COLOR_SOLID);
                break;
            case 2:
                arrow_pos = glm::vec2(arrow_x_pos, left_arrow_destination_norm.y);
                left_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size);
                break;
            case 3:
                arrow_pos = glm::vec2(arrow_x_pos, right_arrow_destination_norm.y);
                right_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size);
                break;
            // case 5:
            //     arrow_pos = glm::vec2(arrow_x_pos, up_arrow_destination_norm.y);
            //     up_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, A_CHOICE_COLOR_SOLID);
            //     arrow_pos = glm::vec2(arrow_x_pos, left_arrow_destination_norm.y);
            //     left_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, B_CHOICE_COLOR_SOLID);
            //     break;
            // case 6:
            //     arrow_pos = glm::vec2(arrow_x_pos, down_arrow_destination_norm.y);
            //     down_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, A_CHOICE_COLOR_SOLID);
            //     arrow_pos = glm::vec2(arrow_x_pos, right_arrow_destination_norm.y);
            //     right_arrow->draw(norm_to_window(arrow_pos, window_size), arrow_size, B_CHOICE_COLOR_SOLID);
            //     break;
            default:
                std::cout << "Invalid key provided for beatmap." << std::endl;
                break;
        }
    }
}