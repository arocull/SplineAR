#pragma once

#include "src/Objects/Enum/enums.h"
#include <GLFW/glfw3.h>
#include <string.h>

class PWindow {
    public:
        PWindow(int windowWidth, int windowHeight, const char* windowName);
        void Close();

        GLFWwindow* glWindow;

    private:
        int width;
        int height;
        float innerViewportWidth = 1.0f;//0.85f;//1.0f;//
        float innerViewportHeight = 1.0f;//0.9f;

        int lastWidthResizing;
        int lastHeightResizing;
        int resizeFrameCounter = 0;

    public:
        GLFWwindow* getGLWindow();
        //GLFWwindow* getContext();

        void checkResizing();
        void resize();
        
        void UpdateTitle(char* workspaceName, EWorkMode workMode);

        // GETTERS //
        int getFullWidth();
        int getFullHeight();

        int getViewWidth();
        int getViewHeight();
        float getViewWidthGL();
        float getViewHeightGL();
        int getViewHorizontalOffset();
        int getViewVerticalOffset();
        float getViewHorizontalOffsetGL();
        float getViewVerticalOffsetGL();
};