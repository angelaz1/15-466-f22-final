#include "RhythmParser.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"

#include <iostream>
#include <fstream>

std::vector< RhythmBeats > RhythmParser::parse_rhythm(std::string path) {
    std::vector< RhythmBeats > rhythm_table = std::vector< RhythmBeats >();

    std::filebuf fb;
    fb.open(data_path(path), std::ios::in);
    std::istream rhythm_is(&fb);
    read_chunk(rhythm_is, "rhy0", &rhythm_table);
    fb.close();

    return rhythm_table;
}

