#pragma once

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include "src/Program/gpu.h"

class Pipeline {
    public:
        Pipeline();

    public:
        void SetupContext();

        void RunPipeline(GPU* gpu, GLFWwindow* context, float DeltaTime);

    protected:
        // Start pipeline -- runs initial setup, buffer clears, applies camera transforms, etc
        void StartFrame();
        // Run GL and CL pipelines
        void MidFrame(GPU* gpu, size_t width, size_t height);
        // Swaps buffers and pops out of matrix transforms
        void EndFrame(GLFWwindow* context);
};