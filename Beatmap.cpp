#include "Beatmap.hpp"

Beatmap::Beatmap() {
    num_notes = keys.size();
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