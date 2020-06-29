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

// https://software.intel.com/content/www/us/en/develop/articles/opencl-and-opengl-interoperability-tutorial.html
int main(int argc, char **argv) {
    int platform;
    int hRC;
    int hDC;

    cl_context_properties props[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
        CL_GL_CONTEXT_KHR, (cl_context_properties) hRC,
        CL_WGL_HDC_KHR, (cl_context_properties) hDC,
        0
    };

    printf("Supported platforms %i, GL/CL %i, WGL HDC %i\n", platform, hRC, hDC);

    cl_platform_id platformID;
    cl_uint numPlatforms;
    cl_device_id deviceID;
    cl_uint numDevices;

    cl_int returnValue = clGetPlatformIDs(1, &platformID, &numPlatforms);
    returnValue = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, &numDevices);
    printf("Platform and device info gathered\n\tNumber of platforms %i; Number of devices %i\n", numPlatforms, numDevices);

    // Create Context
    cl_context context = clCreateContext(NULL, 1, &deviceID, NULL, NULL, &returnValue);
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, deviceID, NULL, &returnValue);
    printf("Created an OpenCL context and queue\n");




    // Create OpenGL Texture
    GLuint canvasTexture;
    glGenTextures(1, &canvasTexture);
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei) WindowWidth, (GLsizei) WindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // Pipe into OpenCL
    cl_int errorReturn = 0;
    cl_mem canvasTextureCL = clCreateFromGLTexture2D(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, canvasTexture, &errorReturn);

    glFinish();
    clEnqueueAcquireGLObjects(queue, 1, &canvasTextureCL, 0, 0, NULL);

    // run kernel
    clFinish(queue);
    clEnqueueReleaseGLObjects(queue, 1, &canvasTextureCL, 0, 0, NULL);

    // Free Texture
    clReleaseMemObject(canvasTextureCL);
    clReleaseCommandQueue(queue);
    glDeleteTextures(1, &canvasTexture);


    // Free OpenCL
    clReleaseContext(context);
    clReleaseDevice(deviceID);
}