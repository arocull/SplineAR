#include "src/Render/pipeline.h"

Pipeline::Pipeline(GPU* pipelineGPU, GLFWwindow* windowContext) {
    gpu = pipelineGPU;
    window = windowContext;

    // Compile Initial Shaders
    shaderPrograms = (cl_program*) calloc(NumPrimaryShaders, sizeof(cl_program));
    shaderKernels = (cl_kernel*) calloc(NumPrimaryShaders, sizeof(cl_kernel));
    gpu->BuildShaderFromFile(&(shaderPrograms[0]), &(shaderKernels[0]), "src/Render/cl_kernels/time_test.cl", "time_test");

    // Generate Canvas Texture and Pipe into OpenCL
    glGenTextures(1, &canvasGL);
    glBindTexture(GL_TEXTURE_2D, canvasGL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFinish();

    canvasCL = clCreateFromGLTexture(gpu->context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, canvasGL, NULL);

    #ifdef DEBUG_VERBOSE
        printf("Created and linked canvas texture for OpenGL and OpenCL\n");
    #endif

    // Generate texture sampler
    cl_sampler_properties uvSamplerProperties[] = {
        CL_SAMPLER_NORMALIZED_COORDS, (cl_sampler_properties) true,
        CL_SAMPLER_ADDRESSING_MODE, (cl_sampler_properties) CL_ADDRESS_CLAMP_TO_EDGE,
        CL_SAMPLER_FILTER_MODE, (cl_sampler_properties) CL_FILTER_NEAREST,
        0
    };
    uvSampler = clCreateSamplerWithProperties(gpu->context, uvSamplerProperties, NULL);

    #ifdef DEBUG_VERBOSE
        printf("Created OpenCL sampler for shader kernels\n");
    #endif


    // Allocate other CL Memory types
    clTime = clCreateBuffer(gpu->context, CL_MEM_READ_ONLY, sizeof(float), NULL, NULL); // Time tracker for OpenCL shaders

    // Set Kernel Arguements
    clSetKernelArg(shaderKernels[0], 0, sizeof(canvasCL), &canvasCL); // Set image to the first arguement of the given Kernel
    clSetKernelArg(shaderKernels[0], 1, sizeof(clTime), &clTime);     // Set the time to the second arguement of the given Kernel
    //clSetKernelArg(shaderKernels[0], 1, sizeof(uvSampler), &uvSampler);
}

void Pipeline::SetupContext() {
    glEnable(GL_DEPTH_TEST | GL_BLEND_COLOR);
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

    clReleaseSampler(uvSampler);
    clReleaseMemObject(clTime);

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

    time += DeltaTime;

    StartFrame();
    MidFrame(WindowWidth, WindowHeight);
    EndFrame();

    pipelineRunning = false;
}


void Pipeline::StartFrame() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearDepth(1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 1000);
    // gluPerspective(70, width / height, 1.0f, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glFinish(); // Finish OpenGL queue before moving to OpenCL
}
void Pipeline::MidFrame(int width, int height) {
    // Fetch OpenGL objects
    clEnqueueAcquireGLObjects(gpu->queue, 1, &canvasCL, 0, 0, NULL);


    // Update memory
    clEnqueueWriteBuffer(gpu->queue, clTime, CL_TRUE, 0, sizeof(float), &time, 0, NULL, NULL); // Updates CL time to be same as shader

    // Run shaders
    size_t work_size[] = {(size_t) width, (size_t) height};
    for (int i = 0; i < NumPrimaryShaders; i++) {
        clEnqueueNDRangeKernel(gpu->queue, shaderKernels[i], 2, NULL, work_size, 0, 0, NULL, NULL);
    }


    // Release hold of OpenGL objects
    clEnqueueReleaseGLObjects(gpu->queue, 1, &canvasCL, 0, 0, NULL);

    // Finish OpenCL
    clFinish(gpu->queue);



    // Draw updated Canvas texture
    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, canvasGL); // Bind OpenGL rendering texture to the GL canvas
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0, 0);   // Set UV and draw coordinates
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