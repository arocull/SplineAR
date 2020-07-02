#pragma once

#include "src/config.h"

#include <CL/cl.h>
#include <CL/cl_gl.h>

#include <GL/glx.h>
#include <GLFW/glfw3.h>

#include <cstdio>

class GPU {
  public:
    GPU();
    void Close();

    cl_platform_id platformID;
    cl_device_id deviceID;
    cl_context context;
    cl_command_queue queue;
  
  public:
    // Builds a CL Program and compiles a CL Kernel from a file path and shader name
    void BuildShaderFromFile(cl_program *program, cl_kernel *kernel, const char* path, const char* shader_name);
    // Builds a CL Program and compiles a CL Kernel from a string and shader name
    void BuildShader(cl_program *program, cl_kernel *kernel, const char* source, const char* shader_name);

    // Allocates a cl_mem object for the given data, flags default to read-write
    // Note: Memory is NOT automatically deallocated
    cl_mem AllocateMemory(int spaces, size_t objectSize, cl_mem_flags flags = CL_MEM_READ_WRITE);
    // Writes to a cl_mem object with the given value, returns an OpenCL error code
    int WriteMemory(cl_mem memory, const void* value, size_t memorySize, bool blockingWrite = false);
};