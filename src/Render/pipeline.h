#pragma once

#include "src/config.h"

#include <vector>

#include "src/Util/gl_headers.h"
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/Program/window.h"
#include "src/Program/gpu.h"
#include "src/Program/gpu_memory.h"
#include "src/Program/gl_texture_handler.h"
#include "src/Render/shader/shader_gl.h"
#include "src/Program/fonts.h"
#include "src/Objects/Art/point.h"
#include "src/Objects/Art/stroke.h"
#include "src/Objects/Interface/uiframe.h"
#include "src/Objects/Interface/ui_manager.h"

class Pipeline {
    public:
        Pipeline(GPU* pipelineGPU, PWindow* programWindow);

    private:
        GPU* gpu;
        PWindow* window;

        GLTextureHandler* canvas;
        cl_sampler uvSampler;

        ShaderGL* fontShader;
        glm::mat4x4 fontProjection;

        cl_mem clTime;
        cl_mem clMEM_MaxStrokes;
        cl_mem clMEM_MaxPoints;
        cl_mem clMEM_WindowWidth; 
        cl_mem clMEM_WindowHeight;

        cl_program* shaderPrograms;
        cl_kernel* shaderKernels;

        bool pipelineRunning = false; // TODO: Use mutex for this?
        bool canRunPipeline = true;

        float time = 0.0f;
        int maxStrokes = MAX_STROKES;   // Maximum number of drawable strokes
        int maxPoints = 0;              // Maximum number of points

        GPUMemory** strokeData;
        GPUMemory* strokeOutlineIndices;

        GLuint FontVertexArray;
        GLuint FontVertexBuffer;

    public:
        // Performs basic setup for GLFW window
        void SetupContext();
        // Closes the pipeline and frees whatever memory it was consuming--halts program until current pipeline is completed (if running)
        void Close();

        // Runs the graphics pipeline--Goes from OpenGL, to OpenCL, to buffer swap
        void RunPipeline(float DeltaTime, Stroke** strokes, std::vector<UIFrame*> interfaces);

    protected:
        // Start pipeline -- runs initial setup, buffer clears, applies camera transforms, etc
        void StartFrame(Stroke** strokes, int width, int height);
        // Run GL and CL pipelines
        void MidFrame(Stroke** strokes, int width, int height, std::vector<UIFrame*> interfaces);
        // Swaps buffers and pops out of matrix transforms
        void EndFrame();

        void CleanseGLState();

        // Does prepwork for rendering text
        void RenderTextMode();
        // Renders actual text, call RenderTextMode before performing
        void RenderText(std::string text, glm::vec2 pos, float scale, glm::vec3 color, Fonts::FFont* font);
};