#include "src/Render/pipeline.h"

Pipeline::Pipeline(GPU* pipelineGPU, PWindow* programWindow) {
    gpu = pipelineGPU;
    window = programWindow;

    // Compile Initial Shaders
    shaderPrograms = (cl_program*) calloc(NUM_SHADERS_PRIMARY, sizeof(cl_program));
    shaderKernels = (cl_kernel*) calloc(NUM_SHADERS_PRIMARY, sizeof(cl_kernel));

    // Build shaders
    gpu->BuildShaderFromFile(&(shaderPrograms[0]), &(shaderKernels[0]), "src/Render/cl_kernels/draw_stroke_outlines.cl", "draw_stroke_outlines", "-I../cl_headers");
    gpu->BuildShaderFromFile(&(shaderPrograms[1]), &(shaderKernels[1]), "src/Render/cl_kernels/fill_strokes.cl", "fill_strokes", "-I../cl_headers");

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
    clMEM_MaxStrokes = clCreateBuffer(gpu->context, CL_MEM_READ_ONLY, sizeof(int), NULL, NULL); // Maximum number of strokes
    clMEM_MaxPoints = clCreateBuffer(gpu->context, CL_MEM_READ_ONLY, sizeof(int), NULL, NULL); // Maximum number of strokes
    clMEM_WindowWidth = clCreateBuffer(gpu->context, CL_MEM_READ_ONLY, sizeof(int), NULL, NULL); // Horizontal width of the window in pixels
    clMEM_WindowHeight = clCreateBuffer(gpu->context, CL_MEM_READ_ONLY, sizeof(int), NULL, NULL); // Vertical height of the window in pixels

    // Store a 2D array of the indices of various stroke outlines
    // - This can be used later on to calculate fills
    // - Only used on GPU, should only need to be copied once
    strokeOutlineIndices = new GPUMemory(gpu, WindowWidth * WindowHeight, sizeof(cl_int), CL_MEM_READ_WRITE);
    cl_int* outlineIndices = (cl_int*) strokeOutlineIndices->GetData(); // Cast void* to int* so we can write to it
    for (int i = 0; i < WindowWidth * WindowHeight; i++) { // Iterate through every single pixel
        outlineIndices[i] = 0;
    }
    strokeOutlineIndices->CopyDataToGPU(); // Copy blank slate to GPU for initialization
    printf("Done blanking Stroke Outline Data\n");


    // Allocate memory for strokes data buffers
    // GPU Memory objects automatically allocate themselves on CPU and GPU (initialized on both for conversion)
    strokeData = (GPUMemory**) calloc(NUM_STROKE_DATA_BUFFERS, sizeof(GPUMemory*));

    strokeData[0] = new GPUMemory(gpu, MAX_STROKES, sizeof(cl_int), CL_MEM_READ_ONLY);  // Number of points in stroke
    strokeData[1] = new GPUMemory(gpu, MAX_STROKES, sizeof(cl_char), CL_MEM_READ_ONLY); // Stroke closed--NOTE: WE USE CHARS HERE DUE TO INCONSISTENT BOOL SIZING
    strokeData[2] = new GPUMemory(gpu, MAX_STROKES, sizeof(cl_char), CL_MEM_READ_ONLY); // Stroke filled--NOTE: WE USE CHARS HERE DUE TO INCONSISTENT BOOL SIZING
    strokeData[3] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float2), CL_MEM_READ_ONLY); // XY positions of points
    strokeData[4] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float2), CL_MEM_READ_ONLY); // XY left handles
    strokeData[5] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float2), CL_MEM_READ_ONLY); // XY right handles
    strokeData[6] = new GPUMemory(gpu, MAX_STROKES * EXPECTED_STROKE_POINTS, sizeof(cl_float), CL_MEM_READ_ONLY);  // Point thickness

    

    // SET KERNEL ARGUMENTS / PARAMETERS //

    // Draw Stroke Outlines
    clSetKernelArg(shaderKernels[0], 0, sizeof(canvas->GetCL()), canvas->GetCLReference()); // Set image to the first arguement of the given Kernel
    clSetKernelArg(shaderKernels[0], 1, sizeof(clTime), &clTime);     // Set the time to the second arguement of the given Kernel
    clSetKernelArg(shaderKernels[0], 2, sizeof(clMEM_MaxStrokes), &clMEM_MaxStrokes);
    clSetKernelArg(shaderKernels[0], 3, sizeof(clMEM_MaxPoints), &clMEM_MaxPoints);
    clSetKernelArg(shaderKernels[0], 4, sizeof(clMEM_WindowWidth), &clMEM_WindowWidth);
    clSetKernelArg(shaderKernels[0], 5, sizeof(clMEM_WindowHeight), &clMEM_WindowHeight);
    clSetKernelArg(shaderKernels[0], 6, sizeof(strokeOutlineIndices->GetGPUData()), strokeOutlineIndices->GetGPUData());

    for (int i = 0; i < NUM_STROKE_DATA_BUFFERS; i++) {   // Set kernel arguements for stroke data
        clSetKernelArg(shaderKernels[0], 7 + i, sizeof(strokeData[i]->GetGPUData()), strokeData[i]->GetGPUData());
    }

    // Fill Strokes
    clSetKernelArg(shaderKernels[1], 0, sizeof(canvas->GetCL()), canvas->GetCLReference());
    clSetKernelArg(shaderKernels[1], 1, sizeof(clTime), &clTime);
    clSetKernelArg(shaderKernels[1], 2, sizeof(clMEM_MaxStrokes), &clMEM_MaxStrokes);
    clSetKernelArg(shaderKernels[1], 3, sizeof(clMEM_WindowWidth), &clMEM_WindowWidth);
    clSetKernelArg(shaderKernels[1], 4, sizeof(clMEM_WindowHeight), &clMEM_WindowHeight);
    clSetKernelArg(shaderKernels[1], 5, sizeof(strokeOutlineIndices->GetGPUData()), strokeOutlineIndices->GetGPUData());
    clSetKernelArg(shaderKernels[1], 6, sizeof(strokeData[1]->GetGPUData()), strokeData[1]->GetGPUData()); // Whether stroke is closed or not
    clSetKernelArg(shaderKernels[1], 7, sizeof(strokeData[2]->GetGPUData()), strokeData[2]->GetGPUData()); // Whether stroke is filled or not
    


    //clSetKernelArg(shaderKernels[0], 1, sizeof(uvSampler), &uvSampler);
    #ifdef DEBUG
        printf("Pipeline setup complete!\n");
    #endif
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

    #ifdef DEBUG
        printf("Closing pipeline:\n\tReleasing CL memory objects\n");
    #endif
    clReleaseSampler(uvSampler);
    clReleaseMemObject(clTime);
    clReleaseMemObject(clMEM_MaxStrokes);
    clReleaseMemObject(clMEM_MaxPoints);
    clReleaseMemObject(clMEM_WindowWidth);
    clReleaseMemObject(clMEM_WindowHeight);

    #ifdef DEBUG
        printf("\tFreeing stroke outline index array\n");
    #endif
    strokeOutlineIndices->FreeMemory();
    delete strokeOutlineIndices;
    strokeOutlineIndices = nullptr;

    #ifdef DEBUG
        printf("\tFreeing stroke data buffers and storage array\n");
    #endif
    for (int i = 0; i < NUM_STROKE_DATA_BUFFERS; i++) {
        if (strokeData[i]) {
            strokeData[i]->FreeMemory();
            delete strokeData[i];
            strokeData[i] = nullptr;
        }
    }
    free(strokeData);

    #ifdef DEBUG
        printf("\tFreeing canvas texture\n");
    #endif
    delete canvas;

    #ifdef DEBUG
        printf("\tReleasing shader kernels and programs\n");
        // Oddly, a segfault happens somewhere around here, but it is inconsistent and will not say.
        // After adding the print statements here, however, it has disappeared??
        for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
            printf("\t\tReleasing kernel %i\n", i);
            clReleaseKernel(shaderKernels[i]);
            printf("\t\tReleasing program %i\n", i);
            clReleaseProgram(shaderPrograms[i]);
        }
    #else
        for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
            clReleaseKernel(shaderKernels[i]);
            clReleaseProgram(shaderPrograms[i]);
        }
    #endif
}


void Pipeline::RunPipeline(float DeltaTime, Stroke** strokes, std::vector<UIFrame*> interfaces) {
    if (!canRunPipeline || pipelineRunning) return;
    pipelineRunning = true;

    time += DeltaTime;

    StartFrame(strokes, window->getViewWidth(), window->getViewHeight());
    MidFrame(strokes, window->getViewWidth(), window->getViewHeight(), interfaces);
    EndFrame();

    pipelineRunning = false;
}


void Pipeline::StartFrame(Stroke** strokes, int width, int height) {
    // Set up GL projection matrix
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClearDepth(10.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    double windowAspect = (double) width / (double) height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 10); // TODO: Make left side and right side of windowAspect centered
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    #ifdef DEBUG_VERBOSE
        printf("\tSet up GL projection mode to Orthographic with a window aspect of %f (scale %i, %i)\n", windowAspect, width, height);
    #endif
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Reset coloring


    // Start updating memory and piping stroke info into OpenCL (requires data conversion)--potentially call read-lock here?
    gpu->WriteMemory(clTime, &time, sizeof(float)); // Updates CL time to be same as shader
    gpu->WriteMemory(clMEM_WindowWidth, &width, sizeof(int)); // Updates CL window width
    gpu->WriteMemory(clMEM_WindowHeight, &height, sizeof(int)); // Updates CL window height

    if (width * height != strokeOutlineIndices->GetMaxAmount()) { // Resize stroke outline indices incase window grows or shrinks
        strokeOutlineIndices->Reallocate(width * height, false, false);
        strokeOutlineIndices->CopyDataToGPU();
    }

    {  // Pre-cast arrays so they do not need to be casted repeatedly, section off to avoid variable confusion, and convert stroke data to be meaningful on GPU
        cl_int* numPoints =         ((cl_int*) (strokeData[0]->GetData()));
        cl_char* closed =           ((cl_char*) (strokeData[1]->GetData()));
        cl_char* filled =           ((cl_char*) (strokeData[2]->GetData()));
        cl_float2* positions =      ((cl_float2*) (strokeData[3]->GetData()));
        cl_float2* leftHandles =    ((cl_float2*) (strokeData[4]->GetData()));
        cl_float2* rightHandles =   ((cl_float2*) (strokeData[5]->GetData()));
        cl_float* thickness =       ((cl_float*) (strokeData[6]->GetData()));

        maxPoints = 0;
        for (int i = 0, pointIndex = 0; i < maxStrokes; i++) {
            if (strokes[i]) {
                int strokeLen = strokes[i]->length();
                numPoints[i] = strokeLen;
                closed[i] = (cl_char) strokes[i]->closed;
                filled[i] = (cl_char) strokes[i]->filled;

                maxPoints += strokeLen;
                if (maxPoints > strokeData[3]->GetMaxAmount()) { // If the number of points is greater than currently allocated, reallocate arrays
                    strokeData[3]->Reallocate(maxPoints * 1.4);
                    strokeData[4]->Reallocate(maxPoints * 1.4);
                    strokeData[5]->Reallocate(maxPoints * 1.4);
                    strokeData[6]->Reallocate(maxPoints * 1.4);

                    // Re-cast new blocks of memory
                    positions =      ((cl_float2*) (strokeData[3]->GetData()));
                    leftHandles =    ((cl_float2*) (strokeData[4]->GetData()));
                    rightHandles =   ((cl_float2*) (strokeData[5]->GetData()));
                    thickness =      ((cl_float*) (strokeData[6]->GetData()));

                    printf("\tReallocated stroke data to account for new max points %i (from %i)\n", strokeData[3]->GetMaxAmount(), maxPoints);
                }

                for (int j = 0; j < strokeLen; pointIndex++, j++) {
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

    // Don't update MaxStrokes and MaxPoints until after they are recalculated
    gpu->WriteMemory(clMEM_MaxStrokes, &maxStrokes, sizeof(int)); // Updates CL max strokes (incase of rescaling)
    gpu->WriteMemory(clMEM_MaxPoints, &maxPoints, sizeof(int)); // Updates CL max points

    // Then copy generated data into GPU
    for (int i = 0; i < NUM_STROKE_DATA_BUFFERS; i++) {
        strokeData[i]->CopyDataToGPU();
    }


    glFinish(); // Finish OpenGL queue before allowing OpenCL to grasp GL objects
    #ifdef DEBUG_VERBOSE
        printf("Start Frame process finished!\n");
    #endif
}
void Pipeline::MidFrame(Stroke** strokes, int width, int height, std::vector<UIFrame*> interfaces) {
    // Fetch OpenGL objects for use in OpenCL
    canvas->HoldCLTexture(gpu);

    // Perform remaining memory updates

    // Run shaders
    size_t work_size[] = {(size_t) width, (size_t) height};
    clEnqueueNDRangeKernel(gpu->queue, shaderKernels[0], 2, NULL, work_size, 0, 0, NULL, NULL);
    //for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
    //    clEnqueueNDRangeKernel(gpu->queue, shaderKernels[i], 2, NULL, work_size, 0, 0, NULL, NULL);
    //}

    // Finish OpenCL to allow OpenGL to have control of objects again
    clFinish(gpu->queue);

    // Release hold of OpenGL objects
    canvas->ReleaseCLTexture(gpu);

    // Draw updated Canvas texture
    canvas->BindGLTexture();
    canvas->DrawGLTexture(
        window->getViewHorizontalOffsetGL(),
        window->getViewVerticalOffsetGL(),
        window->getViewHorizontalOffsetGL() + window->getViewWidthGL(),
        window->getViewVerticalOffsetGL() + window->getViewHeightGL()
    );
    canvas->UnbindGLTexture();

    // Draw strokes ontop of texture
    /*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    }*/

    // Reset canvas to ignore aspect ratio, for drawing UI
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // glOrtho(0, 1, 0, -1, 0, 10);
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // glClear( GL_DEPTH_BUFFER_BIT );
    for (int i = 0; i < interfaces.size(); i++) {
        if (interfaces[i]) {
            interfaces[i]->draw(glm::vec2(1, 1));
        }
    }

    // Do not need to finish or flush GL thanks to buffer swap
    #ifdef DEBUG_VERBOSE
        printf("Mid Frame process finished!\n");
    #endif
}
void Pipeline::EndFrame() {
    glfwSwapBuffers(window->getGLWindow());
}