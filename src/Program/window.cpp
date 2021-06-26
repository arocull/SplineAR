#include "src/Program/window.h"

PWindow::PWindow(int windowWidth, int windowHeight, const char* windowName) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    width = windowWidth;
    height = windowHeight;
    lastWidthResizing = width;
    lastHeightResizing = windowHeight;

    glWindow = glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr);
    glfwSetWindowSizeLimits(glWindow, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);
    // glfwSetFramebufferSizeCallback(framebuffer_size_callback);
}
void PWindow::Close() {
    glfwDestroyWindow(glWindow);
}


GLFWwindow* PWindow::getGLWindow() {
    return glWindow;
}


// PWindow - Check Resizing - Check if resizing is still occurring
// If the window has not been resized for 10+ frames, it will automatically resize the viewport
void PWindow::checkResizing() {
    int newWidth, newHeight;
    glfwGetFramebufferSize(glWindow, &newWidth, &newHeight);

    if (newWidth != lastWidthResizing || newHeight != lastHeightResizing) {
        resizeFrameCounter = 0;
        lastWidthResizing = newWidth;
        lastHeightResizing = newHeight;
    }

    if (resizeFrameCounter >= 0) {
        resizeFrameCounter++;
        if (resizeFrameCounter >= 10) {
            resize();
            resizeFrameCounter = -1; // Do not need to continually resize to the same size
        }
    }
}
// PWindow - Resize - Update window width and height to new values
// Note, this will likely cause some reallocation within the pipeline, so be cautious while performing this action
void PWindow::resize() {
    glfwGetFramebufferSize(glWindow, &width, &height);
    glViewport(0, 0, width, height);
}

// PWindow - Update Title - Updates the title of the Program Window to use the workspace name + work mode
void PWindow::UpdateTitle(char* workspaceName, EWorkMode workMode) {
    char title[64];
    strcpy(title, "SplineAR - ");
    strcat(title, workspaceName);
    strcat(title, " - ");
    strcat(title, ENUM_WorkMode_GetName(workMode));

    glfwSetWindowTitle(glWindow, title);
}


// GETTERS //
int PWindow::getFullWidth() {
    return width;
}
int PWindow::getFullHeight() {
    return height;
}
int PWindow::getViewWidth() {
    return width * innerViewportWidth;
}
int PWindow::getViewHeight() {
    return height * innerViewportHeight;
}
float PWindow::getViewWidthGL() {
    return innerViewportWidth;
}
float PWindow::getViewHeightGL() {
    return innerViewportHeight;
}
int PWindow::getViewHorizontalOffset() {
    return getViewHorizontalOffsetGL() * width;
}
int PWindow::getViewVerticalOffset() {
    return getViewVerticalOffsetGL() * height;
}
float PWindow::getViewHorizontalOffsetGL() {
    return (1.0f - innerViewportWidth) / 2;
}
float PWindow::getViewVerticalOffsetGL() {
    return (1.0f - innerViewportHeight) / 2;
}