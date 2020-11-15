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

        // Brush Settings //
        float thickness = 25.0f;
        float autocloseGap = 0.075f;
        float maxPointGap = 0.05f;

    public:
        Stroke* startStroke(double x, double y);
        void endStroke();
        void sampleStroke(double x, double y);

        bool inStroke();

        void clearSamples();
};