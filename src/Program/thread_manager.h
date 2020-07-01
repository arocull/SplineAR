#pragma once

#define _POSIX_PTHREAD_SEMANTICS

#include "src/config.h"

#include <cstdlib>
#include <cstdio>
#include <pthread.h>

#include <GLFW/glfw3.h>

// Data struct given to thread
typedef struct input_thread_data {
    int thread_id;
    bool mouse_down;
    bool mouse_was_down;
    double x_samples[MaxInputSamples];
    double y_samples[MaxInputSamples];
    int currentSample;
    GLFWwindow* context;
} input_thread_data;

class ThreadManager {
    public:
        ThreadManager(GLFWwindow* glfwContext);
    
    private:
        GLFWwindow* context;

        input_thread_data* input_data;

    public:
        void StartThreads();
        void StopThreads();
};