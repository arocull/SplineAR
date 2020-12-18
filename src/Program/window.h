#pragma once

#include "src/Objects/Enum/enums.h"
#include <GLFW/glfw3.h>
#include <string.h>

class PWindow {
    public:
        PWindow(int windowWidth, int windowHeight, const char* windowName);
        void Close();

        GLFWwindow* glWindow;

        int width;
        int height;

    public:
        void Resize();
        void UpdateTitle(char* workspaceName, EWorkMode workMode);
};