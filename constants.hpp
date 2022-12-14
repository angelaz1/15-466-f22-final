#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// colors for choice arrows
const glm::vec3 A_CHOICE_COLOR = glm::vec3(255, 194, 10);
const glm::vec3 B_CHOICE_COLOR = glm::vec3(12, 123, 200);
const glm::vec3 BASE_COLOR = glm::vec3(255, 255, 255);

const glm::vec3 A_CHOICE_COLOR_NORM = A_CHOICE_COLOR / 255.0f;
const glm::vec3 B_CHOICE_COLOR_NORM = B_CHOICE_COLOR / 255.0f;
const glm::vec3 BASE_COLOR_NORM = BASE_COLOR / 255.0f;

const glm::uvec4 A_CHOICE_COLOR_SOLID = glm::uvec4(A_CHOICE_COLOR, 255);
const glm::uvec4 B_CHOICE_COLOR_SOLID = glm::uvec4(B_CHOICE_COLOR, 255);

const glm::uvec4 MISS_COLOR_SOLID = glm::uvec4(255, 0, 0, 255);
const glm::uvec4 HIT_COLOR_SOLID = glm::uvec4(0, 255, 0, 255);
const glm::uvec4 LOW_SCORE_COLOR_SOLID = glm::uvec4(80, 125, 80, 255);
const glm::uvec4 BASE_COLOR_SOLID = glm::uvec4(BASE_COLOR, 255);

// hit glow parameters
const glm::uvec4 GLOW_COLOR_SOLID = glm::uvec4(255, 255, 255, 255);

const float GLOW_IN_TIME = 0.08f;
const float GLOW_OUT_TIME = 0.8f;

const float SCORE_TEXT_IN_TIME = 0.1f;
const float SCORE_TEXT_OUT_TIME = 1.0f;

// arrow position and size
const float x_pos_ratio = 0.05f;
const float arrow_size = 0.125f;

const glm::vec2 up_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.9f);
const glm::vec2 left_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.8f);
const glm::vec2 right_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.7f);
const glm::vec2 down_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.6f);

// arrow fading
const float RHYTHM_ARROW_FADE_TIME = 0.5f;

// arrow travel speed
const float ARROW_SPEED = 300.0f;

// rhythm game UI positioning
const float choice_bar_x_ratio = 0.95f;
const float choice_bar_y_ratio = 0.75f;
const float indicator_scale = 0.5f;
const float indicator_range_pixels = 500;
const glm::vec2 bar_pos_norm = glm::vec2(choice_bar_x_ratio, choice_bar_y_ratio);

const float scoring_x_ratio = 0.01f;
const float scoring_y_ratio = 0.97f;
const glm::vec2 scoring_pos_norm = glm::vec2(scoring_x_ratio, scoring_y_ratio);

const float score_text_x_ratio = 0.5f;
const float score_text_y_ratio = 0.75f;
const glm::vec2 score_text_pos_norm = glm::vec2(score_text_x_ratio, score_text_y_ratio);

const float UI_FADE_TIME = 2.0f;

// scoring guidelines
const float FULL_SCORE_THRESH = (0.05f * 0.05f);
const float NO_SCORE_THRESH = (0.2f * 0.2f);
const float SCORING_TIME_RANGE = 0.4f;

const float CHOICE_DIFF_THRESH = 0.2f; // TODO: playtest this
const float LEVEL_FAIL_THRESH = 0.6f; // level fails if score lower than this

const float PERFECT_HIT = 0.9999f;
const float GREAT_HIT = 0.9f;
const float GOOD_HIT = 0.8f;
const float FAIL_HIT = 0.5f;

// dialogue constants
const float delay_after_enter = 1.0f; // for delay when choices appear
// text animation
const float normal_time_between_letters = 0.01f;
const float slow_time_between_letters = 0.05f;
const float very_slow_time_between_letters = 0.1f;
// other animations
const float SHAKE_ANIMATION_DURATION = 0.5f;
const float BOUNCE_ANIMATION_DURATION = 0.3f;
const int NUMBER_OF_BOUNCES = 3;
const float BOUNCE_MAX_OFFSET = 60.0f;

// max number of blur passes, 40 is lowest that gives smooth animation
constexpr int MAX_BLUR_PASSES = 40;