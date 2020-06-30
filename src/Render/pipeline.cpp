#include "src/Render/pipeline.h"

Pipeline::Pipeline(GPU* pipelineGPU, GLFWwindow* windowContext) {
    gpu = pipelineGPU;
    window = windowContext;

    // Compile Initial Shaders
    shaderPrograms = (cl_program*) calloc(NumPrimaryShaders, sizeof(cl_program));
    shaderKernels = (cl_kernel*) calloc(NumPrimaryShaders, sizeof(cl_kernel));
    gpu->BuildShaderFromFile(&(shaderPrograms[0]), &(shaderKernels[0]), "src/Render/cl_kernels/test.cl");

    // Generate Canvas Texture and Pipe into OpenCL
    glGenTextures(1, &canvasGL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei) WindowWidth, (GLsizei) WindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFinish();

    canvasCL = clCreateFromGLTexture(gpu->context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, canvasGL, NULL);

    // Set Kernel Arguements
    clSetKernelArg(shaderKernels[0], 0, sizeof(canvasCL), &canvasCL);
}

void Pipeline::SetupContext() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_FRONT);
}

void Pipeline::Close() {
    canRunPipeline = false;
    if (pipelineRunning) {  // Wait for current pipeline to end before deallocating memory
        printf("Halting pipeline close as a pipeline is currently in progress. Waiting...\n");
        while (pipelineRunning) {
            
        }
        printf("Graphics pipeline finished. Closing pipeline...\n");
    }

    clReleaseMemObject(canvasCL);
    glDeleteTextures(1, &canvasGL);

    for (int i = 0; i < NumPrimaryShaders; i++) {
        clReleaseKernel(shaderKernels[i]);
        clReleaseProgram(shaderPrograms[i]);
    }
}


void Pipeline::RunPipeline(float DeltaTime) {
    if (!canRunPipeline) return;
    pipelineRunning = true;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    StartFrame();
    MidFrame((size_t) width, (size_t) height);
    EndFrame();

    pipelineRunning = false;
}


void Pipeline::StartFrame() {
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, 0, 1000);
    // gluPerspective(70, width / height, 1.0f, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glFinish();
}
void Pipeline::MidFrame(size_t width, size_t height) {
    // Fetch OpenGL objects
    clEnqueueAcquireGLObjects(gpu->queue, 1, &canvasCL, 0, 0, NULL);

    // Run shaders
    clEnqueueNDRangeKernel(gpu->queue, shaderKernels[0], 2, NULL, &width, &height, 0, NULL, NULL);

    // Release hold of OpenGL objects
    clEnqueueReleaseGLObjects(gpu->queue, 1, &canvasCL, 0, 0, NULL);

    // Finish OpenCL
    clFinish(gpu->queue);

    // Draw updated Canvas texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, canvasGL); // Bind OpenGL rendering texture to the GL canvas
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0, 0);
    glTexCoord2i(0, 1); glVertex2i(0, 1);
    glTexCoord2i(1, 1); glVertex2i(1, 1);
    glTexCoord2i(1, 0); glVertex2i(1, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Texture is unset

    // Do not need to finish or flush GL due to buffer swap
}
void Pipeline::EndFrame() {
    glfwSwapBuffers(window);
}