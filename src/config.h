#pragma once


/* Compiler Flags */
#define PLATFORM_LINUX
// #define PLATFORM_WINDOWS
// #define PLATFORM_MACOS

#define DEBUG_VERBOSE // If enabled, system will print essentially all relevant information
// #define Enable3D


/* Configuration */

// Predefined window width and height for when program is opened
#define WindowWidth 600
#define WindowHeight 400

// Number of primary shaders the grahpics pipeline uses (used for memory allocation and array iteration)
#define NumPrimaryShaders 1