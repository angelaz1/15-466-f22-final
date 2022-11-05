#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// colors for choice arrows
const glm::vec3 A_CHOICE_COLOR = glm::vec3(255, 194, 10);
const glm::vec3 B_CHOICE_COLOR = glm::vec3(12, 123, 200);

const glm::vec3 A_CHOICE_COLOR_NORM = A_CHOICE_COLOR / 255.0f;
const glm::vec3 B_CHOICE_COLOR_NORM = B_CHOICE_COLOR / 255.0f;

const glm::uvec4 A_CHOICE_COLOR_SOLID = glm::uvec4(A_CHOICE_COLOR, 255);
const glm::uvec4 B_CHOICE_COLOR_SOLID = glm::uvec4(B_CHOICE_COLOR, 255);

const glm::uvec4 MISS_COLOR_SOLID = glm::uvec4(255, 0, 0, 255);
const glm::uvec4 HIT_COLOR_SOLID = glm::uvec4(0, 255, 0, 255);
const glm::uvec4 BASE_COLOR_SOLID = glm::uvec4(255, 255, 255, 255);

// arrow position and size
const float x_pos_ratio = 0.05f;
const float arrow_size = 0.125f;

const glm::vec2 up_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.9f);
const glm::vec2 down_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.8f);
const glm::vec2 left_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.7f);
const glm::vec2 right_arrow_destination_norm = glm::vec2(x_pos_ratio, 0.6f);

// scoring guidelines
const float FULL_SCORE_THRESH = (0.05f * 0.05f);
const float NO_SCORE_THRESH = (0.25f * 0.25f);
const float SCORING_TIME_RANGE = 0.75f;

const float CHOICE_DIFF_THRESH = 0.2f; // TODO: playtest this
const float LEVEL_FAIL_THRESH = 0.6f; // level fails if score lower than this


