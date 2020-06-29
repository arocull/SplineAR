#include "src/Render/pipeline.h"

Pipeline::Pipeline() {

}

void Pipeline::SetupContext() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_FRONT);
}


void Pipeline::RunPipeline(GPU* gpu, GLFWwindow* context, float DeltaTime) {
    int width, height;
    glfwGetWindowSize(context, &width, &height);

    StartFrame();

    MidFrame(gpu, (size_t) width, (size_t) height);

    EndFrame(context);
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
void Pipeline::MidFrame(GPU* gpu, size_t width, size_t height) {
    // clEnqueueAcquireGLObjects(gpu->queue, 1, &canvasTextureCL, 0, 0, NULL);

    // run kernel
    // clEnqueueNDRangeKernel(gpu->queue, kernel, 2, NULL, &width, &height, 0, NULL, NULL);

    // Finish
    clFinish(gpu->queue);
    // clEnqueueReleaseGLObjects(gpu->queue, 1, &canvasTextureCL, 0, 0, NULL);
}
void Pipeline::EndFrame(GLFWwindow* context) {
    glfwSwapBuffers(context);
}