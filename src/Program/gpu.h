#pragma once

#include <CL/cl.h>
#include <CL/cl_gl.h>

#include <cstdio>

class GPU {
  public:
    GPU();
    void Close();

    cl_platform_id platformID;
    cl_device_id deviceID;
    cl_context context;
    cl_command_queue queue;
};