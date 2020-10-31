#include "src/Objects/Art/stroke.h"

int strokesMade = 0;

void STROKE_BlankData(stroke_info* stroke, int numPoints, bool firstTime) {
    stroke->stroke_id = -1;
    stroke->numPoints = numPoints;
    stroke->visible = true;

    if (numPoints > 0) { 
        STROKE_ScalePointData(stroke, numPoints);
    }

    // Zero out data
    for (int i = 0; i < numPoints; i++) {
        STROKE_SetPointData(stroke, i, 0, 0, 0, 0);
    }
    
    // Allocate and mark all shaders as unused
    // if (firstTime) stroke->shaders = (bool*) malloc(NUM_SHADERS_CUSTOM * sizeof(bool));
    // for (int i = 0; i < NUM_SHADERS_CUSTOM; i++) {
    //     stroke->shaders[i] = false;
    // }
}

void STROKE_InitializeData(stroke_info* stroke, double startX, double startY, double startZ) {
    stroke->stroke_id = strokesMade;
    strokesMade++;

    STROKE_ScalePointData(stroke, 1);
    STROKE_SetPointData(stroke, 0, startX, startY, startZ, 20);
}

void STROKE_ScalePointData(stroke_info* stroke, int newNumPoints) {
    // Two different array sizes -- reallocate array, copy data from old into new, delete old
    if (stroke->numPoints > 0 && newNumPoints > 0 && stroke->numPoints != newNumPoints) {
        int oldPoints = stroke->numPoints;
        stroke->numPoints = newNumPoints;

        // Perform reallocations for all given arrays (automatically copies data over)
        glm::vec3* points = (glm::vec3*) realloc(stroke->points, newNumPoints * sizeof(glm::vec3));
        glm::vec3* leftHandles = (glm::vec3*) realloc(stroke->leftHandles, newNumPoints * sizeof(glm::vec3));
        glm::vec3* rightHandles = (glm::vec3*) realloc(stroke->rightHandles, newNumPoints * sizeof(glm::vec3));
        float* thickness = (float*) realloc(stroke->thickness, newNumPoints * sizeof(float));

        if (points && leftHandles && rightHandles && thickness) {
            // Fill remaining point data with empty values (if there are more points now than before)
            for (int i = oldPoints; i < newNumPoints; i++) {
                STROKE_SetPointData(stroke, i, 0, 0, 0, 0);
            }

            // Update memory addresses
            stroke->points = points;
            stroke->leftHandles = leftHandles;
            stroke->rightHandles = rightHandles;
            stroke->thickness = thickness;
        } else {
            printf("ERROR: Reallocation failed on stroke %i (attempted to move from size %i to %i)!\n", stroke->stroke_id, oldPoints, newNumPoints);
            stroke->numPoints = oldPoints;
        }

    } else if (stroke->numPoints == 0 && newNumPoints > 0) { // Stroke had no point data previously, build new arrays
        STROKE_AllocatePointData(stroke, newNumPoints);
    } else if (stroke->numPoints > 0 && newNumPoints == 0) { // Deallocate stroke data and leave as empty
        STROKE_DeallocatePointData(stroke);
    }
    // If none of these cases occur, do nothing
}
void STROKE_AllocatePointData(stroke_info* stroke, int newNumPoints) {
    stroke->numPoints = newNumPoints;
    stroke->points = (glm::vec3*) malloc(newNumPoints * sizeof(glm::vec3));
    stroke->leftHandles = (glm::vec3*) malloc(newNumPoints * sizeof(glm::vec3));
    stroke->rightHandles = (glm::vec3*) malloc(newNumPoints * sizeof(glm::vec3));
    stroke->thickness = (float*) malloc(newNumPoints * sizeof(float));    
}
void STROKE_DeallocatePointData(stroke_info* stroke) {
    stroke->numPoints = 0;
    free(stroke->points);
    free(stroke->leftHandles);
    free(stroke->rightHandles);
    free(stroke->thickness);
    stroke->points = nullptr;
    stroke->leftHandles = nullptr;
    stroke->rightHandles = nullptr;
    stroke->thickness = nullptr;
}
void STROKE_SetPointData(stroke_info* stroke, int index, double x, double y, double z, float thickness) {
    stroke->points[index] = glm::vec3(x, y, z);
    stroke->leftHandles[index] = glm::vec3(x, y, z);
    stroke->rightHandles[index] = glm::vec3(x, y, z);
    stroke->thickness[index] = thickness;
}


// float GetParametricOnStroke(stroke_info* stroke, double x, double y);
glm::vec4 STROKE_GetPoint(stroke_info* stroke, float parametric) {
    float t = parametric * (stroke->numPoints - (!stroke->closed)); // Count 1 less index if stroke is not closed (as we won't be performing wrapping)
    int index = (int) t; // Automatically rounds down to nearest integer
    int indexPlusOne = index + 1;
    if (stroke->closed && indexPlusOne >= stroke->numPoints) { indexPlusOne = 0; } // Wrap to bottom

    printf("Parametric %f, index %i, indexPlusOne%i\n", t, index, indexPlusOne);
    // TODO: Utilize stroke handles if points[index] != leftHandle[index]

    // Do basic lerp between points
    // TODO: Benchmark allocation of a single vec3 position and pipe XYZ into the vec4 rather than independently calculate XYZ
    return glm::vec4(
        (1 - t) * stroke->points[index].x + t * stroke->points[indexPlusOne].x,  // X
        (1 - t) * stroke->points[index].y + t * stroke->points[indexPlusOne].y,  // Y
        (1 - t) * stroke->points[index].z + t * stroke->points[indexPlusOne].z,  // Z
        (1 - t) * stroke->thickness[index] + t * stroke->thickness[indexPlusOne] // Thickness
    );
}