#include "src/Program/gpu.h"

GPU::GPU() {
    // Currently, just select first available platform and device
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

    /* { // Later, we will want to pick and choose the best GPU
        size_t bytes = 0;
        // Notice that extension functions are accessed via pointers 
        // initialized with clGetExtensionFunctionAddressForPlatform.

        // queuring how much bytes we need to read
        clGetGLContextInfoKHR(props, CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, NULL, &bytes);

        // allocating the mem
        size_t devNum = bytes/sizeof(cl_device_id);
        std::vector<cl_device_id> devs (devNum);
        //reading the info
        clGetGLContextInfoKHR(props, CL_DEVICES_FOR_GL_CONTEXT_KHR, bytes, devs, NULL));
        //looping over all devices
        for(size_t i=0; i < devNum; i++) {
            //enumerating the devices for the type, names, CL_DEVICE_EXTENSIONS, etc
            cl_device_type deviceType;
            clGetDeviceInfo(devs[i], CL_DEVICE_TYPE, 1, &deviceType, NULL);

            
            clGetDeviceInfo(devs[i],CL_DEVICE_EXTENSIONS,);
        }
    } */

    // GLX is Linux-only
    cl_context_properties props[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties) platformID,
        CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
        0
    };

    // Create Context
    context = clCreateContext(props, 1, &deviceID, NULL, NULL, &returnValue);
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