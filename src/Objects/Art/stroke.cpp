#include "src/Objects/Art/stroke.h"

int strokesMade = 0;

void BlankStrokeData(stroke_info* stroke, bool doAlloc) {
    stroke->stroke_id = -1;
    stroke->numPoints = 0;
    stroke->visible = true;

    if (doAlloc) { // Uses malloc as it is faster and data is zero'd out after
        stroke->points = (glm::vec2*) malloc(MAX_STROKE_POINTS * sizeof(glm::vec2));
        stroke->dir = (glm::vec2*) malloc(MAX_STROKE_POINTS * sizeof(glm::vec2));
        stroke->thickness = (float*) malloc(MAX_STROKE_POINTS * sizeof(float));
        stroke->alpha = (float*) malloc(MAX_STROKE_POINTS * sizeof(float));
        stroke->shaders = (bool*) malloc(NUM_SHADERS_CUSTOM * sizeof(bool));
    }

    // Zero out data
    for (int i = 0; i < MAX_STROKE_POINTS; i++) {
        stroke->points[i] = glm::vec2(0, 0);
        stroke->dir[i] = glm::vec2(0, 0);
        stroke->thickness[i] = 0.0f;
        stroke->alpha[i] = 1.0f;
    }
    // Mark all shaders as unused
    for (int i = 0; i < NUM_SHADERS_CUSTOM; i++) {
        stroke->shaders[i] = false;
    }
}

void InitializeStrokeData(stroke_info* stroke, double startX, double startY) {
    stroke->stroke_id = strokesMade;
    strokesMade++;

    stroke->numPoints = 1;
    stroke->points[0] = glm::vec2(startX, startY);
    // stroke->dir[0] = glm
    stroke->thickness[0] = 20.0f;
    // stroke->alpha[0] = 1.0f;
}


// float GetParametricOnStroke(stroke_info* stroke, double x, double y);
// void GetStrokeInfo(stroke_info* stroke, float parametric);