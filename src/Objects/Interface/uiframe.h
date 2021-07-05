#pragma once

#include "src/config.h"

#ifdef DEBUG
#include <cstdio>
#endif

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <GL/gl.h>

#include "Objects/Input/click.h"

// UI Frame - Base class for all UI elements
class UIFrame {
    public:
        UIFrame();

        void setPositionScale(glm::vec2 pos, glm::vec2 size);
        void setColor(glm::vec4 col);
        void setParent(UIFrame* parentPointer);
        glm::vec2 getPosition();
        glm::vec2 getScale();
        glm::vec4 getColor();
        UIFrame* getParent();
        bool isParentedBy(UIFrame* ancenstor);

        bool lockAspect; // If true, locks the dimensions of the button to lowest of X or Y screen width
        bool interactable; // If true,

    private:
        glm::vec2 position;
        glm::vec2 scale;

        glm::vec4 color;

        UIFrame* parent; // Parent of this frame, should be drawn first


        glm::vec2 computedPosition;
        glm::vec2 computedScale;

        void computeBounds(glm::vec2 screenDimensions);
        void paint();
    
    public:
        glm::vec2 getComputedPosition();
        glm::vec2 getComputedScale();

        void draw(glm::vec2 screenDimensions);
        bool containsPosition(glm::vec2 testPosition);

        void hover();
        void hoverMousedown();
        void click(struct IEClick *event);
};