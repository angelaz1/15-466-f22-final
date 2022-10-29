#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "data_path.hpp"

struct Beatmap {
    std::vector<uint32_t> timestamps;
    std::vector<uint8_t> keys;
    
    Beatmap(std::string fname, uint32_t num_notes);
    ~Beatmap();  

    // debug purposes
    void print_beatmap();  
};



