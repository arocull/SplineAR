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
};