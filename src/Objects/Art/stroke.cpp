#include "src/Objects/Art/stroke.h"

// Initialize new stroke object
Stroke::Stroke() {
    points = {
        new Point(glm::vec2(0, 0))
    };
}
// Deallocate stroke object--deallocates contained points and clears the vector
Stroke::~Stroke() {
    for (int i = 0; i < length(); i++) {
        if (points[i]) delete points[i];
    }

    points.clear();
}

void Stroke::pushPoint(glm::vec2 pointPosition) {
    int len = length();
    Point* newPoint = new Point(pointPosition);
    Point* lastPoint = points[len-1];

    // Make left handle halfway between last pos and new pos (auto-smooth)
    // Could definitely be improved, brush smoothing modes?
    newPoint->leftHandle = ((lastPoint->pos + lastPoint->rightHandle) + pointPosition) / 2.0f;
    newPoint->thickness = lastPoint->thickness;
    newPoint->opacity = lastPoint->opacity;
    
    points.push_back(newPoint);
}

int Stroke::length() {
    return (int) points.size();
}


Point* STROKE_GetPoint(Stroke* stroke, float parametric) {
    float t = parametric * (stroke->length() - (!stroke->closed)); // Count 1 less index if stroke is not closed (as we won't be performing wrapping)
    int index = (int) t; // Automatically rounds down to nearest integer
    int indexPlusOne = index + 1;
    if (stroke->closed && indexPlusOne >= stroke->length()) { indexPlusOne = 0; } // Wrap to bottom

    printf("Parametric %f, index %i, indexPlusOne%i\n", t, index, indexPlusOne);

    return POINT_Blend(stroke->points[index], stroke->points[indexPlusOne], t);
}