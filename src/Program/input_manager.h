#pragma once

#include "src/config.h"

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "src/Objects/Tools/brush.h"

class InputManager {
    public:
        InputManager(GLFWwindow* glfwContext);
        ~InputManager();

    private:
        GLFWwindow* context;
        Brush* brush = nullptr;

        bool mouseWasDown = false;

    public:
        void setBrush(Brush* newBrush);
        Stroke* tickInput();
        void forceEndStroke();
};