#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <cstdlib>
#include <cstdio>
#include <time.h>

#include "src/config.h"

#include "src/Program/window.h"
#include "src/Program/gpu.h"

// https://software.intel.com/content/www/us/en/develop/articles/opencl-and-opengl-interoperability-tutorial.html
int main(int argc, char **argv) {
    // Initialize openGL and PWindow
    if (!glfwInit()) return -1;

    PWindow window = PWindow(WindowWidth, WindowHeight, "OpenCL GL Testing");
    if (!window.glWindow) {
        const char** errorMsg;
        int errorCode = glfwGetError(errorMsg);
        perror("Error: could not create GL window!\n");
        printf("Code %i with message \'%s\'\n", errorCode, *errorMsg);

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

    // Build OpenCL Kernels
    cl_program program;
    cl_kernel kernel;
    gpu.BuildShaderFromFile(&program, &kernel, "src/Render/cl_kernels/test.cl");

    // Create OpenGL Texture
    GLuint canvasTexture;
    glGenTextures(1, &canvasTexture);
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei) WindowWidth, (GLsizei) WindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFinish();

    // Pipe into OpenCL
    cl_int errorcode;
    cl_mem canvasTextureCL = clCreateFromGLTexture(gpu.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, canvasTexture, &errorcode);
    printf("Attempted to bind GL texture to CL with error code %i\n", errorcode);
    clSetKernelArg(kernel, 0, sizeof(canvasTextureCL), &canvasTextureCL);

    errorcode = clEnqueueAcquireGLObjects(gpu.queue, 1, &canvasTextureCL, 0, 0, NULL);
    printf("Attempted to take control of GL object to CL with error code %i\n", errorcode);

    // run kernel
    const size_t width = WindowWidth;
    const size_t height = WindowHeight;
    clEnqueueNDRangeKernel(gpu.queue, kernel, 2, NULL, &width, &height, 0, NULL, NULL);

    // Finish
    clFinish(gpu.queue);
    clEnqueueReleaseGLObjects(gpu.queue, 1, &canvasTextureCL, 0, 0, NULL);

    // Free Texture
    clReleaseMemObject(canvasTextureCL);
    glDeleteTextures(1, &canvasTexture);


    // Free OpenCL
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    gpu.Close();

    window.Close();
    glfwTerminate();

    printf("Closed successfully!\n");

    return 0;
}