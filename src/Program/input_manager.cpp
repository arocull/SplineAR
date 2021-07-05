#include "src/Program/input_manager.h"

GLFWwindow* InputManager::context;
Brush* InputManager::brush = nullptr;
bool InputManager::mouseWasDown = false;
Keystroke** InputManager::keystrokes = nullptr;
bool InputManager::mouseWasDownUI = false;

void InputManager::Initialize() {


    keystrokes = (Keystroke**) malloc(KEYSTROKE_BUFFER_SIZE * sizeof(Keystroke*));
    for (int i = 0; i < KEYSTROKE_BUFFER_SIZE; i++) {
        keystrokes[i] = nullptr;
    }
}
void InputManager::Deallocate() {
    for (int i = 0; i < KEYSTROKE_BUFFER_SIZE; i++) {
        handleKeystroke(i);
    }
    free(keystrokes);
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


// KEYSTROKES //

// Append Keystroke - Appends the given keystroke to the keystroke array
void InputManager::appendKeystroke(Keystroke* keystroke) {
    #ifdef DEBUG_VERBOSE
        printf("Inputted ");
        DEBUG_Print_Keystroke(keystroke);
    #endif

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
// Marks the given keystroke as handled, and frees it
void InputManager::handleKeystroke(int index) {
    if (keystrokes[index]) {
        #ifdef DEBUG_VERBOSE
            printf("Freeing keystroke %i\n\t", index);
            DEBUG_Print_Keystroke(keystrokes[index]);
        #endif

        delete keystrokes[index];
        keystrokes[index] = nullptr;
    }
}
// Callback function for GLFW's input function, adds the given item to the keystrokes array
// TODO: Does this need a mutex?
void InputManager::callbackKeystroke(GLFWwindow* context, int key, int scancode, int action, int modifiers) {
    if (action == GLFW_RELEASE) return; // Don't record inputs for released keys
    if (action == GLFW_REPEAT) return; // Don't record inputs for repeated keys

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

    state->key = key;
    state->scancode = scancode;

    InputManager::appendKeystroke(state);
}

// Clicks the given interface button
void InputManager::clickButton(UIFrame* button, struct IEClick* event) {
    button->click(event);

    // Click event was handled, return it
    free(event);
}
// Sorts through the given interface array, and returns the current button that the mouse cursor is over
UIFrame* InputManager::checkButton(std::vector<UIFrame*> interfaces) {
    // Get mouse position proportional to screen
    int windowWidth, windowHeight;
    double mouseX, mouseY;
    glfwGetCursorPos(context, &mouseX, &mouseY);
    glfwGetWindowSize(context, &windowWidth, &windowHeight);
    glm::vec2 clickPos = glm::vec2(mouseX / windowWidth, mouseY / windowHeight);

    // Iterate through *all* UI frames, store any frame that is containing the click event
    // If a new clicked frame is a child of the current stored UI frame, replace the stored frame
    // If a new clicked frame is a parent of the given current stored frame, ignore it (need hierchary check function)

    // If a later on a new element is overlapping the current one, and there is no relationship, pick the new one
    // - Being lower in the list currently acts as our Z-Index method

    UIFrame* selected = nullptr;
    for (int i = 0; i < interfaces.size(); i++) {
        if (
            interfaces[i] == nullptr ||
            !interfaces[i]->interactable ||
            !interfaces[i]->containsPosition(clickPos)
        ) { continue; } // Ignore non-existant / non-interactable / non-clicked frames

        if (nullptr == selected) { selected = interfaces[i]; } // Take first oprtion if one has not been selected yet
        else if (interfaces[i]->isParentedBy(selected)) { selected = interfaces[i]; } // New parent
        else if (selected->isParentedBy(interfaces[i])) { continue; } // Currently selected frame is parented by us, ignore
        else { selected = interfaces[i]; } // If no relationship is present, overrite earlier UI frames, as new ones have higher Z Index order
    }

    return selected;
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