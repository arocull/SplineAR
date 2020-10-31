#pragma once

#include "src/config.h"

#include <cstdio>
#include <math.h>

#include <CL/cl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/* Stroke Info - C Structure
* 
* Retains information about a pen stroke
* Contains 2D/3D positions, directions, and curves
* Strokes should be treated like parametric equations (put in value from 0 to 1 for start / end respectively, get output position and dir)
*/
typedef struct stroke_info {
    int stroke_id;
    int numPoints;

    // Point Data
    glm::vec3* points;
    glm::vec3* leftHandles;     // Bezier curve handles--set to nullptrs
    glm::vec3* rightHandles;    // Bezier curve handles
    float* thickness;           // Read line thickness at given point
    // float* alpha;            // Read line alpha at given point
    
    // Shape info
    bool closed;        // If true, data from the last point will wrap back to the first point
    // bool* shaders;      // What shaders are enabled on this stroke?
    bool visible;       // Should the stroke be displayed?
} stroke_info;

// Fills out stroke data with default values up to X number points
void STROKE_BlankData(stroke_info* stroke, int numPoints, bool firstTime);
// Initializes a stroke object and fills out blank data
void STROKE_InitializeData(stroke_info* stroke, int numPoints);

// Dynamically allocates and/or resizes a stroke to contain more points
void STROKE_ScalePointData(stroke_info* stroke, int newNumPoints);
// Allocates all point data on the stroke to the given array size
void STROKE_AllocatePointData(stroke_info* stroke, int newNumPoints);
// Deallocates all point data on a given stroke
void STROKE_DeallocatePointData(stroke_info* stroke);
// Sets the given point index on a stroke to the given value (eliminates bezier handles)
void STROKE_SetPointData(stroke_info* stroke, int index, double x, double y, double z, float thickness);


// Returns the point info on the stroke at the given parametric
// Returns a vector4 - (x, y, z, thickness)
glm::vec4 STROKE_GetPoint(stroke_info* stroke, float parametric);

/*
CL Stroke Info - Listed in pipeline
int         Stroke ID
int         Number of points
cl_float2   position
cl_float2   direction
cl_float    thickness
cl_bool     shaders
*/