#pragma once

#include "src/Util/gl_headers.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>

#include "src/config.h"
#include "src/Objects/Input/click.h"
#include "src/Objects/Interface/Information/uilabel.h"

// UI Frame - Base class for all UI elements
class UIFrame {
    public:
        UIFrame();
        ~UIFrame();

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
        int zindex; // Z-Index of UI item

        void newLabel();
        UILabel* getLabel();

    private:
        glm::vec2 position;
        glm::vec2 scale;

        glm::vec4 color;

        UIFrame* parent; // Parent of this frame, should be drawn first
        UILabel* label;

        glm::vec2 computedPosition;
        glm::vec2 computedScale;

        void computeBounds(glm::vec2 screenDimensions);
        virtual void paint();
    
    public:
        glm::vec2 getComputedPosition();
        glm::vec2 getComputedScale();

        void draw(glm::vec2 screenDimensions);
        bool containsPosition(glm::vec2 testPosition);

        void hover();
        void hoverMousedown();
        void click(struct IEClick *event);
};