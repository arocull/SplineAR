#include "src/Objects/Tools/brush.h"

Brush::Brush() {
    numSamples = 0;
    samples = (glm::vec2*) malloc(MaxInputSamples * sizeof(glm::vec2));
    clearSamples();
}
Brush::~Brush() {
    free(samples);
}


// Starts a new stroke and returns it
Stroke* Brush::startStroke(double x, double y) {
    clearSamples();

    stroke = new Stroke();
    stroke->points[0]->pos.x = x;
    stroke->points[0]->pos.y = y;
    stroke->points[0]->thickness = thickness;

    stroke->points[1]->pos.x = x;
    stroke->points[1]->pos.y = y;
    stroke->points[1]->thickness = thickness;


    printf("Stroke has begun %i\n", inStroke());

    return stroke;
}
void Brush::endStroke() {
    if (!stroke) return;

    stroke->shrinkwrap(); // Remove any excess spots we had
    if (storedPoint) {
        delete storedPoint;
        storedPoint = nullptr;
    }

    clearSamples();
}
void Brush::sampleStroke(double x, double y) {
    if (!inStroke() || numSamples + 1 >= MaxInputSamples) return; // Return if there is not enough space for further samples, or if the stroke does not exist
    numSamples++;
    
    samples[numSamples] = glm::vec2(x, y); // Add sample to list of samples
    
    // If this sample is the same as the previous one, ignore it and return
    if (samples[numSamples] == samples[numSamples-1]) {
        numSamples--;
        return;
    }

    int strokeLen = stroke->length();
    Point* lastPoint = stroke->points[strokeLen - 1];

    // If the stroke is longer than 2 points, detect auto-closing
    if (strokeLen > 2 && glm::length(samples[numSamples] - stroke->points[0]->pos) <= autocloseGap) {
        if (!stroke->closed) {
            storedPoint = lastPoint; // Store the current point away
            stroke->pullPoint(lastPoint); // Pop it from array so it does not appear in visual
        }
        stroke->closed = true;
        

    } else { // If the cursor pos moves further than autoclose distance, go ahead and disable closing again
        if (stroke->closed) { // Pull last point out of storage if we are unclosing the last stroke
            stroke->pushPoint(storedPoint);
            lastPoint = storedPoint; // Update last point to be our stored point
            strokeLen++; // Update the stroke length variable since we have a new point

            storedPoint = nullptr; // Set stored point to null pointer so it is not deallocated when stroke ends
        }
        stroke->closed = false;

        // If the distance between the last point and the current point is greater than the maximum point gap, create a new point to control
        if (glm::length(lastPoint->pos - stroke->points[strokeLen-2]->pos) > maxPointGap) {
            //printf("Adding new segment to stroke\n");
            stroke->pushPoint(samples[numSamples]);
            strokeLen = stroke->length(); // Don't simply ++ incase of allocation error inside vector (in which case length is unchanged)
        }

        // Set position to last point here
        stroke->points[strokeLen-1]->pos = samples[numSamples];
        // printf("Set stroke %i position to %f, %f\n", strokeLen-1, samples[numSamples].x, samples[numSamples].y);
    }
}


// Returns true if a stroke is still in progress
bool Brush::inStroke() {
    return !(stroke == nullptr);
}
// Returns the current stroke we're drawing to
Stroke* Brush::getActiveStroke() {
    return stroke;
}


// Forcibly finishes the current stroke and clears all given samples
void Brush::clearSamples() {
    for (int i = 0; i < MaxInputSamples; i++) {
        samples[i] = glm::vec2(0,0);
    }
    numSamples = 0;
    stroke = nullptr;
}