#pragma once

#include <vector>
#include <string>

#include "constants.hpp"

#include "Sprite.hpp"
#include "TextRendering.hpp"
#include "DialogueManager.hpp"
#include "Beatmap.hpp"
#include "SFXManager.hpp"
#include "SpriteManager.hpp"
#include "PostProcessor.hpp"

struct Dialogue {
    Dialogue();
    ~Dialogue();

    // What user wants dialogue box to display
    std::string dialogue;
    std::vector<std::string> choices;
    std::string character_name;
    std::string portrait_name;
    bool is_in_beatmap;
    size_t choice_index;

    // For displaying sprites
    std::string default_dialogue_box = "dialogue/dialogue_box";

    Sprite *character_sprite;
    Sprite *dialogue_sprite;
    bool use_default_dialogue_box = true;
    Sprite *background_sprite;
    Sprite *default_background_sprite;

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

    // For text animation
    float time_between_letters;
    float letter_time_elapsed = 0.0f;
    float time_between_blips = 0.1f;
    int prev_blip_count = 0;

    // Config for text fade-in and fade-out
    Fade *text_fade;
    Fade *background_fade;

    // Post processing
    PostProcessor *post_processor;
    float post_processor_time_elapsed = 0.0f;

    // Animation states
    bool is_shaking = false;
	float shake_time_elapsed = 0.0f;

    bool is_bouncing = false;
    float bounce_time_elapsed = 0.0f;
    double bounce_offset = 0.0f;

    glm::uvec3 background_tint = glm::uvec3(230, 230, 230);

    bool finished_text_rendering();
    void finish_text_rendering();

    void update_dialogue_box(float elapsed);

    void set_dialogue(DialogueNode *dialogue_node, bool are_color_options);
    void set_dialogue_emotion(DialogueNode::Emotion dialogue_emotion);
    void set_choice_selected(size_t index);
    
    void draw_dialogue_box(glm::uvec2 const &window_size);
    void fade_in_dialogue_box();
    void fade_out_dialogue_box();

private:
    void start_shaking_animation();
    void end_shaking_animation();
    void start_bouncing_animation();
    void end_bouncing_animation();
};