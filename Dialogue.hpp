#pragma once

#include <vector>
#include <string>

#include "constants.hpp"
#include "Sprite.hpp"
#include "TextRendering.hpp"
#include "DialogueManager.hpp"

struct Dialogue {
    Dialogue();
    ~Dialogue();

    // What user wants dialogue box to display
    Sprite *character_sprite;
    Sprite *dialogue_sprite;
    bool use_default_dialogue_box = true;
    
    std::string dialogue;
    std::vector<std::string> choices;
    std::string character_name;
    std::string portrait_name;
    bool is_in_beatmap;
    size_t choice_index;

    // For displaying sprites
    std::unordered_map<std::string, Sprite*> sprite_map;

    // Font renderers 
    TextRenderer *vt323_renderer = new TextRenderer(data_path("fonts/VT323-Regular.ttf"), 54);
	
	// Configuration for main text, choices and inputs
	TextRenderer *dialogue_text_renderer = vt323_renderer;
	TextRenderer *choices_renderer = vt323_renderer;
    TextRenderer *character_name_renderer = vt323_renderer;
    float dialogue_text_size = 0.5f;
    glm::vec3 dialogue_text_color = glm::vec3(0.0f, 0.0f, 0.0f);
    float choices_text_size = 0.5f;
    glm::vec3 choices_text_color = glm::vec3(0.0f, 0.0f, 0.0f);
    float character_name_text_size = 0.5f;
    glm::vec3 character_name_text_color = glm::vec3(1.0f, 1.0f, 1.0f);

    // Config for text animation
    float time_between_letters = 0.01f;
    float letter_time_elapsed = 0.0f;

    // Config for text fade-in and fade-out
    bool fading_out_started = false;
    bool fading_in_started = false;
    float fade_alpha = 1.0f;
    float total_fade_time = 2.0f;


    bool finished_text_rendering();
    void finish_text_rendering();

    void update_dialogue_box(float elapsed);

    void set_dialogue(DialogueNode *dialogue_node, bool are_color_options);
    void set_choice_selected(size_t index);
    
    void draw_dialogue_box(glm::uvec2 const &window_size);
    void fade_in_dialogue_box();
    void fade_out_dialogue_box();
};