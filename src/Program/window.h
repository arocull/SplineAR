#pragma once

#include <GLFW/glfw3.h>

class Window {
    public:
        Window(int windowWidth, int windowHeight, const char* windowName);
        void Close();

        GLFWwindow* glWindow;

        int width;
        int height;

    public:
        void Resize();
};