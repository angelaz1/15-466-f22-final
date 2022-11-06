#include "Dialogue.hpp"

#include <filesystem>

Dialogue::Dialogue() {
    choice_index = 0;

    // Load sprites
    for (auto &entry : std::filesystem::recursive_directory_iterator(data_path("images/dialogue"))) {
        std::string filename = entry.path().stem().string();
        std::string pathname = entry.path().string();
        sprite_map.insert(std::pair(filename, new Sprite(pathname)));
    }

    // Set fade params
    background_fade = new Fade(2.0f, 2.0f, Fade::SUSTAIN, Fade::LINEAR);
    background_fade->alpha = 1.0f;
    text_fade = new Fade(2.0f, 2.0f, Fade::SUSTAIN, Fade::LINEAR);
    text_fade->alpha = 1.0f;
}

Dialogue::~Dialogue() {

}

void Dialogue::update_dialogue_box(float elapsed) {
    letter_time_elapsed += elapsed;

    background_fade->update(elapsed);
    text_fade->update(elapsed);
}

void Dialogue::set_dialogue(DialogueNode *dialogue_node, bool in_beatmap) {
    // Check if we need to fade background out when entering beatmap section
    if (!is_in_beatmap && in_beatmap) {
        background_fade->fade_out();
    }
    else if (is_in_beatmap && !in_beatmap) {
        background_fade->fade_in();
    }

    // Set options for dialogue
    dialogue = dialogue_node->text;
    std::vector<std::string> choices_text;
    for (DialogueChoice* dialogue_choice : dialogue_node->choices) {
        choices_text.push_back(dialogue_choice->choice_text);
    }
    choices = choices_text;
    is_in_beatmap = in_beatmap;
    character_name = dialogue_node->character;

    // Set parameters for text animation
    letter_time_elapsed = 0.0f;

    // Lookup character and emotion for sprite
    {
        std::string emotion;
        switch (dialogue_node->emotion) {
            case DialogueNode::BLUSH :
                emotion = std::string("blush");
                break;
            case DialogueNode::NEUTRAL :
                emotion = std::string("neutral");
                break;
            case DialogueNode::SAD :
                emotion = std::string("sad");
                break;
            case DialogueNode::SMILE :
                emotion = std::string("smile");
                break;
            default :
                emotion = std::string("neutral");
                break;
        }

        std::string character_name_lowercase = character_name;
        std::transform(character_name.begin(), character_name.end(), character_name_lowercase.begin(), [](unsigned char c){ return (char)std::tolower(c); });

        // perform lookup
        if (is_in_beatmap) {
            std::string lookup_string = "dialogue_box_" + character_name_lowercase + "_" + emotion;
            auto lookup_entry = sprite_map.find(lookup_string);
            if (lookup_entry == sprite_map.end()) {
                dialogue_sprite = sprite_map.find(std::string("dialogue_box"))->second;
                use_default_dialogue_box = true;
            }
            else {
                dialogue_sprite = lookup_entry->second;
                use_default_dialogue_box = false;
            }
        }
        else {
            std::string lookup_string = character_name_lowercase + "_" + emotion;
            auto lookup_entry = sprite_map.find(lookup_string);
            if (lookup_entry == sprite_map.end()) {
                character_sprite = NULL;
            }
            else {
                character_sprite = lookup_entry->second;
            }
            use_default_dialogue_box = true;
            dialogue_sprite = sprite_map.find(std::string("dialogue_box"))->second;
        }
    }

    // Lookup background in sprites
    {
        std::string background_name;
        switch (dialogue_node->background) {
            case DialogueNode::CLASSROOM :
                background_name = std::string("PLACEHOLDER_BG");
                break;
            case DialogueNode::OUTSIDE :
                background_name = std::string("PLACEHOLDER_NON_EXISTENT_BG");
                break;
            default :
                background_name = std::string("PLACEHOLDER_BG");
                break;
        }

        auto lookup_entry = sprite_map.find(background_name);
        if (lookup_entry == sprite_map.end()) {
            background_sprite = NULL;
        }
        else {
            background_sprite = lookup_entry->second;
        }
    }
}

void Dialogue::set_choice_selected(size_t index) {
    if (index >= choices.size()) {
        std::cout << "Invalid choice index chosen." << std::endl;
    }
    else {
        choice_index = index;
    }
}

void Dialogue::draw_dialogue_box(glm::uvec2 const &window_size) {
    // Render background image
    {
        if (background_sprite != NULL) {
            float background_scale = std::max((float)window_size.x / background_sprite->size.x, (float)window_size.y / background_sprite->size.y);
            float background_x = window_size.x * 0.5f;
            float background_y = window_size.y * 0.5f;
            
            unsigned int beatmap_alpha = 50;
            unsigned int non_beatmap_alpha = 255;
            unsigned int background_alpha = std::max((int)floor(background_fade->alpha * 255.0f), (int)beatmap_alpha);
            background_alpha = std::min((int)background_alpha, (int)non_beatmap_alpha);

            glm::uvec3 background_tint = glm::uvec3(252, 197, 221); // pink tint
            glm::uvec4 background_hue = glm::uvec4(background_tint, background_alpha);

            background_sprite->set_drawable_size(window_size);
            background_sprite->draw(glm::vec2(background_x, background_y), background_scale, background_hue);
        }
    }

    // Render character sprite
    {
        if (!is_in_beatmap && character_sprite != NULL) {
            float character_x = window_size.x * 0.5f + 200.0f;
            float character_y = window_size.y * 0.5f + 100.0f;
            float character_scale = 1.0f;
            glm::u8vec4 character_hue = glm::u8vec4(255, 255, 255, (int)floor(text_fade->alpha * 255));

            character_sprite->set_drawable_size(window_size);
            character_sprite->draw(glm::vec2(character_x, character_y), character_scale, character_hue);
        }
    }
    
    // Determine number of letters to render, for text animation
    int num_letters_to_render = std::min((int)dialogue.length(), (int)floor(letter_time_elapsed/ time_between_letters));

    // Positioning parameters:
    float dialogue_box_scale = 0.48f;

    float dialogue_box_bottom_offset = 30.0f;
    float text_left_offset_non_beatmap = 20.0f;
    float text_left_offset_in_beatmap = 370.0f * dialogue_box_scale;
    float text_top_offset = 70.0f;
    float choices_bottom_offset = 20.0f;
    float character_name_top_offset = 40.0f;

    // Render the box where text goes
    dialogue_sprite->set_drawable_size(window_size);

    double dialogue_box_x = window_size.x * 0.5;
    double dialogue_box_y = dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale * 0.5;
    glm::u8vec4 dialogue_box_hue = glm::u8vec4(255, 255, 255, (int)floor(text_fade->alpha * 255));
    dialogue_sprite->draw(glm::vec2(dialogue_box_x, dialogue_box_y), dialogue_box_scale, dialogue_box_hue);

    // Render text
    dialogue_text_renderer->set_drawable_size(window_size);
	choices_renderer->set_drawable_size(window_size);

    // Render main dialogue text
    if (is_in_beatmap && !use_default_dialogue_box) {
        float dialogue_margin = (1.0f - (dialogue_sprite->size.x * dialogue_box_scale - text_left_offset_in_beatmap - text_left_offset_non_beatmap * 2) / window_size.x) * 0.5f;
        dialogue_text_renderer->set_margin(dialogue_margin);
        float dialogue_text_y = window_size.y - (dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale - text_top_offset);
        dialogue_text_renderer->renderWrappedText(dialogue.substr(0, num_letters_to_render), text_left_offset_in_beatmap * 0.5f, dialogue_text_y, dialogue_text_size, glm::vec4(dialogue_text_color, text_fade->alpha), true);
    }
    else {
        float dialogue_margin = (1.0f - (dialogue_sprite->size.x * dialogue_box_scale - text_left_offset_non_beatmap * 2) / window_size.x) * 0.5f;
        dialogue_text_renderer->set_margin(dialogue_margin);
        float dialogue_text_y = window_size.y - (dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale - text_top_offset);
        dialogue_text_renderer->renderWrappedText(dialogue.substr(0, num_letters_to_render), 0.0f, dialogue_text_y, dialogue_text_size, glm::vec4(dialogue_text_color, text_fade->alpha), true);
    }
    

    // Render choice text
    float text_left_offset = is_in_beatmap && !use_default_dialogue_box ? text_left_offset_in_beatmap + text_left_offset_non_beatmap : text_left_offset_non_beatmap;
    float choices_x_pos = text_left_offset + (window_size.x - dialogue_sprite->size.x * dialogue_box_scale) * 0.5f;
    float choices_y_pos = dialogue_box_bottom_offset + choices_bottom_offset;
    if (is_in_beatmap) {
        if (choices.size() != 4) {
            std::cout << "Invalid amount of choices: " << choices.size() << " (expected 4)"<< std::endl;
        }
        else {
            std::string choice1("> ");
            choice1.append(choices[0]);
            std::string choice2("> ");
            choice2.append(choices[1]);
            choices_renderer->renderTextAB(choice1, choice2, choices_x_pos, choices_y_pos, choices_text_size, glm::vec4(A_CHOICE_COLOR_NORM, text_fade->alpha), glm::vec4(B_CHOICE_COLOR_NORM, text_fade->alpha));
        }
    }
    else {
        std::string choices_text;
        for (unsigned int i = 0; i < choices.size(); i++) {
            if (i == choice_index) choices_text.append("> ");
            choices_text.append(choices[i]);
            choices_text.append("\n");
        }
        choices_renderer->renderText(choices_text, choices_x_pos, choices_y_pos, choices_text_size, glm::vec4(choices_text_color, text_fade->alpha));
    }

    // Render character name text
    character_name_renderer->set_drawable_size(window_size);
    float character_name_x_pos = text_left_offset + (window_size.x - dialogue_sprite->size.x * dialogue_box_scale) * 0.5f;
    float character_name_y_pos = dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale - character_name_top_offset;
    character_name_renderer->renderText(character_name, character_name_x_pos, character_name_y_pos, character_name_text_size, glm::vec4(character_name_text_color, text_fade->alpha));
}

void Dialogue::fade_in_dialogue_box() {
    text_fade->fade_in();
}

void Dialogue::fade_out_dialogue_box() {
    text_fade->fade_out();
}