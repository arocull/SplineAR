#pragma once

#include "src/config.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "src/Program/gpu.h"
#include "src/Program/gpu_memory.h"
#include "src/Objects/Art/point.h"
#include "src/Objects/Art/stroke.h"

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

        // GPUMemory* strokeData[NUM_STROKE_DATA_BUFFERS];

    public:
        // Performs basic setup for GLFW window
        void SetupContext();
        // Closes the pipeline and frees whatever memory it was consuming--halts program until current pipeline is completed (if running)
        void Close();

        // Runs the graphics pipeline--Goes from OpenGL, to OpenCL, to buffer swap
        void RunPipeline(float DeltaTime, Stroke** strokes);

    protected:
        // Start pipeline -- runs initial setup, buffer clears, applies camera transforms, etc
        void StartFrame(Stroke** strokes, int width, int height);
        // Run GL and CL pipelines
        void MidFrame(Stroke** strokes, int width, int height);
        // Swaps buffers and pops out of matrix transforms
        void EndFrame();
};