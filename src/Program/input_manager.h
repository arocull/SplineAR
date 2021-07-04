#pragma once

#include "src/config.h"

#ifdef DEBUG
    #include <cstdio>
#endif

#include <glm/vec2.hpp>
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "src/Objects/Tools/brush.h"
#include "src/Objects/Input/keystroke.h"
#include "src/Objects/Input/click.h"
#include "src/Objects/Interface/uiframe.h"

namespace InputManager {
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
    void clickButton(UIFrame* button, struct IEClick* event);
    void checkButtons(UIFrame** interfaces, struct IEClick* event);
    void checkMousedown(UIFrame** interfaces);
    
    // DEBUG UTILS //
    #ifdef DEBUG
    void DEBUG_Print_Keystroke(Keystroke* state);

    #endif
};