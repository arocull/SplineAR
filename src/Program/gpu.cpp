#include "src/Program/gpu.h"

GPU::GPU() {
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

    cl_uint numPlatforms;
    cl_uint numDevices;

    cl_int returnValue = clGetPlatformIDs(1, &platformID, &numPlatforms);
    returnValue = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, &numDevices);
    printf("Platform and device info gathered\n\tNumber of platforms %i; Number of devices %i\n", numPlatforms, numDevices);

    // Create Context
    context = clCreateContext(NULL, 1, &deviceID, NULL, NULL, &returnValue);
    queue = clCreateCommandQueueWithProperties(context, deviceID, NULL, &returnValue);
    printf("Created an OpenCL context and queue\n");
}

void GPU::Close() {
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseDevice(deviceID);
}