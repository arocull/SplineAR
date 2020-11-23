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
#include "src/Program/input_manager.h"

#include "src/Objects/Art/point.h"
#include "src/Objects/Art/stroke.h"
#include "src/Objects/Tools/brush.h"

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


    // Set up input sampler
    InputManager* inputManager = new InputManager(window.glWindow);


    // Initialize threads
    // ThreadManager threads = ThreadManager(window.glWindow);
    // threads.StartThreads();

    Stroke** strokes = (Stroke**) calloc(MAX_STROKES, sizeof(Stroke*));
    for (int i = 0; i < MAX_STROKES; i++) {
        strokes[i] = nullptr;
    }
    //strokes[0] = new Stroke();
    //strokes[0]->pushPoint(glm::vec2(0.3 * WindowWidth, 0.7 * WindowHeight));
    //strokes[0]->closed = true;
//
    //strokes[0]->points[0]->pos.x = 0.2 * WindowWidth;
    //strokes[0]->points[0]->pos.y = 0.2 * WindowHeight;
    //strokes[0]->points[1]->pos.x = 0.4 * WindowWidth;
    //strokes[0]->points[1]->pos.y = 0.5 * WindowHeight;
    //strokes[0]->points[0]->thickness = 50;
    //strokes[0]->points[1]->thickness = 50;
    //strokes[0]->points[2]->thickness = 40;

    Brush* brush = new Brush();
    inputManager->setBrush(brush);


    // Basic interop test
    // Point* testPointData = strokes[0]->getPoint(0.5f);
    // printf("Test point (%f, %f) with thickness %f\n", testPointData->pos.x, testPointData->pos.y, testPointData->thickness);
    // delete testPointData;

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

        Stroke* newStroke = inputManager->tickInput();
        if (newStroke) { // If a stroke was created, go through all strokes and fill the next one with this one
            bool placed = false;
            for (int i = 0; i < MAX_STROKES && !placed; i++) {
                if (!strokes[i]) {
                    strokes[i] = newStroke;
                    placed = true;
                }
            }
            if (!placed) { // If we are unable to place the stroke, forcibly end the stroke and delete it
                brush->endStroke();
                delete newStroke;
            }
        }

        pipeline.RunPipeline(DeltaTime, strokes);

        //strokes[0]->points[0]->pos.x = WindowWidth * (1 + cos(timeRun / 2)) / 2;
        //strokes[0]->points[0]->pos.y = WindowHeight * (1 + sin(timeRun)) / 2;
    }
    printf("Loop ended. Closing program...\n");


    // Free OpenCL
    pipeline.Close();
    gpu.Close();

    delete inputManager;

    window.Close();
    glfwTerminate();

    delete brush;

    for (int i = 0; i < MAX_STROKES; i++) {
        if (strokes[i]) delete strokes[i];
    }
    free(strokes);

    printf("\nClosed successfully!\n\tTotal frames was %i with an estimated average FPS of %f\n", frames, frames / timeRun);

    return 0;
}