#include "Dialogue.hpp"

Dialogue::Dialogue() {
    dialogue_box = new Sprite("images/dialogue_box.png");
    choice_index = 0;
}

Dialogue::~Dialogue() {

}

void Dialogue::set_dialogue(DialogueNode *dialogue_node, bool are_color_options) {
    dialogue = dialogue_node->text;
    std::vector<std::string> choices_text;
    for (DialogueChoice* dialogue_choice : dialogue_node->choices) {
        choices_text.push_back(dialogue_choice->choice_text);
    }
    choices = choices_text;
    color_options = are_color_options;
    character_name = dialogue_node->character;
}

void Dialogue::set_choice_selected(size_t index) {
    if (index >= choices.size()) {
        std::cout << "Invalid choice index chosen." << std::endl;
    }
    else {
        choice_index = index;
    }
}

void Dialogue::draw_dialogue_box(glm::uvec2 const &window_size, float time_elapsed) {
    // Determine number of letters to render
    int num_letters_to_render = std::min((int)dialogue.length(), (int)floor(time_elapsed / time_between_letters));

    // Positioning parameters:
    float dialogue_box_bottom_offset = 30.0f;
    float text_left_offset = 20.0f;
    float text_top_offset = 70.0f;
    float choices_bottom_offset = 20.0f;
    float character_name_top_offset = 40.0f;

    // Render the box where text goes
    dialogue_box->set_drawable_size(window_size);
    float dialogue_box_scale = 0.48f;
    
    double dialogue_box_x = window_size.x * 0.5;
    double dialogue_box_y = dialogue_box_bottom_offset + dialogue_box->size.y * dialogue_box_scale * 0.5;
    dialogue_box->draw(glm::vec2(dialogue_box_x, dialogue_box_y), dialogue_box_scale);

    // Render text
    dialogue_text_renderer->set_drawable_size(window_size);
	choices_renderer->set_drawable_size(window_size);

    // Render main dialogue text
    float dialogue_margin = (1.0f - (dialogue_box->size.x * dialogue_box_scale - text_left_offset * 2) / window_size.x) * 0.5f;
    dialogue_text_renderer->set_margin(dialogue_margin);
    float dialogue_text_y = window_size.y - (dialogue_box_bottom_offset + dialogue_box->size.y * dialogue_box_scale - text_top_offset);
    dialogue_text_renderer->renderWrappedText(dialogue.substr(0, num_letters_to_render), dialogue_text_y, dialogue_text_size, dialogue_text_color, true);

    // Render choice text
    float choices_x_pos = text_left_offset + (window_size.x - dialogue_box->size.x * dialogue_box_scale) * 0.5f;
    float choices_y_pos = dialogue_box_bottom_offset + choices_bottom_offset;
    if (color_options) {
        if (choices.size() != 4) {
            std::cout << "Invalid amount of choices: " << choices.size() << " (expected 4)"<< std::endl;
        }
        else {
            std::string choice1("> ");
            choice1.append(choices[0]);
            std::string choice2("> ");
            choice2.append(choices[1]);
            choices_renderer->renderTextAB(choice1, choice2, choices_x_pos, choices_y_pos, choices_text_size, A_CHOICE_COLOR_NORM, B_CHOICE_COLOR_NORM);
        }
    }
    else {
        std::string choices_text;
        for (unsigned int i = 0; i < choices.size(); i++) {
            if (i == choice_index) choices_text.append("> ");
            choices_text.append(choices[i]);
            choices_text.append("\n");
        }
        choices_renderer->renderText(choices_text, choices_x_pos, choices_y_pos, choices_text_size, choices_text_color);
    }

    // Render character name text
    character_name_renderer->set_drawable_size(window_size);
    float character_name_x_pos = text_left_offset + (window_size.x - dialogue_box->size.x * dialogue_box_scale) * 0.5f;
    float character_name_y_pos = dialogue_box_bottom_offset + dialogue_box->size.y * dialogue_box_scale - character_name_top_offset;
    character_name_renderer->renderText(character_name, character_name_x_pos, character_name_y_pos, character_name_text_size, character_name_text_color);
}
