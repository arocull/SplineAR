#include "src/Render/pipeline.h"

Pipeline::Pipeline(GPU* pipelineGPU, GLFWwindow* windowContext) {
    gpu = pipelineGPU;
    window = windowContext;

    // Compile Initial Shaders
    shaderPrograms = (cl_program*) calloc(NUM_SHADERS_PRIMARY, sizeof(cl_program));
    shaderKernels = (cl_kernel*) calloc(NUM_SHADERS_PRIMARY, sizeof(cl_kernel));

    // Build shaders
    gpu->BuildShaderFromFile(&(shaderPrograms[0]), &(shaderKernels[0]), "src/Render/cl_kernels/draw_strokes_two.cl", "draw_strokes");

    // Generate Canvas Texture and Pipe into OpenCL
    canvas = new GLTextureHandler(gpu, WindowWidth, WindowHeight);

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
    clMaxStrokes = clCreateBuffer(gpu->context, CL_MEM_READ_ONLY, sizeof(int), NULL, NULL); // Maximum number of strokes



    // Allocate memory for strokes data buffers
    // GPU Memory objects automatically allocate themselves on CPU and GPU (initialized on both for conversion)
    strokeData = (GPUMemory**) calloc(NUM_STROKE_DATA_BUFFERS, sizeof(GPUMemory*));

    strokeData[0] = new GPUMemory(gpu, MAX_STROKES, sizeof(cl_int), CL_MEM_READ_ONLY);  // Number of points in stroke
    strokeData[1] = new GPUMemory(gpu, MAX_STROKES, sizeof(cl_bool), CL_MEM_READ_ONLY); // Stroke closed
    strokeData[2] = new GPUMemory(gpu, MAX_STROKES, sizeof(cl_bool), CL_MEM_READ_ONLY); // Stroke filled
    strokeData[3] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float2), CL_MEM_READ_ONLY); // XY positions of points
    strokeData[4] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float2), CL_MEM_READ_ONLY); // XY left handles
    strokeData[5] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float2), CL_MEM_READ_ONLY); // XY right handles
    strokeData[6] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float), CL_MEM_READ_ONLY);  // Point thickness

    

    // Set (Default) Kernel Arguements
    for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
        clSetKernelArg(shaderKernels[i], 0, sizeof(canvas->GetCL()), canvas->GetCLReference()); // Set image to the first arguement of the given Kernel
        clSetKernelArg(shaderKernels[i], 1, sizeof(clTime), &clTime);     // Set the time to the second arguement of the given Kernel
        clSetKernelArg(shaderKernels[i], 2, sizeof(clMaxStrokes), &clMaxStrokes);
    }
    

    for (int i = 0; i < NUM_STROKE_DATA_BUFFERS; i++) {   // Set kernel arguements for stroke data
        clSetKernelArg(shaderKernels[0], 3 + i, sizeof(strokeData[i]->GetGPUData()), strokeData[i]->GetGPUData());
    }

    //clSetKernelArg(shaderKernels[0], 1, sizeof(uvSampler), &uvSampler);
}

void Pipeline::SetupContext() {
    glEnable(GL_BLEND); // | GL_DEPTH_TEST (disabled unless we want to do 3D)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    // Allow image to transparent and blend
    glDepthFunc(GL_FRONT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Set color before an image for color multiplication
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

    for (int i = 0; i < NUM_STROKE_DATA_BUFFERS; i++) {
        if (strokeData[i]) {
            strokeData[i]->FreeMemory();
            free(strokeData[i]);
            strokeData[i] = nullptr;
        }
    }
    free(strokeData);

    canvas->Free();

    for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
        clReleaseKernel(shaderKernels[i]);
        clReleaseProgram(shaderPrograms[i]);
    }
}


void Pipeline::RunPipeline(float DeltaTime, Stroke** strokes) {
    if (!canRunPipeline || pipelineRunning) return;
    pipelineRunning = true;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    time += DeltaTime;

    StartFrame(strokes, width, height);
    MidFrame(strokes, width, height);
    EndFrame();

    pipelineRunning = false;
}


void Pipeline::StartFrame(Stroke** strokes, int width, int height) {
    // Set up GL projection matrix
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClearDepth(10.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    double windowAspect = width / height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowAspect, 0, 1, 0, 10); // TODO: Make left side and right side of windowAspect centered
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Start updating memory and piping stroke info into OpenCL (requires data conversion)--potentially call read-lock here?
    gpu->WriteMemory(clTime, &time, sizeof(float)); // Updates CL time to be same as shader
    gpu->WriteMemory(clMaxStrokes, &maxStrokes, sizeof(int)); // Updates CL max strokes (incase of rescaling)

    {  // Pre-cast arrays so they do not need to be casted repeatedly, section off to avoid variable confusion, and convert stroke data to be meaningful on GPU
        cl_int* numPoints =         ((cl_int*) (strokeData[0]->GetData()));
        cl_bool* closed =           ((cl_bool*) (strokeData[1]->GetData()));
        cl_bool* filled =           ((cl_bool*) (strokeData[2]->GetData()));
        cl_float2* positions =      ((cl_float2*) (strokeData[3]->GetData()));
        cl_float2* leftHandles =    ((cl_float2*) (strokeData[4]->GetData()));
        cl_float2* rightHandles =   ((cl_float2*) (strokeData[5]->GetData()));
        cl_float* thickness =       ((cl_float*) (strokeData[6]->GetData()));

        for (int i = 0, pointIndex = 0; i < maxStrokes; i++) {
            if (strokes[i]) {
                numPoints[i] = strokes[i]->length();
                closed[i] = strokes[i]->closed;
                filled[i] = strokes[i]->filled;


                for (int j = 0; j < strokes[i]->length(); pointIndex++, j++) {
                    positions[pointIndex] = cl_float2();
                    positions[pointIndex].x = strokes[i]->points[j]->pos.x;
                    positions[pointIndex].y = strokes[i]->points[j]->pos.y;

                    leftHandles[pointIndex] = cl_float2();
                    leftHandles[pointIndex].x = strokes[i]->points[j]->leftHandle.x;
                    leftHandles[pointIndex].y = strokes[i]->points[j]->leftHandle.y;

                    rightHandles[pointIndex] = cl_float2();
                    rightHandles[pointIndex].x = strokes[i]->points[j]->rightHandle.x;
                    rightHandles[pointIndex].y = strokes[i]->points[j]->rightHandle.y;

                    thickness[pointIndex] = strokes[i]->points[j]->thickness;
                }
            } else { // Ensure we don't try and draw unset points
                numPoints[i] = -1;
                closed[i] = false;
                filled[i] = false;
            }
        }
    }

    // Then copy generated data into GPU
    for (int i = 0; i < NUM_STROKE_DATA_BUFFERS; i++) {
        strokeData[i]->CopyDataToGPU();
    }


    glFinish(); // Finish OpenGL queue before allowing OpenCL to grasp GL objects
}
void Pipeline::MidFrame(Stroke** strokes, int width, int height) {
    // Fetch OpenGL objects for use in OpenCL
    canvas->HoldCLTexture(gpu);

    // Perform remaining memory updates

    // Run shaders
    size_t work_size[] = {(size_t) width, (size_t) height};
    for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
        clEnqueueNDRangeKernel(gpu->queue, shaderKernels[i], 2, NULL, work_size, 0, 0, NULL, NULL);
    }

    // Finish OpenCL to allow OpenGL to have control of objects again
    clFinish(gpu->queue);

    // Release hold of OpenGL objects
    canvas->ReleaseCLTexture(gpu);

    // Draw updated Canvas texture
    canvas->BindGLTexture();
    canvas->DrawGLTexture();
    canvas->UnbindGLTexture();

    // Draw strokes ontop of texture
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i < MAX_STROKES; i++) {
        Stroke* stroke = strokes[i];
        if (stroke && stroke->visible) {
            #ifdef DEBUG_VERBOSE
                printf("\tBeginning GL polygon of stroke %i\n", i);
            #endif
            int strokeLen = stroke->length();

            if (stroke->closed && strokeLen > 2) {   // Only fill the shape if it is closed
                glColor3f(1.0f, 0.0f, ((float) i / MAX_STROKES));
                glBegin(GL_POLYGON);
                for (int x = 0; x < strokeLen; x++) {
                    glVertex2f(stroke->points[x]->pos.x, stroke->points[x]->pos.y);
                }
                glEnd();
            }
            
            // Outline stroke
            glColor3f(1.0f, 1.0f, 1.0f);
            for (int x = 1; x < strokeLen; x++) {
                glLineWidth(stroke->points[x]->thickness);
                glBegin(GL_LINES);
                glVertex2f(stroke->points[x-1]->pos.x, stroke->points[x-1]->pos.y);
                glVertex2f(stroke->points[x]->pos.x, stroke->points[x]->pos.y);
                glEnd();

                // printf("\t\tDrawing point %f, %f\n", stroke->points[x]->pos.x, stroke->points[x]->pos.y);
            }

            if (stroke->closed) { // Closing line for shape
                glLineWidth(stroke->points[strokeLen-1]->thickness);
                glBegin(GL_LINES);
                glVertex2f(stroke->points[strokeLen-1]->pos.x, stroke->points[strokeLen-1]->pos.y);
                glVertex2f(stroke->points[0]->pos.x, stroke->points[0]->pos.y);
                glEnd();
            }
        }
    }

    // Do not need to finish or flush GL thanks to buffer swap
}
void Pipeline::EndFrame() {
    glfwSwapBuffers(window);
}