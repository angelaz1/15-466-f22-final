#include "Dialogue.hpp"

#include <filesystem>

Dialogue::Dialogue() {
    dialogue_sprite = new Sprite(data_path("images/dialogue/dialogue_box.png"));
    choice_index = 0;

    // Load sprites
    for (auto &entry : std::filesystem::recursive_directory_iterator(data_path("images/dialogue"))) {
        std::string filename = entry.path().stem().string();
        std::string pathname = entry.path().string();
        sprite_map.insert(std::pair(filename, new Sprite(pathname)));
    }
}

Dialogue::~Dialogue() {}

bool Dialogue::finished_text_rendering() {
    return letter_time_elapsed >= dialogue.length() * time_between_letters;
}

void Dialogue::finish_text_rendering() {
    letter_time_elapsed = dialogue.length() * time_between_letters;
}

void Dialogue::update_dialogue_box(float elapsed) {
    letter_time_elapsed += elapsed;

    if (fading_in_started) {
        fade_alpha += elapsed / total_fade_time;
        fade_alpha = std::min(fade_alpha, 1.0f);
    }
    else if (fading_out_started) {
        fade_alpha -= elapsed / total_fade_time;
        fade_alpha = std::max(0.0f, fade_alpha);
    }
}

void Dialogue::set_dialogue(DialogueNode *dialogue_node, bool in_beatmap) {
    // Set options for dialogue
    dialogue = dialogue_node->text;
    std::vector<std::string> choices_text;
    for (DialogueChoice* dialogue_choice : dialogue_node->choices) {
        choices_text.push_back(dialogue_choice->choice_text);
    }
    choices = choices_text;
    is_in_beatmap = in_beatmap;
    character_name = dialogue_node->character;
    portrait_name = dialogue_node->portraitName;

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
            case DialogueNode::ANGRY :
                emotion = std::string("angry");
                break;
            default :
                emotion = std::string("neutral");
                break;
        }

        std::string portrait_name_lowercase = portrait_name;
        std::transform(portrait_name.begin(), portrait_name.end(), portrait_name_lowercase.begin(), [](unsigned char c){ return (char)std::tolower(c); });

        // perform lookup
        if (is_in_beatmap) {
            std::string lookup_string = "dialogue_box_" + portrait_name_lowercase + "_" + emotion;
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
            std::string lookup_string = portrait_name_lowercase + "_" + emotion;
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
    // Render character sprite
    {
        if (!is_in_beatmap && character_sprite != NULL) {
            float character_x = window_size.x * 0.5f + 200.0f;
            float character_y = window_size.y * 0.5f + 100.0f;
            float character_scale = 1.0f;
            glm::u8vec4 character_hue = glm::u8vec4(255, 255, 255, (int)floor(fade_alpha * 255));

            character_sprite->set_drawable_size(window_size);
            character_sprite->draw(glm::vec2(character_x, character_y), character_scale, character_hue);
        }
    }
    
    // Determine number of letters to render
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
    glm::u8vec4 dialogue_box_hue = glm::u8vec4(255, 255, 255, (int)floor(fade_alpha * 255));
    dialogue_sprite->draw(glm::vec2(dialogue_box_x, dialogue_box_y), dialogue_box_scale, dialogue_box_hue);

    // Render text
    dialogue_text_renderer->set_drawable_size(window_size);
	choices_renderer->set_drawable_size(window_size);

    // Render main dialogue text
    if (is_in_beatmap && !use_default_dialogue_box) {
        float dialogue_margin = (1.0f - (dialogue_sprite->size.x * dialogue_box_scale - text_left_offset_in_beatmap - text_left_offset_non_beatmap * 2) / window_size.x) * 0.5f;
        dialogue_text_renderer->set_margin(dialogue_margin);
        float dialogue_text_y = window_size.y - (dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale - text_top_offset);
        dialogue_text_renderer->renderWrappedText(dialogue.substr(0, num_letters_to_render), text_left_offset_in_beatmap * 0.5f, dialogue_text_y, dialogue_text_size, glm::vec4(dialogue_text_color, fade_alpha), true);
    }
    else {
        float dialogue_margin = (1.0f - (dialogue_sprite->size.x * dialogue_box_scale - text_left_offset_non_beatmap * 2) / window_size.x) * 0.5f;
        dialogue_text_renderer->set_margin(dialogue_margin);
        float dialogue_text_y = window_size.y - (dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale - text_top_offset);
        dialogue_text_renderer->renderWrappedText(dialogue.substr(0, num_letters_to_render), 0.0f, dialogue_text_y, dialogue_text_size, glm::vec4(dialogue_text_color, fade_alpha), true);
    }
    
    float text_left_offset = is_in_beatmap && !use_default_dialogue_box ? text_left_offset_in_beatmap + text_left_offset_non_beatmap : text_left_offset_non_beatmap;

    // Render character name text
    character_name_renderer->set_drawable_size(window_size);
    float character_name_x_pos = text_left_offset + (window_size.x - dialogue_sprite->size.x * dialogue_box_scale) * 0.5f;
    float character_name_y_pos = dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale - character_name_top_offset;
    character_name_renderer->renderText(character_name, character_name_x_pos, character_name_y_pos, character_name_text_size, glm::vec4(character_name_text_color, fade_alpha));

    // Render choice text only if dialogue finished rendering
    if (finished_text_rendering()) {
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
                choices_renderer->renderTextAB(choice1, choice2, choices_x_pos, choices_y_pos, choices_text_size, glm::vec4(A_CHOICE_COLOR_NORM, fade_alpha), glm::vec4(B_CHOICE_COLOR_NORM, fade_alpha));
            }
        }
        else {
            std::string choices_text;
            for (unsigned int i = 0; i < choices.size(); i++) {
                if (i == choice_index) choices_text.append("> ");
                choices_text.append(choices[i]);
                choices_text.append("\n");
            }
            choices_renderer->renderText(choices_text, choices_x_pos, choices_y_pos, choices_text_size, glm::vec4(choices_text_color, fade_alpha));
        }
    }
}

void Dialogue::fade_in_dialogue_box() {
    fading_in_started = true;
    fading_out_started = false;
    fade_alpha = 0.0f;
}

void Dialogue::fade_out_dialogue_box() {
    fading_in_started = false;
    fading_out_started = true;
    fade_alpha = 1.0f;
}