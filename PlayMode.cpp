#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "RoomParser.hpp"
#include "Beatmap.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <fstream>

#include <ctype.h>

#define FONT "Roboto-Medium.ttf"

Load< Sound::Sample > proto_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("levels/proto/proto.wav"));
});

void PlayMode::start_song(Load<Sound::Sample> sample) {
	Sound::play(*sample, 1.0f, 0.0f);
	song_start_time = std::chrono::system_clock::now();
}

PlayMode::PlayMode() {
    current_room = room_parser.parse_room("room0.txt");

	// load beatmap
	current_beatmap = Beatmap("levels/proto/proto.beatmap", 41);

	// load audio
	start_song(proto_sample);
}

PlayMode::~PlayMode() {
}

// handle key presses
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	static bool key_down = false;
	// check keys pressed
	if (evt.type == SDL_KEYDOWN) {
		
		// only once per key down
		if (!key_down) {
			// register key press time
			auto key_time = std::chrono::system_clock::now();
			float key_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();

			// check key against beatmap
			uint8_t pressed_key = translate_key(evt.key.keysym.sym);
			bool keys_left = current_beatmap.score_key(key_elapsed, pressed_key);
			if (!keys_left) {
				std::cout << "no more keys left; score = " << current_beatmap.avg_score() << std::endl;
			}
			// set key_down to true to prevent double counting
			key_down = true;
		}

		return true;
	}
	else if (evt.type == SDL_KEYUP) {
		// reset key down
		key_down = false;
		return true;
	}

	return false;
}


void PlayMode::update(float elapsed) {
	if(time_elapsed > 1) {
        time_elapsed = 1;
    } else {
        time_elapsed += elapsed;
    }
}

void PlayMode::draw(glm::uvec2 const &drawable_size, glm::uvec2 const &window_size) {

	glClearColor(bg_color.r,bg_color.g,bg_color.b,1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{ // render arrows
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST); 
		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		auto key_time = std::chrono::system_clock::now();
		float song_time_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();
		current_beatmap.draw_arrows(window_size, song_time_elapsed);
	}
	
	GL_ERRORS();
}