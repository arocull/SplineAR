#include "src/Objects/Art/stroke.h"

// Initialize new stroke object
Stroke::Stroke() {
    points = {  // We need a minimum of 2 points to create a stroke
        new Point(glm::vec2(0, 0)),
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

// Pushes the given point into the point vector
void Stroke::pushPoint(Point* newPoint) {
    points.push_back(newPoint);
}
// Creates a point at the given position, inheriting properties from the last point in the array, and pushes the point to the point vector
void Stroke::pushPoint(glm::vec2 pointPosition) {
    int len = length();
    Point* newPoint = new Point(pointPosition);
    Point* lastPoint = points[len-1];

    // Make left handle halfway between last pos and new pos (auto-smooth)
    // Could definitely be improved, brush smoothing modes?
    newPoint->leftHandle = ((lastPoint->pos + lastPoint->rightHandle) + pointPosition) / 2.0f;
    newPoint->thickness = lastPoint->thickness;
    newPoint->opacity = lastPoint->opacity;
    
    pushPoint(newPoint);
}
// Pulls the given point out of the vector array once (from back to front, will not crash if point is not present)
void Stroke::pullPoint(Point* point) {
    for (int i = length() - 1; i >= 0; i--) {
        if (points[i] == point) {
            points[i] = nullptr;
            points.erase(points.begin() + i);
            return;
        }
    }
}

// Returns the number of points in the stroke
int Stroke::length() {
    return (int) points.size();
}


// Returns a point from the given parametric
Point* Stroke::getPoint(float parametric) {
    float t = parametric * (length() - (!closed)); // Count 1 less index if stroke is not closed (as we won't be performing wrapping)
    int index = (int) t; // Automatically rounds down to nearest integer
    int indexPlusOne = index + 1;
    if (closed && indexPlusOne >= length()) { indexPlusOne = 0; } // Wrap to bottom

    printf("Parametric %f, index %i, indexPlusOne%i\n", t, index, indexPlusOne);

    return POINT_Blend(points[index], points[indexPlusOne], t);
}