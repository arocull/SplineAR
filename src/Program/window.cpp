#include "src/Program/window.h"

Window::Window(int windowWidth, int windowHeight, const char* windowName) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    width = windowWidth;
    height = windowHeight;

    glWindow = glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr);
    // glfwSetFramebufferSizeCallback(framebuffer_size_callback);
}
void Window::Close() {
    glfwDestroyWindow(glWindow);
}

void Window::Resize() {
    glfwGetFramebufferSize(glWindow, &width, &height);
    // glViewport(0, 0, width, height);
}