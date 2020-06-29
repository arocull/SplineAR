#include "src/Program/gpu.h"

GPU::GPU() {
    // printf("Supported platforms %i, GL/CL %i, WGL HDC %i\n", platform, hRC, hDC);

    cl_uint numPlatforms;
    cl_uint numDevices;

    cl_int returnValue = clGetPlatformIDs(1, &platformID, &numPlatforms);
    #ifdef DEBUG_VERBOSE
        printf("Searched for valid CL Platform with error code %i\n", returnValue);
    #endif
    returnValue = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, &numDevices);
    printf("Platform and device info gathered with error code %i\n\tNumber of platforms %i; Number of devices %i\n", returnValue, numPlatforms, numDevices);

    #ifdef DEBUG_VERBOSE
        size_t extensionListSize;
        clGetDeviceInfo(deviceID, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionListSize);
        char extensions[extensionListSize];
        clGetDeviceInfo(deviceID, CL_DEVICE_EXTENSIONS, extensionListSize, extensions, &extensionListSize);
        printf("\nSupported GL extensions include:\n\t%s\n\n", extensions);
        // Note: cl_khr_gl_sharing should be included in this list if this program is to work
    #endif


    cl_context_properties hRC;
    clGetGLContextInfoKHR(&hRC, CL_DEVICES_FOR_GL_CONTEXT_KHR, 1, NULL, NULL);
    
    cl_context_properties props[] = {
        // CL_CONTEXT_PLATFORM, (cl_context_properties) platformID,
        CL_GL_CONTEXT_KHR, (cl_context_properties) hRC,
        // CL_WGL_HDC_KHR, (cl_context_properties) ,
        0
    };

    // Create Context
    context = clCreateContext(NULL, 1, &deviceID, NULL, NULL, &returnValue);
    #ifdef DEBUG_VERBOSE
        printf("Created an OpenCL context with error code %i\n", returnValue);
    #endif
    queue = clCreateCommandQueueWithProperties(context, deviceID, NULL, &returnValue);
    #ifdef DEBUG_VERBOSE
        printf("Created an OpenCL queue with error code %i\n", returnValue);
    #endif
}

void GPU::Close() {
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseDevice(deviceID);
}