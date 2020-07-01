#pragma once


/* Compiler Flags */
#define CL_TARGET_OPENCL_VERSION 220

#define PLATFORM_LINUX
// #define PLATFORM_WINDOWS
// #define PLATFORM_MACOS

#define DEBUG_VERBOSE // If enabled, system will print essentially all relevant information
// #define Enable3D

/* Configuration */

// Predefined window width and height for when program is opened
#define WindowWidth 1000
#define WindowHeight 800

// Maximum number of CPU threads that can be used
#define MAX_THREADS 3

// Number of primary shaders the grahpics pipeline uses (used for memory allocation and array iteration)
#define NumPrimaryShaders 1

// Number of X and Y samples of a stroke input
#define MaxInputSamples 1000