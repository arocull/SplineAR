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

    stroke_info* strokes = (stroke_info*) calloc(MAX_STROKES, 100);
    for (int i = 0; i < MAX_STROKES; i++) {
        BlankStrokeData(&strokes[i], true);
    }

    InitializeStrokeData(&strokes[0], 200, 50);
    strokes[0].numPoints = 2;
    strokes[0].points[1] = glm::vec2(WindowWidth / 2, WindowHeight / 2);
    strokes[0].thickness[1] = 50.0f;
    InitializeStrokeData(&strokes[1], 120, 20);

    
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

        strokes[0].points[0].x = (WindowWidth / 2) * (1 + cos(timeRun / 2));
        strokes[0].points[0].y = (WindowHeight / 2) * (1 + + sin(timeRun));
    }


    // Free OpenCL
    pipeline.Close();
    gpu.Close();

    window.Close();
    glfwTerminate();

    for (int i = 0; i < MAX_STROKES; i++) {
        free(strokes[i].points);
        free(strokes[i].dir);
        free(strokes[i].thickness);
        free(strokes[i].alpha);
        free(strokes[i].shaders);
    }
    free(strokes);

    printf("\nClosed successfully!\n\tTotal frames was %i with an estimated average FPS of %f\n", frames, frames / timeRun);

    return 0;
}