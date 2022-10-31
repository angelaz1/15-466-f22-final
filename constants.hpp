#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const glm::vec3 A_CHOICE_COLOR = glm::vec3(255, 194, 10);
const glm::vec3 B_CHOICE_COLOR = glm::vec3(12, 123, 200);

const glm::vec3 A_CHOICE_COLOR_NORM = A_CHOICE_COLOR / 255.0f;
const glm::vec3 B_CHOICE_COLOR_NORM = B_CHOICE_COLOR / 255.0f;

const glm::uvec4 A_CHOICE_COLOR_SOLID = glm::uvec4(A_CHOICE_COLOR, 255);
const glm::uvec4 B_CHOICE_COLOR_SOLID = glm::uvec4(B_CHOICE_COLOR, 255);
