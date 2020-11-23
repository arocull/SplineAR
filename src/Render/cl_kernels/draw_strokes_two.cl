// Fills empty background and overlays strokes ontop of it
kernel void draw_strokes(
    write_only image2d_t texture, // Canvas texture
    global float* time,           // Current time
    global int* maxStrokes,       // Maximum number of strokes that can be drawn (for indexing)
    global int* windowWidth,      // Width of viewport in pixels
    global int* windowHeight,     // Height of viewport in pixels
    global int* stroke_points,    // Number of points on the stroke
    global bool* closed,          // Is the stroke closed?
    global bool* filled,          // Is the stroke filled?
    global float2* position,      // Positions of stroke points
    global float2* leftHandle,    // Left handle of stroke point
    global float2* rightHandle,   // Right handle of stroke points
    global float* thickness       // Thickness of stroke at points
) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    float4 color = (float4)(0.5f, 0.5f, 0.5f, 1.0f); // Default background color

    for (int strokeIndex = 0, pointIndex = 0, lastPointIndex = 0; strokeIndex < (*maxStrokes); strokeIndex++) {
        if (stroke_points[strokeIndex] <= 0) continue; // If there are no points to draw, don't attempt to draw them

        int maxIndex = lastPointIndex + stroke_points[strokeIndex];

        for (; pointIndex <= maxIndex; pointIndex++) {
            int nextIndex = pointIndex + 1;
            if (nextIndex == maxIndex) {
                if (closed[strokeIndex]) { // Accept the max index if it is a closed shape
                    nextIndex = lastPointIndex; // Set next index to first point
                } else continue; // Otherwise skip over it and continue
            }

            // Approximate the parametric position
            float tx = (float) (x - position[pointIndex].x * (*windowWidth)) / (float) (position[nextIndex].x * (*windowWidth) - position[pointIndex].x * (*windowWidth));
            float ty = (float) (y - position[pointIndex].y * (*windowHeight)) / (float) (position[nextIndex].y * (*windowHeight) - position[pointIndex].y * (*windowHeight));


            float t = (tx + ty) / 2.0f; // Estimated T position
            float tThickness = ((1.0f - t)*thickness[pointIndex] + t*thickness[nextIndex]) / 10.0f; // Line thickness at this T value

            // Make sure X and Y parametric calculations are not too far from each other
            // Clip anything that is not close to the lines, though allow stuff that is probably in range of thickness
            if (fabs(tx - ty) < 0.1f) { // thickness[pointIndex]
                t = clamp(t, 0.0f, 1.0f); // Clamp T so we do not draw anything past the edges of the line
                float dist = distance( // If they passed the initial test, do a more accurate look
                    (float2)((float) x, (float) y),
                    ((1.0f - t) * position[pointIndex] + t * position[nextIndex]) * (float2)((float) *windowWidth, (float) *windowHeight)
                );
                
                if (dist < 5) {
                    color = (float4)(dist / tThickness, 0, 0.0f, 1.0f);
                    break; // Don't try to draw any more points on this line
                }
            }
        }
        lastPointIndex = maxIndex; // We shouldn't iterate through any points of the previous stroke now
    }
    
    write_imagef(texture, (int2)(x, y), color);
}