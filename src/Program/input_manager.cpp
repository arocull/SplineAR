#include "src/Program/input_manager.h"

InputManager::InputManager(GLFWwindow* glfwContext) {
    context = glfwContext;

    keystrokes = (Keystroke**) malloc(KEYSTROKE_BUFFER_SIZE * sizeof(Keystroke*));
    for (int i = 0; i < KEYSTROKE_BUFFER_SIZE; i++) {
        keystrokes[i] = nullptr;
    }

    glfwSetKeyCallback(context, callbackKeystroke);
}
InputManager::~InputManager() {
    for (int i = 0; i < KEYSTROKE_BUFFER_SIZE; i++) {
        handleKeystroke(i);
    }
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

    //mouseX /= windowWidth;
    //mouseY /= windowHeight;
    //mouseY = 1.0 - mouseY;
    mouseY = windowHeight - mouseY; // Invert mouse height

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

void InputManager::forceEndStroke() {
    if (brush) brush->endStroke();
}

void InputManager::appendKeystroke(Keystroke* keystroke) {
    for (int i = 0; i < KEYSTROKE_BUFFER_SIZE; i++) {
        if (keystrokes[i] == nullptr) {
            keystrokes[i] = keystroke;
            return;
        }
    }
    
    // If there was no spot available for the input, handle the first one and replace it?
    handleKeystroke(0);
    keystrokes[0] = keystroke;
}
void InputManager::handleKeystroke(int index) {
    if (keystrokes[index]) {
        delete keystrokes[index];
        keystrokes[index] = nullptr;
    }
}

// STATIC FUNCTION //
void InputManager::callbackKeystroke(GLFWwindow* context, int key, int scancode, int action, int modifiers) {
    Keystroke* state = new Keystroke();
    // state->shift = glfwGetKey(context, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(context, GLFW_KEY_RIGHT_SHIFT);
    // state->control = glfwGetKey(context, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(context, GLFW_KEY_RIGHT_CONTROL);
    // state->alt = glfwGetKey(context, GLFW_KEY_LEFT_ALT) || glfwGetKey(context, GLFW_KEY_RIGHT_ALT);
    // state->super = glfwGetKey(context, GLFW_KEY_HOME);

    state->shift = (modifiers & (1<<(GLFW_MOD_SHIFT))); // Check if shift bit is set to true
    state->control = (modifiers & (1<<(GLFW_MOD_CONTROL))); // Check if control bit is set to true
    state->alt = (modifiers & (1<<(GLFW_MOD_ALT)));
    state->super = (modifiers & (1<<(GLFW_MOD_SUPER)));
    state->capsMode = (modifiers & (1<<(GLFW_MOD_CAPS_LOCK)));
}