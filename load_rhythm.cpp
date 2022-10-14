#include <glm/glm.hpp>

#include "data_path.hpp"
#include "read_write_chunk.hpp"
#include "PlayMode.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "RhythmParser.hpp"

bool read_rhythm(std::string file_path, RhythmBeats *rhythm) {
    std::fstream txtfile;
    txtfile.open(file_path, std::ios::in); 
    if (txtfile.is_open()) {
        if (txtfile.eof()) {
            // Unexpected end of file
            return false;
        }

        std::string line;
        getline(txtfile, line);
        rhythm->bpm = std::stoi(line);

        uint32_t index = 0;
        while (getline(txtfile, line)) {
            for (auto &ch : line) {
                if (ch == 'x') {
                    rhythm->beats[index] = true;
                } else {
                    rhythm->beats[index] = false;
                }
                index++;
            }
        }
        rhythm->beat_count = index;

        txtfile.close();
    }
    return true;
}

int main(int argc, char **argv) {
    // Using same wrapper as starter code in main.cpp for Windows error handling
#ifdef _WIN32
	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif
    std::vector< RhythmBeats > rhythm_table = std::vector< RhythmBeats >();
    RhythmBeats rhythm;

    // Referenced sample code in documentation https://en.cppreference.com/w/cpp/filesystem/directory_iterator
#ifdef __MACOSX__
    for (auto const& dir_entry : std::__fs::filesystem::directory_iterator{ data_path("") }) {
        if (dir_entry.path().extension() == ".txt") {
            if (!read_rhythm(dir_entry.path(), &rhythm)) {
                return 1;
            }
            rhythm_table.push_back(rhythm);
        }
    }
#else
    for (auto const& dir_entry : std::filesystem::directory_iterator{ data_path("") }) {
        if (dir_entry.path().extension() == ".txt") {
            if (!read_rhythm(dir_entry.path(), &rhythm)) {
                return 1;
            }
            rhythm_table.push_back(rhythm);
        }
    }
#endif

    std::filebuf fb;
    fb.open(data_path("../dist/rhythm.chunk"), std::ios::out);
    std::ostream rhythm_os(&fb);
    write_chunk("rhy0", rhythm_table, &rhythm_os);
    fb.close();

	return 0;

#ifdef _WIN32
	} catch (std::exception const &e) {
		std::cerr << "Unhandled exception:\n" << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unhandled exception (unknown type)." << std::endl;
		throw;
	}
#endif
}