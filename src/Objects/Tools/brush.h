#pragma once

#include "src/config.h"
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include "src/Objects/Art/point.h"
#include "src/Objects/Art/stroke.h"

class Brush {
    public:
        Brush();
        ~Brush();

    private:
        int numSamples = 0; // Number of samples taken this stroke
        glm::vec2* samples;    // Sample of X and Y position

        Stroke* stroke = nullptr; // Stroke we're currently drawing to
        Point* storedPoint = nullptr; // Point we're currently drawing to, stored in case of a closed-preview

        // Brush Settings //
        float thickness = 25.0f;
        float autocloseGap = 30.0f; // Maximum distance (in pixels?) between cursor and starting point to auto-close the curve 
        float maxPointGap = 100.0f; // Maximum distance (in pixels) before a new point is created for the given line

    public:
        Stroke* startStroke(double x, double y);
        void endStroke();
        void sampleStroke(double x, double y);

        bool inStroke();
        Stroke* getActiveStroke();

        void clearSamples();
};