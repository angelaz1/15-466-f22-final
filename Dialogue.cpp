#include "Dialogue.hpp"
#include "Framebuffers.hpp"

#include <filesystem>
#include <cmath> // for calculating bounce offset

Dialogue::Dialogue() {
    choice_index = 0;
    default_background_sprite = SpriteManager::GetInstance()->get_sprite("dialogue/background");

    // Set fade params
    background_fade = new Fade(2.0f, 2.0f, Fade::SUSTAIN, Fade::LINEAR);
    background_fade->alpha = 1.0f;
    text_fade = new Fade(2.0f, 2.0f, Fade::SUSTAIN, Fade::LINEAR);
    text_fade->alpha = 1.0f;

    // Post processing
    post_processor = NULL;
}

Dialogue::~Dialogue() {}

bool Dialogue::finished_text_rendering() {
    return letter_time_elapsed >= dialogue.length() * time_between_letters;
}

void Dialogue::finish_text_rendering() {
    letter_time_elapsed = dialogue.length() * time_between_letters;
}

void Dialogue::start_shaking_animation() {
    is_shaking = true;
    shake_time_elapsed = 0.0f;
}

void Dialogue::end_shaking_animation() {
    is_shaking = false;
}

void Dialogue::start_bouncing_animation() {
    is_bouncing = true;
    bounce_time_elapsed = 0.0f;
}

void Dialogue::end_bouncing_animation() {
    is_bouncing = false;
}

void Dialogue::update_dialogue_box(float elapsed) {
    // text animation update
    letter_time_elapsed += elapsed;

    // animation updates
    {
        // shake
        if (is_shaking && shake_time_elapsed >= SHAKE_ANIMATION_DURATION) {
            end_shaking_animation();
        }
        shake_time_elapsed += elapsed;

        // bounce
        if (is_bouncing) {
            bounce_offset = BOUNCE_MAX_OFFSET * 0.5f * sin((2.0 * M_PI / BOUNCE_ANIMATION_DURATION) * bounce_time_elapsed - (M_PI / 2.0));
            if (bounce_time_elapsed >= BOUNCE_ANIMATION_DURATION * NUMBER_OF_BOUNCES) {
                end_bouncing_animation();
            }
        }
        bounce_time_elapsed += elapsed;
    }

    // post processing update
    {
        post_processor_time_elapsed += elapsed;

        if (post_processor != NULL) {
            post_processor->Shake = is_shaking;
        }
    }

    // fading updates
    background_fade->update(elapsed);
    text_fade->update(elapsed);
}

void Dialogue::set_dialogue_emotion(DialogueNode::Emotion dialogue_emotion) {
    std::string emotion;
    switch (dialogue_emotion) {
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
        std::string lookup_string = "dialogue/dialogue_box_" + portrait_name_lowercase + "_" + emotion;

        use_default_dialogue_box = false;
        dialogue_sprite = SpriteManager::GetInstance()->get_sprite(lookup_string);

        if (dialogue_sprite == nullptr) {
            dialogue_sprite = SpriteManager::GetInstance()->get_sprite(default_dialogue_box);
            use_default_dialogue_box = true;
        }
    }
    else {
        std::string lookup_string = "dialogue/" + portrait_name_lowercase + "_" + emotion;

        character_sprite = SpriteManager::GetInstance()->get_sprite(lookup_string);
        dialogue_sprite = SpriteManager::GetInstance()->get_sprite(default_dialogue_box);
        use_default_dialogue_box = true;
    }
}

void Dialogue::set_dialogue(DialogueNode *dialogue_node, bool in_beatmap) {
    // Check animation
    {
        end_shaking_animation();
        end_bouncing_animation();
        if (dialogue_node->animation == DialogueNode::SHAKE) {
            start_shaking_animation();
        }
        else if (dialogue_node->animation == DialogueNode::BOUNCE) {
            start_bouncing_animation();
        }
    }

    // Check color
    {
        if (SpriteManager::GetInstance()->get_sprite(dialogue_node->dialogueColor) != nullptr) {
            default_dialogue_box = dialogue_node->dialogueColor;
            background_tint = dialogue_node->backgroundTint;
        }
    }

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
    portrait_name = dialogue_node->portraitName;

    // Set parameters for text animation
    letter_time_elapsed = 0.0f;
    switch(dialogue_node->textSpeed) {
        case DialogueNode::NORMAL :
            time_between_letters = normal_time_between_letters;
            break;
        case DialogueNode::SLOW :
            time_between_letters = slow_time_between_letters;
            break;
        case DialogueNode::VERY_SLOW :
            time_between_letters = very_slow_time_between_letters;
            break;
        default :
            time_between_letters = normal_time_between_letters;
            break;
    };

    // Lookup character and emotion for sprite
    set_dialogue_emotion(dialogue_node->emotion);

    // Lookup background in sprites
    {
        std::string background_name;
        switch (dialogue_node->background) {
            case DialogueNode::CONCERT_HALL :
                background_name = std::string("Alumni_Concert_Hall_CMU");
                break;
            case DialogueNode::CLASSROOM :
                background_name = std::string("classroom");
                break;
            case DialogueNode::COFFEE_SHOP :
                background_name = std::string("coffee_shop");
                break;
            case DialogueNode::HALLWAY :
                background_name = std::string("hallway");
                break;
            case DialogueNode::OUTSIDE :
                background_name = std::string("outside");
                break;
            case DialogueNode::BUS :
                background_name = std::string("pittsburgh_bus");
                break;
            case DialogueNode::BALLROOM :
                background_name = std::string("ballroom");
                break;
            case DialogueNode::BEACH :
                background_name = std::string("beach");
                break;
            case DialogueNode::FOREST :
                background_name = std::string("forest");
                break;
            case DialogueNode::HOUSE :
                background_name = std::string("house");
                break;
            case DialogueNode::LIBRARY :
                background_name = std::string("library");
                break;
            case DialogueNode::NONE :
                background_sprite = nullptr;
                return;
            case DialogueNode::KEEP :
            default :
                return; // DO NOTHING
        }

        background_sprite = SpriteManager::GetInstance()->get_sprite("dialogue/" + background_name);
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

    // set up post processor for shake effects
    {
        if (post_processor == NULL) {
            post_processor = new PostProcessor(window_size.x, window_size.y);
        } 
        post_processor->EmptyFB();
    }

    // setup blur framebuffer
    {
        framebuffers.realloc(window_size);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.hdr_fb);
    }

    // Render background image into blur framebuffer
    int blur_passes = 0;
    constexpr int MAX_BLUR_PASSES = 60;
    {
        {
            float background_scale = std::max((float)window_size.x / default_background_sprite->size.x, (float)window_size.y / default_background_sprite->size.y);
            float background_x = window_size.x * 0.5f;
            float background_y = window_size.y * 0.5f;
            default_background_sprite->set_drawable_size(window_size);
            default_background_sprite->draw(glm::vec2(background_x, background_y), background_scale, glm::uvec4(background_tint, 255));
        }
        if (background_sprite != nullptr) {
            float background_scale = std::max((float)window_size.x / background_sprite->size.x, (float)window_size.y / background_sprite->size.y);
            float background_x = window_size.x * 0.5f;
            float background_y = window_size.y * 0.5f;
            
            unsigned int beatmap_alpha = 50;
            unsigned int non_beatmap_alpha = 255;
            unsigned int background_alpha = std::max((int)floor(background_fade->alpha * 255.0f), (int)beatmap_alpha);

            // NEW interpolate background alpha to number of passes
            blur_passes = (int)floor((float)MAX_BLUR_PASSES * (1 - background_fade->alpha));

            background_alpha = std::min((int)background_alpha, (int)non_beatmap_alpha);

            glm::uvec4 background_hue = glm::uvec4(background_tint, background_alpha);

            background_sprite->set_drawable_size(window_size);
            background_sprite->draw(glm::vec2(background_x, background_y), background_scale, background_hue);
        }
    }
    
    {   // process blur framebuffer using variable number of iterations before transfering to post processing fb
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        framebuffers.add_bloom(blur_passes, post_processor->MSFBO);
    }

    // switch render to post processing fb
    post_processor->BeginRender();

    // Render character sprite
    {
        if (!is_in_beatmap && character_sprite != NULL) {
            float character_x = window_size.x * 0.5f + 200.0f;
            float character_y = window_size.y * 0.5f + 100.0f + (float)bounce_offset;
            float character_scale = 1.0f;
            glm::u8vec4 character_hue = glm::u8vec4(255, 255, 255, (int)floor(text_fade->alpha * 255));

            character_sprite->set_drawable_size(window_size);
            character_sprite->draw(glm::vec2(character_x, character_y), character_scale, character_hue);
        }
    }
    
    // Determine number of letters to render, for text animation
    int num_letters_to_render = std::min((int)dialogue.length(), (int)floor(letter_time_elapsed/time_between_letters));

    int blip_count = std::min((int)dialogue.length(), (int)floor(letter_time_elapsed/time_between_blips));
    if (num_letters_to_render != (int)dialogue.length() && blip_count != prev_blip_count) {
        if (character_name.length() > 0) {
            SFXManager::GetInstance()->play_one_shot("midblip", 0.1f);
        } else {
            SFXManager::GetInstance()->play_one_shot("lowblip", 0.1f);
        }

        prev_blip_count = blip_count;
    }

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
    
    float text_left_offset = is_in_beatmap && !use_default_dialogue_box ? text_left_offset_in_beatmap + text_left_offset_non_beatmap : text_left_offset_non_beatmap;

    // Render character name text
    character_name_renderer->set_drawable_size(window_size);
    float character_name_x_pos = text_left_offset + (window_size.x - dialogue_sprite->size.x * dialogue_box_scale) * 0.5f;
    float character_name_y_pos = dialogue_box_bottom_offset + dialogue_sprite->size.y * dialogue_box_scale - character_name_top_offset;
    character_name_renderer->renderText(character_name, character_name_x_pos, character_name_y_pos, character_name_text_size, glm::vec4(character_name_text_color, text_fade->alpha));

    // Render choices
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
    }
    
    // End post processing
    {
        post_processor->EndRender();
        post_processor->Render(post_processor_time_elapsed);
    }
}

void Dialogue::fade_in_dialogue_box() {
    text_fade->fade_in();
}

void Dialogue::fade_out_dialogue_box() {
    text_fade->fade_out();
}