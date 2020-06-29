#include "src/Program/window.h"

PWindow::PWindow(int windowWidth, int windowHeight, const char* windowName) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    width = windowWidth;
    height = windowHeight;

    glWindow = glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr);
    // glfwSetFramebufferSizeCallback(framebuffer_size_callback);
}
void PWindow::Close() {
    glfwDestroyWindow(glWindow);
}

void PWindow::Resize() {
    glfwGetFramebufferSize(glWindow, &width, &height);
    // glViewport(0, 0, width, height);
}