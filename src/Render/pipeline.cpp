#include "src/Render/pipeline.h"

Pipeline::Pipeline(GPU* pipelineGPU, GLFWwindow* windowContext) {
    gpu = pipelineGPU;
    window = windowContext;

    // Compile Initial Shaders
    shaderPrograms = (cl_program*) calloc(NUM_SHADERS_PRIMARY, sizeof(cl_program));
    shaderKernels = (cl_kernel*) calloc(NUM_SHADERS_PRIMARY, sizeof(cl_kernel));

    // Build shaders
    gpu->BuildShaderFromFile(&(shaderPrograms[0]), &(shaderKernels[0]), "src/Render/cl_kernels/draw_strokes.cl", "draw_strokes");


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

    stroke_mem_data = (cl_mem*) calloc(NUM_STROKE_DATA_CLMEM, sizeof(cl_mem));
    stroke_mem_data[0] = gpu->AllocateMemory(MAX_STROKES, sizeof(cl_int), CL_MEM_READ_ONLY);     // Allocate stroke_id
    stroke_mem_data[1] = gpu->AllocateMemory(MAX_STROKES, sizeof(cl_int), CL_MEM_READ_ONLY);     // Allocate number of points for X stroke
    #ifdef Enable3D
        stroke_mem_data[2] = gpu->AllocateMemory(MAX_STROKES * MAX_STROKE_POINTS, sizeof(cl_float3));  // X, Y and Z positions of stroke
        stroke_mem_data[3] = gpu->AllocateMemory(MAX_STROKES * MAX_STROKE_POINTS, sizeof(cl_float3));  // X, Y and Z directions of stroke
    #else
        stroke_mem_data[2] = gpu->AllocateMemory(MAX_STROKES * MAX_STROKE_POINTS, sizeof(cl_float2));  // X and Y positions of stroke
        stroke_mem_data[3] = gpu->AllocateMemory(MAX_STROKES * MAX_STROKE_POINTS, sizeof(cl_float2));  // X and Y directions of stroke
    #endif
    stroke_mem_data[4] = gpu->AllocateMemory(MAX_STROKES * MAX_STROKE_POINTS, sizeof(cl_float));   // Line thickness at given point
    stroke_mem_data[5] = gpu->AllocateMemory(MAX_STROKES * NUM_SHADERS_CUSTOM, sizeof(cl_bool), CL_MEM_READ_ONLY);   // What custom shaders are applied to the stroke

    // Initialize stroke data on CPU as well in preparation for GPU conversion
    strokeData_id = (cl_int*) malloc(MAX_STROKES * sizeof(cl_int));
    strokeData_numPoints = (cl_int*) malloc(MAX_STROKES * sizeof(cl_int));
    strokeData_positions = (cl_float2*) malloc(MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_float2));
    strokeData_directions = (cl_float2*) malloc(MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_float2));
    strokeData_thickness =  (cl_float*) malloc(MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_float));
    strokeData_shaders =  (cl_bool*) malloc(MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_bool));


    // Set (Default) Kernel Arguements
    for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
        clSetKernelArg(shaderKernels[i], 0, sizeof(canvasCL), &canvasCL); // Set image to the first arguement of the given Kernel
        clSetKernelArg(shaderKernels[i], 1, sizeof(clTime), &clTime);     // Set the time to the second arguement of the given Kernel
    }
    
    for (int i = 0; i < NUM_STROKE_DATA_CLMEM; i++) {   // Set kernel arguements for stroke data
        clSetKernelArg(shaderKernels[0], 2 + i, sizeof(stroke_mem_data[i]), &stroke_mem_data[i]);
    }

    //clSetKernelArg(shaderKernels[0], 1, sizeof(uvSampler), &uvSampler);
}

void Pipeline::SetupContext() {
    glEnable(GL_DEPTH_TEST | GL_BLEND);
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

    for (int i = 0; i < NUM_STROKE_DATA_CLMEM; i++) {
        clReleaseMemObject(stroke_mem_data[i]);
    }

    clReleaseMemObject(canvasCL);
    glDeleteTextures(1, &canvasGL);

    for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
        clReleaseKernel(shaderKernels[i]);
        clReleaseProgram(shaderPrograms[i]);
    }

    free(strokeData_id);
    free(strokeData_numPoints);
    free(strokeData_positions);
    free(strokeData_directions);
    free(strokeData_thickness);
    free(strokeData_shaders);
}


void Pipeline::RunPipeline(float DeltaTime, stroke_info* strokes) {
    if (!canRunPipeline || pipelineRunning) return;
    pipelineRunning = true;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    time += DeltaTime;

    StartFrame(strokes);
    MidFrame(WindowWidth, WindowHeight);
    EndFrame();

    pipelineRunning = false;
}


void Pipeline::StartFrame(stroke_info* strokes) {
    // Set up GL projection matrix
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearDepth(5.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 1000);
    // gluPerspective(70, width / height, 1.0f, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Start updating memory and piping stroke info into OpenCL (requires data conversion)--potentially call read-lock here?
    gpu->WriteMemory(clTime, &time, sizeof(float)); // Updates CL time to be same as shader


    // Convert stroke data to be meaningfull on GPU
    for (int i = 0; i < MAX_STROKES; i++) {
        if (!strokes[i].visible) { // If the stroke is not visible, set GPU version as inactive and do not bother updating info
            strokes[i].stroke_id = -1;
            continue;
        } else strokeData_id[i] = strokes[i].stroke_id;
        strokeData_numPoints[i] = strokes[i].numPoints;

        for (int j = 0; j < MAX_STROKE_POINTS; j++) {
            int index = MAX_STROKE_POINTS * i + j;
            strokeData_positions[index] = cl_float2();
            strokeData_positions[index].x = strokes[i].points[j].x;
            strokeData_positions[index].y = WindowHeight - strokes[i].points[j].y;

            strokeData_directions[index] = cl_float2();
            strokeData_directions[index].x = strokes[i].dir[j].x;
            strokeData_directions[index].y = -strokes[i].dir[j].y;

            strokeData_thickness[index] = strokes[i].thickness[j];
        }

        for (int j = 0; j < NUM_SHADERS_CUSTOM; j++) {
            int index = MAX_STROKE_POINTS * i + j;
            strokeData_shaders[index] = strokes[i].shaders[j];
        }
    }

    // Then copy it over
    gpu->WriteMemory(stroke_mem_data[0], strokeData_id, MAX_STROKES * sizeof(cl_int));
    gpu->WriteMemory(stroke_mem_data[1], strokeData_numPoints, MAX_STROKES * sizeof(cl_int));
    gpu->WriteMemory(stroke_mem_data[2], strokeData_positions, MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_float2));
    gpu->WriteMemory(stroke_mem_data[3], strokeData_directions, MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_float2));
    gpu->WriteMemory(stroke_mem_data[4], strokeData_thickness, MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_float));
    gpu->WriteMemory(stroke_mem_data[5], strokeData_shaders, MAX_STROKES * MAX_STROKE_POINTS * sizeof(cl_bool));


    glFinish(); // Finish OpenGL queue before allowing OpenCL to grasp GL objects
}
void Pipeline::MidFrame(int width, int height) {
    // Fetch OpenGL objects for use in OpenCL
    clEnqueueAcquireGLObjects(gpu->queue, 1, &canvasCL, 0, 0, NULL);

    // Perform remaining memory updates

    // Run shaders
    size_t work_size[] = {(size_t) width, (size_t) height};
    for (int i = 0; i < NUM_SHADERS_PRIMARY; i++) {
        clEnqueueNDRangeKernel(gpu->queue, shaderKernels[i], 2, NULL, work_size, 0, 0, NULL, NULL);
    }


    // Release hold of OpenGL objects
    clEnqueueReleaseGLObjects(gpu->queue, 1, &canvasCL, 0, 0, NULL);

    // Finish OpenCL to allow OpenGL to have control of ojbects again
    clFinish(gpu->queue);

    // Draw updated Canvas texture
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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

    // Do not need to finish or flush GL thanks to buffer swap
}
void Pipeline::EndFrame() {
    glfwSwapBuffers(window);
}