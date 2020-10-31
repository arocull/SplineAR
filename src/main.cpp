// Written by Alan O'Cull
// Contributions are welcome

#include "src/config.h"

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <math.h>

#include "src/Program/window.h"
#include "src/Program/gpu.h"
#include "src/Render/pipeline.h"
#include "src/Program/thread_manager.h"

#include "src/Objects/Art/stroke.h"

// https://software.intel.com/content/www/us/en/develop/articles/opencl-and-opengl-interoperability-tutorial.html
int main(int argc, char **argv) {
    // Initialize openGL and PWindow
    if (!glfwInit()) return -1;

    PWindow window = PWindow(WindowWidth, WindowHeight, "OpenCL GL Testing");
    if (!window.glWindow) {
        const char** errorMsg;
        int errorCode = glfwGetError(errorMsg);
        perror("Error: could not create GL window!\n");
        printf("Code %i with message %s\n", errorCode, *errorMsg);

        return -1;
    }

    // Initialize GLEW and set up rendering context
    glfwMakeContextCurrent(window.glWindow);

    if (glewInit() != GLEW_OK) { perror("GLEW NOT OKAY, ABORT ABORT\n"); return -1; }

    #ifdef DEBUG_VERBOSE
        int versionMajor, versionMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
        printf("OpenGL version %i.%i\n", versionMajor, versionMinor);
    #endif


    // Initialize OpenCL
    GPU gpu = GPU();
    Pipeline pipeline = Pipeline(&gpu, window.glWindow);

    pipeline.SetupContext();


    // Initialize threads
    // ThreadManager threads = ThreadManager(window.glWindow);
    // threads.StartThreads();

    stroke_info* strokes = (stroke_info*) calloc(MAX_STROKES, sizeof(stroke_info));
    for (int i = 0; i < MAX_STROKES; i++) {
        STROKE_BlankData(&strokes[i], 0, true);
    }

    STROKE_InitializeData(&strokes[0], 4);
    STROKE_SetPointData(&strokes[0], 0, 0.2, 0.05, 1, 20);
    STROKE_SetPointData(&strokes[0], 1, 0.5, 0.5, 1, 50);
    STROKE_SetPointData(&strokes[0], 2, 0.7, 0.7, 1, 40);
    STROKE_SetPointData(&strokes[0], 3, 0.5, 0.3, 1, 20);
    strokes[0].closed = true;
    STROKE_InitializeData(&strokes[1], 1);
    STROKE_SetPointData(&strokes[1], 0, 0.1, 0.1, 1, 10);

    // Basic interop test
    glm::vec4 testPointData = STROKE_GetPoint(&strokes[0], 0.5);
    printf("Test point (%f, %f, %f) with thickness %f\n", testPointData.x, testPointData.y, testPointData.z, testPointData.w);
    
    // Set up clock for delta time fetching
    timespec lastTime;
    timespec currentTime;
    float timeRun = 0.0f;
    int frames = 0;
    if (clock_gettime(CLOCK_MONOTONIC, &lastTime) != 0) {
        perror("Error on fetching initial time!");
        return -1;
    }

    printf("Passed initialization.\n");
    // Run until window closes
    while(!glfwWindowShouldClose(window.glWindow)) {
        glfwPollEvents();

        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        float DeltaTime = ((currentTime.tv_sec - lastTime.tv_sec) * 1e9 + (currentTime.tv_nsec - lastTime.tv_nsec)) * 1e-9;
        lastTime = currentTime;
        timeRun += DeltaTime;
        frames++;

        pipeline.RunPipeline(DeltaTime, strokes);

        strokes[0].points[0].x = (1 + cos(timeRun / 2)) / 2;
        strokes[0].points[0].y = (1 + + sin(timeRun)) / 2;
    }
    printf("Loop ended. Closing program...\n");


    // Free OpenCL
    pipeline.Close();
    gpu.Close();

    window.Close();
    glfwTerminate();

    for (int i = 0; i < MAX_STROKES; i++) {
        STROKE_ScalePointData(&strokes[i], 0); // Deallocate stroke data, but don't attempt to free strokes that don't have point data present anyways
        //free(strokes[i].shaders);
    }
    free(strokes);

    printf("\nClosed successfully!\n\tTotal frames was %i with an estimated average FPS of %f\n", frames, frames / timeRun);

    return 0;
}