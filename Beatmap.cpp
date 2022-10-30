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
    for (int i = 0; i < timestamps.size(); i++) {
        printf("time: %f, key: %u\n", timestamps[i], keys[i]);
    }
}

bool Beatmap::score_key(float key_timestamp, uint8_t key) {
    // temporary debug print
    float a = timestamps[curr_index];
    std::cout << "key " << curr_index << "/" << num_notes << " || correct key " << keys[curr_index] << " at " << a << "s; hit " << key << " at " << key_timestamp << std::endl;

    // check if correct note
    // TODO: add choices
    if (key != keys[curr_index]) {
        // wrong note, no score
        std::cout << "Incorrect key" << std::endl;
    }
    else {
        // score current note based on linear interpolation of square difference
        float curr_timestamp = timestamps[curr_index];
        float diff = sqdiff(key_timestamp, curr_timestamp);
        
        // interpolate score
        float score = 1.0f - (diff - FULL_SCORE_THRESHOLD) / (NO_SCORE_THRESHOLD - FULL_SCORE_THRESHOLD);
        // apply max and min
        score = std::max(0.0f, score);
        score = std::min(1.0f, score);
        std::cout << "Score: " << score << std::endl;
        
        // add to total score
        total_score += score;
    }

    // increment index to mark current note as hit
    curr_index++;

    // return false if end of beatmap
    if (curr_index == num_notes) {
        std::cout << "Beatmap complete!" << std::endl;
        return false;
    }

    return true;
}

uint8_t translate_key(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:
            return 0;
        case SDLK_DOWN:
            return 1;
        case SDLK_LEFT:
            return 2;
        case SDLK_RIGHT:
            return 3;
        default:
            std::cout << "Error: Invalid key pressed." << std::endl;
            return 9;
    }
}

void Beatmap::draw_arrows(glm::uvec2 const &window_size, float song_time_elapsed) {
    up_arrow->set_drawable_size(window_size);
    up_arrow_empty->set_drawable_size(window_size);

    down_arrow->set_drawable_size(window_size);
    down_arrow_empty->set_drawable_size(window_size);

    left_arrow->set_drawable_size(window_size);
    left_arrow_empty->set_drawable_size(window_size);

    right_arrow->set_drawable_size(window_size);
    right_arrow_empty->set_drawable_size(window_size);

    const float arrow_size = 0.25f;

    // render arrows that are at end of screen as target
    const float x_pos_destination = 70.0f;
    const glm::uvec2 up_arrow_destination = glm::uvec2(x_pos_destination, 4 * window_size.y / 5);
    const glm::uvec2 down_arrow_destination = glm::uvec2(x_pos_destination, 3 * window_size.y / 5);
    const glm::uvec2 left_arrow_destination = glm::uvec2(x_pos_destination, 2 * window_size.y / 5);
    const glm::uvec2 right_arrow_destination = glm::uvec2(x_pos_destination, 1 * window_size.y / 5);
    up_arrow_empty->draw(up_arrow_destination, arrow_size);
    down_arrow_empty->draw(down_arrow_destination, arrow_size);
    left_arrow_empty->draw(left_arrow_destination, arrow_size);
    right_arrow_empty->draw(right_arrow_destination, arrow_size);

    // render arrows from beatmap
    const float arrow_speed = 200.0f;
    // TODO: wrap arrows in a helper function?
    // Start loop at curr_index, because only draw arrows we haven't scored
    for (unsigned int i = curr_index; i < num_notes; i++) {
        float arrow_x_pos = x_pos_destination + (timestamps[i] - song_time_elapsed) * arrow_speed;

        // Don't draw arrows that are off-screen
        if (arrow_x_pos < 10) {
            // If arrow is on the left side of screen, means we missed hitting it
            curr_index++;
            std::cout << "key " << curr_index << "/" << num_notes << " || correct key " << keys[curr_index] << " at " << timestamps[curr_index] << "s; never hit" << std::endl;
            continue;
        }
        else if (arrow_x_pos > window_size.x) continue;

        // arrow drawing logic
        switch (keys[i])
        {
        case 0:
            up_arrow->draw(glm::uvec2(arrow_x_pos, up_arrow_destination.y), arrow_size);
            break;
        case 1:
            down_arrow->draw(glm::uvec2(arrow_x_pos, down_arrow_destination.y), arrow_size);
            break;
        case 2:
            left_arrow->draw(glm::uvec2(arrow_x_pos, left_arrow_destination.y), arrow_size);
            break;
        case 3:
            right_arrow->draw(glm::uvec2(arrow_x_pos, right_arrow_destination.y), arrow_size);
            break;
        case 5:
            up_arrow->draw(glm::uvec2(arrow_x_pos, up_arrow_destination.y), arrow_size);
            left_arrow->draw(glm::uvec2(arrow_x_pos, left_arrow_destination.y), arrow_size);
            break;
        case 6:
            down_arrow->draw(glm::uvec2(arrow_x_pos, down_arrow_destination.y), arrow_size);
            right_arrow->draw(glm::uvec2(arrow_x_pos, right_arrow_destination.y), arrow_size);
            break;
        default:
            std::cout << "Invalid key provided for beatmap." << std::endl;
            break;
        }
    }
}