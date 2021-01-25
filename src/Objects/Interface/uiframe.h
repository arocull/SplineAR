#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <GL/gl.h>

// UI Frame - Base class for all UI elements
class UIFrame {
    public:
        UIFrame(
            glm::vec2 pos = glm::vec2(0, 0),
            glm::vec2 size = glm::vec2(0.1, 0.1),
            glm::vec4 col = glm::vec4(1, 1, 1, 0),
            UIFrame* parentPointer = nullptr
        );
    
        glm::vec2 position;
        glm::vec2 scale;
        bool lockAspect = false; // If true, locks the dimensions of the button to lowest of X or Y screen width

        glm::vec4 color;

        UIFrame* parent; // Parent of this frame, should be drawn first

    private:
        glm::vec2 computedPosition;
        glm::vec2 computedScale;

        void computeBounds(glm::vec2 screenDimensions);
        void paint();
    
    public:
        glm::vec2 getComputedPosition();
        glm::vec2 getComputedScale();

        void draw(glm::vec2 screenDimensions);
        bool containsPosition(glm::vec2 testPosition);
};