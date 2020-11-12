#pragma once

#include "src/config.h"
#include "src/Util/math.h"

#include <glm/vec2.hpp>

class Point {
    public:
        Point(glm::vec2 position);

        glm::vec2 pos;
        glm::vec2 leftHandle;
        glm::vec2 rightHandle;

        float thickness = 1.0f;
        float opacity = 1.0f;
};

Point* POINT_Blend(Point* a, Point* b, float alpha);