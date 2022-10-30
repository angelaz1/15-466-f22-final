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
	song_time_elapsed = 0;
}

PlayMode::PlayMode() {
    current_room = room_parser.parse_room("room0.txt");

	right_arrow = new Sprite("images/right.png");
	right_arrow_empty = new Sprite("images/right_empty.png");
	up_arrow = new Sprite("images/up.png");
	up_arrow_empty = new Sprite("images/up_empty.png");
	left_arrow = new Sprite("images/left.png");
	left_arrow_empty = new Sprite("images/left_empty.png");
	down_arrow = new Sprite("images/down.png");
	down_arrow_empty = new Sprite("images/down_empty.png");

	// load beatmap
	current_beatmap = Beatmap("levels/proto/proto.beatmap", 41);

	// load audio
	start_song(proto_sample);
}

PlayMode::~PlayMode() {
}

// handle key presses
bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	// check keys pressed
	if (evt.type == SDL_KEYDOWN) {
		// register key press time
		auto key_time = std::chrono::system_clock::now();
		float key_elapsed = std::chrono::duration< float >(key_time - song_start_time).count();
		std::cout << "key time: " << key_elapsed << std::endl;



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

	song_time_elapsed += elapsed;
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
		// TODO: some way to do this calculation in the Beatmap struct? 
		// TODO: some method to not render any more arrows past/before a certain index if they're off screen?
		// TODO: wrap arrows in a helper function?
		for (unsigned int i = 0; i < current_beatmap.keys.size(); i++) {
			float arrow_x_pos = x_pos_destination + (current_beatmap.timestamps[i] - song_time_elapsed) * arrow_speed;
			switch (current_beatmap.keys[i])
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
	
	GL_ERRORS();
}