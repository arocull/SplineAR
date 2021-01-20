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

#include "src/Objects/Enum/enums.h"

#include "src/Program/window.h"
#include "src/Program/gpu.h"
#include "src/Render/pipeline.h"
#include "src/Program/thread_manager.h"
#include "src/Program/input_manager.h"

#include "src/Objects/Workspace/workspace.h"
#include "src/Objects/Art/point.h"
#include "src/Objects/Art/stroke.h"
#include "src/Objects/Tools/brush.h"

// https://software.intel.com/content/www/us/en/develop/articles/opencl-and-opengl-interoperability-tutorial.html
int main(int argc, char **argv) {
    // Initialize openGL and PWindow
    if (!glfwInit()) return -1;

    PWindow window = PWindow(WindowWidth, WindowHeight, "SplineAR");
    if (!window.glWindow) {
        perror("Error: could not create GL window!\n");

        const char** errorMsg;
        #ifdef PLATFORM_MINT
        int errorCode = glGetError();
        printf("Error Code %i\n", errorCode);
        #else
        int errorCode = glfwGetError(errorMsg);
        printf("Error Code %i with message %s\n", errorCode, *errorMsg);
        #endif

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
    #ifndef DISABLE_GPU
    GPU gpu = GPU();
    Pipeline pipeline = Pipeline(&gpu, &window);
    pipeline.SetupContext();
    #endif


    // Set up input sampler
    InputManager* inputManager = new InputManager(window.glWindow);


    // Initialize threads
    // ThreadManager threads = ThreadManager(window.glWindow);
    // threads.StartThreads();

    // Create a workspace
    Workspace* workspace = new Workspace("TestWindow");
    window.UpdateTitle(workspace->getName(), workspace->getMode());

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

        // Set active drawing brush to the current brush the workspace is using
        inputManager->setBrush(workspace->getBrush());

        workspace->tick(DeltaTime);

        Stroke* newStroke = inputManager->tickInput(); // Tick stroke input, recieve new stroke if new one was created
        if (newStroke) { // If a stroke was created, go through all strokes and fill the next one with this one
            bool placed = workspace->addStroke(newStroke);
            if (!placed) { // If we are unable to place the stroke, forcibly end the stroke and delete it
                inputManager->forceEndStroke();
                delete newStroke;
            }
        }

        window.checkResizing();
        #ifndef DISABLE_GPU
        pipeline.RunPipeline(DeltaTime, workspace->getStrokeArray(EWorkMode::EMDraw));
        #endif
    }
    printf("Loop ended. Closing program...\n");

    // Close workspace
    delete workspace;

    // Free OpenCL
    #ifndef DISABLE_GPU
    pipeline.Close();
    gpu.Close();
    #endif

    printf("Deleting input manager\n");
    delete inputManager;

    printf("Closing window\n");
    window.Close();
    glfwTerminate();

    printf("\nClosed successfully!\n\tTotal frames was %i with an estimated average FPS of %f\n", frames, frames / timeRun);

    return 0;
}