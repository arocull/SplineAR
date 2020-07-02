#pragma once

#include "src/config.h"

#include <CL/cl.h>
#include <glm/vec2.hpp>

// Each
typedef struct polynomial {
    float a;
    float b;
    float c;
    float d;
} polynomial;

/* Stroke Info - C Strucutre
* 
* Retains information about a pen stroke
* Contains 2D/3D positions, directions, and curves
* Strokes should be treated like parametric equations (put in value from 0 to 1 for start / end respectively, get output position and dir)
*/
typedef struct stroke_info {
    int stroke_id;
    int numPoints;
    #ifdef Enable3D
        glm::vec3* points;
        glm::vec3* dir;
    #else
        glm::vec2* points;
        glm::vec2* dir;
    #endif
    float* thickness;   // Read line thickness at given point
    float* alpha;       // Read line alpha at given point
    bool* shaders;      // What shaders are enabled on this stroke?
    bool visible;
} stroke_info;


// Fills out stroke data with default values
void BlankStrokeData(stroke_info* stroke, bool doAlloc = false);
// Initialize a given piece of stroke data (makes it 'active' and fills out initial positions)
void InitializeStrokeData(stroke_info* stroke, double startX, double startY);
// Returns the closest parametric value (float between 0 and 1) of curve based on given X and Y position
// float GetParametricOnStroke(stroke_info* stroke, double x, double y);
// Returns info on the at the given parametric
// void GetStrokeInfo(stroke_info* stroke, float parametric);

/*
CL Stroke Info - Listed in pipeline
int         Stroke ID
int         Number of points
cl_float2   position
cl_float2   direction
cl_float    thickness
cl_bool     shaders
*/