#pragma once

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "src/config.h"
#include "src/Program/gpu.h"

class Pipeline {
    public:
        Pipeline(GPU* pipelineGPU, GLFWwindow* windowContext);

    private:
        GPU* gpu;
        GLFWwindow* window;

        GLuint canvasGL;
        cl_mem canvasCL;
        cl_sampler uvSampler;
        cl_mem clTime;

        cl_program* shaderPrograms;
        cl_kernel* shaderKernels;

        bool pipelineRunning = false;
        bool canRunPipeline = true;

        float time = 0.0f;

    public:
        // Performs basic setup for GLFW window
        void SetupContext();
        // Closes the pipeline and frees whatever memory it was consuming--halts program until current pipeline is completed (if running)
        void Close();

        // Runs the graphics pipeline--Goes from OpenGL, to OpenCL, to buffer swap
        void RunPipeline(float DeltaTime);

    protected:
        // Start pipeline -- runs initial setup, buffer clears, applies camera transforms, etc
        void StartFrame();
        // Run GL and CL pipelines
        void MidFrame(int width, int height);
        // Swaps buffers and pops out of matrix transforms
        void EndFrame();
};