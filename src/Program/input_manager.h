#pragma once

#include "src/config.h"

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "src/Objects/Tools/brush.h"
#include "src/Objects/Input/keystroke.h"

class InputManager {
    public:
        InputManager(GLFWwindow* glfwContext);
        ~InputManager();

    private:
        GLFWwindow* context;
        Brush* brush = nullptr;

        bool mouseWasDown = false;

        Keystroke** keystrokes;
        void appendKeystroke(Keystroke* keystroke);

    public:
        void setBrush(Brush* newBrush);
        Stroke* tickInput();
        void forceEndStroke();

        void handleKeystroke(int index);
        static void callbackKeystroke(GLFWwindow* context, int key, int scancode, int action, int modifiers);
};