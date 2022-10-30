#include "Dialogue.hpp"

Dialogue::Dialogue() {
    dialogue_box = new Sprite("images/dialogue_box.png");
}

Dialogue::~Dialogue() {

}

void Dialogue::set_dialogue(std::string main_text, std::vector<std::string> choices_text, bool are_color_options) {
    this->dialogue = main_text;
    this->choices = choices_text;
    this->color_options = are_color_options;
}

void Dialogue::draw_dialogue_box(glm::uvec2 const &window_size) {
    // Positioning parameters:
    float dialogue_box_bottom_offset = 30.0f;
    float text_left_offset = 20.0f;
    float text_top_offset = 100.0f;
    float choices_bottom_offset = 20.0f;

    // Render the box where text goes
    dialogue_box->set_drawable_size(window_size);
    float dialogue_box_scale = 0.49f;
    
    double dialogue_box_x = window_size.x * 0.5;
    double dialogue_box_y = dialogue_box_bottom_offset + dialogue_box->size.y * dialogue_box_scale * 0.5;
    dialogue_box->draw(glm::vec2(dialogue_box_x, dialogue_box_y), dialogue_box_scale);

    // Render text
    dialogue_text_renderer->set_drawable_size(window_size);
	choices_renderer->set_drawable_size(window_size);

    // Render main dialogue text
    float dialogue_margin = (1.0f - (dialogue_box->size.x * dialogue_box_scale - text_left_offset * 2) / window_size.x) * 0.5f;
    dialogue_text_renderer->set_margin(dialogue_margin);
    float dialogue_text_y = dialogue_box_bottom_offset + dialogue_box->size.y * dialogue_box_scale - text_top_offset;
    dialogue_text_renderer->renderWrappedText(dialogue, dialogue_text_y, dialogue_text_size, dialogue_text_color);

    // Render choice text
    float choices_x_pos = text_left_offset + (window_size.x - dialogue_box->size.x * dialogue_box_scale) * 0.5f;
    float choices_y_pos = dialogue_box_bottom_offset + choices_bottom_offset;
    if (color_options) {
        if (choices.size() != 2) {
            std::cout << "Invalid amount of choices: " << choices.size() << std::endl;
        }
        else {
            std::string choice1("> ");
            choice1.append(choices[0]);
            std::string choice2("> ");
            choice2.append(choices[1]);
            choices_renderer->renderTextAB(choice1, choice2, choices_x_pos, choices_y_pos, choices_text_size, choices_text_color1, choices_text_color2);
        }
    }
    else {
        std::string choices_text;
        for (unsigned int i = 0; i < choices.size(); i++) {
            choices_text.append("> ");
            choices_text.append(choices[i]);
            choices_text.append("\n");
        }
        choices_renderer->renderText(choices_text, choices_x_pos, 0.0f, choices_text_size, choices_text_color);
    }
}