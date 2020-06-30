#pragma once

#include <CL/cl.h>
#include <CL/cl_gl.h>

#include <GL/glx.h>
#include <GLFW/glfw3.h>

#include <cstdio>

#include "src/config.h"

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
};