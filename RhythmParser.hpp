#pragma once

#include <vector>
#include <string>

#define MAX_BEATS_IN_SONG 5000

struct RhythmBeats {
    std::uint32_t bpm;
    bool beats[MAX_BEATS_IN_SONG]; // Beats are indicated with true; rests are false
    uint32_t beat_count;
    uint32_t beat_index = 0; // Index of starting beat for the song, incremented at runtime
	float song_timer = 0;    // Timer used for the song
};

struct RhythmParser {
    RhythmParser() = default;

    std::vector<RhythmBeats> parse_rhythm(std::string);
};
