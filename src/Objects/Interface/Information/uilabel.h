#pragma once

#include <string>
#include <glm/vec3.hpp>

struct UILabel {
    int font;
    glm::vec3 color;
    std::string text;
};
typedef struct UILabel UIlabel;