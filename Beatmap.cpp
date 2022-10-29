#include "Beatmap.hpp"

Beatmap::Beatmap(std::string fname, uint32_t num_notes) {
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
        timestamps.push_back(time_ms);
        keys.push_back(key); 
    }
}

Beatmap::~Beatmap() {
    timestamps.clear();
    keys.clear();
}

void Beatmap::print_beatmap() {
    for (int i = 0; i < timestamps.size(); i++) {
        printf("time: %u, key: %u\n", timestamps[i], keys[i]);
    }
}