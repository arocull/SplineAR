/**
 * GL Headers Include File
 * This takes import order into account and provides one,
 * easy include for classes that uses these includes.
 */

#pragma once

#define CL_TARGET_OPENCL_VERSION 300

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h> // TODO: THIS IS PLATFORM SPECIFIC
#include <GLFW/glfw3.h>