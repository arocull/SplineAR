#include "src/Program/input_manager.h"

GLFWwindow* InputManager::context;
Brush* InputManager::brush = nullptr;
bool InputManager::mouseWasDown = false;
std::vector<Keystroke*> InputManager::keystrokes = std::vector<Keystroke*>();
bool InputManager::mouseWasDownUI = false;

void InputManager::Initialize() {
    keystrokes.reserve(3);
}
void InputManager::Deallocate() {
    for (int i = 0; i < keystrokes.size(); i++) {
        handleKeystroke(i);
    }
    keystrokes.clear();
}


// Register Context - Takes the given GLFW window and makes it the InputManager's context
// TODO: Handle context as an array (for multiple windows)?
void InputManager::RegisterContext(GLFWwindow* newContext) {
    context = newContext;
    glfwSetKeyCallback(context, callbackKeystroke);
}


// BRUSHES //
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


// KEYSTROKES //

// Append Keystroke - Appends the given keystroke to the keystroke array
void InputManager::appendKeystroke(Keystroke* keystroke) {
    #ifdef DEBUG_VERBOSE
        printf("Inputted ");
        DEBUG_Print_Keystroke(keystroke);
    #endif

    keystrokes.push_back(keystroke);
}
// Marks the given keystroke as handled, and frees it
void InputManager::handleKeystroke(int index) {
    if (keystrokes[index]) {
        #ifdef DEBUG_VERBOSE
            printf("Freeing keystroke %i\n\t", index);
            DEBUG_Print_Keystroke(keystrokes[index]);
        #endif

        delete keystrokes[index]; // Delete keystroke object at this position

        auto iterator = keystrokes.begin(); // Get iterator to front of keystroke array
        iterator += index; // Move forward X indices
        keystrokes.erase(iterator); // Erase this item from the array
    }
}
// Callback function for GLFW's input function, adds the given item to the keystrokes array
// TODO: Does this need a mutex?
void InputManager::callbackKeystroke(GLFWwindow* context, int key, int scancode, int action, int modifiers) {
    if (action == GLFW_RELEASE) return; // Don't record inputs for released keys
    if (action == GLFW_REPEAT) return; // Don't record inputs for repeated keys

    Keystroke* state = new Keystroke();

    state->shift = (modifiers & (1<<(GLFW_MOD_SHIFT))); // Check if shift bit is set to true
    state->control = (modifiers & (1<<(GLFW_MOD_CONTROL))); // Check if control bit is set to true
    state->alt = (modifiers & (1<<(GLFW_MOD_ALT)));
    state->super = (modifiers & (1<<(GLFW_MOD_SUPER)));
    state->capsMode = (modifiers & (1<<(GLFW_MOD_CAPS_LOCK)));

    state->key = key;
    state->scancode = scancode;
    state->modifiers = state->shift + state->control + state->alt + state->super;

    InputManager::appendKeystroke(state);
}


void InputManager::getMousePosition(double *mouseX, double *mouseY, int *windowWidth, int *windowHeight) {
    glfwGetCursorPos(context, mouseX, mouseY);
    glfwGetWindowSize(context, windowWidth, windowHeight);
}
InputManager::MouseState InputManager::checkMouseState() {
    bool mouseDown = glfwGetMouseButton(context, GLFW_MOUSE_BUTTON_LEFT);
    InputManager::MouseState state;

    if (mouseDown && !mouseWasDownUI) { state = InputManager::MouseState::EI_MS_JustPressed; }
    else if (!mouseDown && mouseWasDownUI) { state = InputManager::MouseState::EI_MS_JustReleased; }
    else if (mouseDown) { state = InputManager::MouseState::EI_MS_Pressed; }
    else state = InputManager::MouseState::EI_MS_Released;

    mouseWasDownUI = mouseDown;
    return state;
}


// DEBUG //
#ifdef DEBUG
void InputManager::DEBUG_Print_Keystroke(Keystroke* state) {
    printf("Keystroke: Key %i [%s]\tScancode %i\n\tSHIFT %i, CNTRL %i, ALT %i, SUPER %i, CAPS %i\n",
        state->key, glfwGetKeyName(state->key, state->scancode), state->scancode,
        state->shift, state->control, state->alt, state->super, state->capsMode
    );
}

#endif