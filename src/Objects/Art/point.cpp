#include "src/Objects/Art/point.h"

Point::Point(glm::vec2 position) {
    pos = position;
    leftHandle = glm::vec2(0, 0);
    rightHandle = glm::vec2(0, 0);
}


// Blends two points using the given alpha
Point* POINT_Blend(Point* a, Point* b, float alpha) {
    Point* ret = new Point(lerp(a->pos, b->pos, alpha));

    // TODO: Consider that right handle of point A corresponds to left handle of point B, and so on
    // Currently we blend left + left, and right + right as this point is supposed to just be an in-between
    ret->leftHandle = lerp(a->leftHandle, b->leftHandle, alpha);
    ret->rightHandle = lerp(a->rightHandle, b->rightHandle, alpha);

    ret->thickness = lerp(a->thickness, b->thickness, alpha);
    ret->opacity = lerp(a->thickness, b->thickness, alpha);

    return ret;
}