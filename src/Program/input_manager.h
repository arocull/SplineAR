#pragma once

#include "src/config.h"

#ifdef DEBUG
    #include <cstdio>
#endif

#include <glm/vec2.hpp>
#include "src/Util/gl_headers.h"

#include "src/Objects/Tools/brush.h"
#include "src/Objects/Input/keystroke.h"
#include "src/Objects/Input/click.h"

// Input Manager - Global namespace used for keeping track of inputs
namespace InputManager {
    // STRUCTS / ENUMS //

    // (Enum) Input - Mouse State - Current state of mouse, use > 1 for pressed, < 2 for released
    enum MouseState {
        EI_MS_Released = 0,
        EI_MS_JustReleased = 1,
        EI_MS_JustPressed = 2,
        EI_MS_Pressed = 3,
    };


    // OVERALL //
    void Initialize();
    void Deallocate();

    // CONTEXT MANAGEMENT //
    void RegisterContext(GLFWwindow* glfwContext);
    extern GLFWwindow* context;
    
    // BRUSHES //
    extern Brush* brush;
    extern bool mouseWasDown;

    void setBrush(Brush* newBrush);
    Stroke* tickInput();
    void forceEndStroke();

    // KEYSTROKES //
    extern Keystroke** keystrokes;
    void appendKeystroke(Keystroke* keystroke);
    void handleKeystroke(int index);
    void callbackKeystroke(GLFWwindow* context, int key, int scancode, int action, int modifiers);

    // UI BUTTONS //
    extern bool mouseWasDownUI;
    void getMousePosition(double *mouseX, double *mouseY, int *windowWidth, int *windowHeight);
    InputManager::MouseState checkMouseState();
    
    // DEBUG UTILS //
    #ifdef DEBUG
    void DEBUG_Print_Keystroke(Keystroke* state);

    #endif
};