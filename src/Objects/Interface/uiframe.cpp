#include "src/Objects/Interface/uiframe.h"

UIFrame::UIFrame() {
    glm::vec2 position = glm::vec2(0.05, 0.05);
    glm::vec2 scale = glm::vec2(0.1, 0.1);
    glm::vec2 computedPosition = glm::vec2(0, 0);
    glm::vec2 computedScale = glm::vec2(0, 0);

    color = glm::vec4(1, 1, 1, 1);
    parent = nullptr;
    lockAspect = false;
}

// GETTERS / SETTERS //
void UIFrame::setPositionScale(glm::vec2 pos, glm::vec2 size) {
    position = pos;
    scale = size;
}
void UIFrame::setColor(glm::vec4 col) {
    color = col;
}
void UIFrame::setParent(UIFrame* parentPointer) {
    parent = parentPointer;
}
glm::vec2 UIFrame::getPosition() { return position; }
glm::vec2 UIFrame::getScale() { return scale; }
glm::vec4 UIFrame::getColor() { return color; }
UIFrame* UIFrame::getParent() { return parent; }


glm::vec2 UIFrame::getComputedPosition() {
    return computedPosition;
}
glm::vec2 UIFrame::getComputedScale() {
    return computedScale;
}


// Draws this frame based off of screen or parent width + height
void UIFrame::draw(glm::vec2 drawScale) {
    computeBounds(drawScale);
    paint();
}
// Returns true if this frame contains the given position within its draw bounds
bool UIFrame::containsPosition(glm::vec2 testPosition) {
    return (
        testPosition.x > computedPosition.x &&
        testPosition.x < computedPosition.x + computedScale.x &&
        testPosition.y > computedPosition.y &&
        testPosition.y < computedPosition.y + computedScale.y
    );
}


// Calculates computedPosition (top left corner) and computedScale (width of frame)
void UIFrame::computeBounds(glm::vec2 drawScale) {
    glm::vec2 offset = glm::vec2(0, 0);
    if (parent) {
        drawScale = parent->getComputedScale();
        offset = parent->getComputedPosition();
    }

    if (lockAspect) { // To preserve a fixed ratio regardless of screen/ui size, lock to minimum scale
        if (drawScale.x < drawScale.y)
            drawScale.y = drawScale.x;
        else
            drawScale.x = drawScale.y;
    }

    // Find top left corner
    computedPosition = offset + drawScale * position; // Add parent frame offset, then add current position (scaled to parent frame)
    
    // Get width + height of frame
    computedScale = drawScale * scale; // Computed scale will be smaller than the drawScale (must factor in own scale)
}
// Performs actual OpenGL drawing of this frame
void UIFrame::paint() {
    glm::vec2 corner = computedPosition + computedScale; // Get bottom right corner

    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_QUADS);
    
    glVertex2f(computedPosition.x, computedPosition.y);
    glVertex2f(corner.x, computedPosition.y);
    glVertex2f(corner.x, corner.y);
    glVertex2f(computedPosition.x, corner.y);

    glEnd();
}