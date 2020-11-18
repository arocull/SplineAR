#include "src/Program/input_manager.h"

InputManager::InputManager(GLFWwindow* glfwContext) {
    context = glfwContext;
}
InputManager::~InputManager() {

}

void InputManager::setBrush(Brush* newBrush) {
    brush = newBrush;
}
Stroke* InputManager::tickInput() {
    if (!brush) return nullptr; // Do not do anything without a brush

    Stroke* createdStroke = nullptr;

    int windowWidth, windowHeight;
    double mouseX, mouseY;
    glfwGetCursorPos(context, &mouseX, &mouseY);
    glfwGetWindowSize(context, &windowWidth, &windowHeight);

    mouseX /= windowWidth;
    mouseY /= windowHeight;
    mouseY = 1.0 - mouseY;

    if (glfwGetMouseButton(context, GLFW_MOUSE_BUTTON_LEFT) == GLFW_TRUE) {
        if (!mouseWasDown) { // If this is the start of a click, go ahead and create a stroke
            createdStroke = brush->startStroke(mouseX, mouseY);
            mouseWasDown = true;
            #ifdef DEBUG_VERBOSE
                printf("Starting stroke!\n");
            #endif
        } else if (brush->inStroke()) {
            brush->sampleStroke(mouseX, mouseY);
            #ifdef DEBUG_VERBOSE
                printf("Sampling stroke\n");
            #endif
        }
    } else { // Mouse button isn't down, so end stroke
        mouseWasDown = false;
        if (brush->inStroke()) {
            brush->endStroke();
            #ifdef DEBUG_VERBOSE
                printf("Stroke ended\n");
            #endif
        }
    }

    return createdStroke;
}