#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "data_path.hpp"

struct Beatmap {
    std::vector<float> timestamps;
    std::vector<uint8_t> keys;
    
    Beatmap();
    Beatmap(std::string fname, uint32_t num_notes);
    ~Beatmap();  

    float score(float timestamp, uint8_t key);

    // debug purposes
    void print_beatmap();  
};
