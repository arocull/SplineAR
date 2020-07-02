#pragma once

#include "src/config.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "src/Program/gpu.h"
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

        cl_mem* stroke_mem_data;
        cl_int* strokeData_id;
        cl_int* strokeData_numPoints;
        cl_float2* strokeData_positions;
        cl_float2* strokeData_directions;
        cl_float* strokeData_thickness;
        cl_bool* strokeData_shaders;

    public:
        // Performs basic setup for GLFW window
        void SetupContext();
        // Closes the pipeline and frees whatever memory it was consuming--halts program until current pipeline is completed (if running)
        void Close();

        // Runs the graphics pipeline--Goes from OpenGL, to OpenCL, to buffer swap
        void RunPipeline(float DeltaTime, stroke_info* strokes);

    protected:
        // Start pipeline -- runs initial setup, buffer clears, applies camera transforms, etc
        void StartFrame(stroke_info* strokes);
        // Run GL and CL pipelines
        void MidFrame(int width, int height);
        // Swaps buffers and pops out of matrix transforms
        void EndFrame();
};