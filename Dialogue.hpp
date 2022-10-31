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
    Sprite *dialogue_box;
    std::string dialogue;
    std::vector<std::string> choices;
    std::string character_name;
    bool color_options;

    // Font renderers 
	TextRenderer *rubik_renderer = new TextRenderer(data_path("fonts/RubikDirt-Regular.ttf"), 72);
	TextRenderer *patua_renderer = new TextRenderer(data_path("fonts/PatuaOne-Regular.ttf"), 54);
	
	// Configuration for main text, choices and inputs
	TextRenderer *dialogue_text_renderer = patua_renderer;
	TextRenderer *choices_renderer = patua_renderer;
    TextRenderer *character_name_renderer = patua_renderer;
    float dialogue_text_size = 0.5f;
    glm::vec3 dialogue_text_color = glm::vec3(0.0f, 0.0f, 0.0f);
    float choices_text_size = 0.5f;
    glm::vec3 choices_text_color = glm::vec3(0.0f, 0.0f, 0.0f);
    float character_name_text_size = 0.5f;
    glm::vec3 character_name_text_color = glm::vec3(1.0f, 1.0f, 1.0f);


    void set_dialogue(DialogueNode *dialogue_node, bool are_color_options);
    void draw_dialogue_box(glm::uvec2 const &window_size);
};