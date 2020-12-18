#pragma once

#include "src/config.h"

#include <cstdio>
#include <vector>
#include <math.h>

#include <CL/cl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "src/Objects/Art/point.h"

/* Stroke Info - C Structure
* 
* Retains information about a pen stroke
* Contains 2D/3D positions, directions, and curves
* Strokes should be treated like parametric equations (put in value from 0 to 1 for start / end respectively, get output position and dir)
*/
class Stroke {
    public:
        Stroke();
        ~Stroke();

        // Point Data
        std::vector<Point*> points;
        
        // Shape info
        bool closed = false;        // If true, data from the last point will wrap back to the first point
        bool filled = true;         // If true, the stroke should be filled (if it is closed)
        bool visible = true;        // Should the stroke be displayed?

    private:

    public:
        void pushPoint(Point* point);
        void pushPoint(glm::vec2 newPoint);
        void pullPoint(Point* point);

        int length();

        // Returns the point info on the stroke at the given parametric
        // Returns a newly allocated Point* (note: must be deleted after use)
        Point* getPoint(float parametric);
};