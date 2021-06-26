#pragma once

#include "src/config.h"

#ifdef DEBUG
    #include <cstdio>
#endif

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "src/Objects/Tools/brush.h"
#include "src/Objects/Input/keystroke.h"

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

    // GLOBAL VARIABLES //
    extern Keystroke** keystrokes;
    void appendKeystroke(Keystroke* keystroke);
    void handleKeystroke(int index);
    void callbackKeystroke(GLFWwindow* context, int key, int scancode, int action, int modifiers);
    
    // DEBUG UTILS //
    #ifdef DEBUG
    void DEBUG_Print_Keystroke(Keystroke* state);

    #endif
};