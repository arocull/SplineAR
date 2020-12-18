#pragma once


/* Compiler Flags */
#define CL_TARGET_OPENCL_VERSION 220

#define PLATFORM_LINUX
// #define PLATFORM_WINDOWS
// #define PLATFORM_MACOS

#define DEBUG         // If enabled, certain developer features and error-catches will be enabled
// #define DEBUG_VERBOSE // If enabled, system will print essentially all relevant information
// #define Enable3D

/* Configuration */

/* PROGRAM CONFIG */
    // Predefined window width for when program is opened
    #define WindowWidth 1000
    // Predefined window height for when program is opened
    #define WindowHeight 800

    // Maximum number of CPU threads that can be used
    #define MAX_THREADS 1


/* PIPELINE CONFIG */
    // Number of primary shaders the grahpics pipeline uses (used for memory allocation and array iteration)
    #define NUM_SHADERS_PRIMARY 2
    #define NUM_SHADERS_CUSTOM 5

    // Number of memory objects used for processing info on brush strokes within the graphics pipeline (used for memory allocation and array iteration)
    #define NUM_STROKE_DATA_BUFFERS 7


/* INPUT CONFIG */
    #define MaxInputSamples 1000 // Number of X and Y samples of a stroke input

#define MAX_STROKES 20 // Maximum number of brush strokes visible / simulating at a time
#define EXPECTED_STROKE_POINTS 12 // Base, expected number of stroke points for base GPU memory allocation--lean a bit higher for this