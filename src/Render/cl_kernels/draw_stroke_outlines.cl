// Draws stroke outlines and records boundaries of the strokes
kernel void draw_stroke_outlines(
    write_only image2d_t texture, // Canvas texture, note images can only be read OR write in any given kernel, not both
    global float* time,           // Current time
    global int* maxStrokes,       // Maximum number of strokes that can be drawn (for indexing)
    global int* maxPoints,        // Number of points currently in use
    global int* windowWidth,      // Width of viewport in pixels
    global int* windowHeight,     // Height of viewport in pixels
    global int* outlineIndices,   // Indices of stroke outline bounds--used for calculating fills
    global int* stroke_points,    // Number of points in the stroke
    global char* closed,          // Is the stroke closed?
    global char* filled,          // Is the stroke filled?
    global float2* position,      // Positions of stroke points
    global float2* leftHandle,    // Left handle of stroke point
    global float2* rightHandle,   // Right handle of stroke points
    global float* thickness       // Thickness of stroke at points
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    float2 pos = (float2)((float) x, (float) y);

    float4 color = (float4)(0.5f, 0.5f, 0.5f, 1.0f); // Default background color

    bool drawn = false; // Set to true once this pixel is fully opaque

    outlineIndices[x + y * (*windowWidth)] = -1; // Blank out fill index

    // Start from front and draw to back
    for (int strokeIndex = *maxStrokes - 1, lastPointIndex = *maxPoints - 1, pointIndex = *maxPoints - 1; strokeIndex >= 0 && !drawn; strokeIndex--) {
        if (stroke_points[strokeIndex] <= 0) continue; // If there are no points to draw, don't attempt to draw them

        int minIndex = lastPointIndex - stroke_points[strokeIndex];

        for (; pointIndex > minIndex && !drawn; pointIndex--) {
            int nextIndex = pointIndex - 1;
            if (nextIndex <= minIndex) {
                if (closed[strokeIndex] != 0) { // Accept the max index if it is a closed shape
                    nextIndex = lastPointIndex; // Set next index to first point
                } else continue; // Otherwise skip over it and continue
            }

            // Approximate the parametric position on both X and Y axis
            float tx = (float) (x - position[pointIndex].x) / (float) (position[nextIndex].x - position[pointIndex].x);
            float ty = (float) (y - position[pointIndex].y) / (float) (position[nextIndex].y - position[pointIndex].y);

            // "Why have two T values?"
            // Using one T-value from our two estimated T values causes squishing along the X or Y axis
            // This is because change in X or Y is minimized in vertical or horizontal directions respectively
            // To alleviate this, we simply treat both estimates as equally-pluasible options
            // We then clamp them, calculate appropiate line thicknesses, and test their calculated position independently
            // If either point falls within the line thickness, it is a valid distance
            // If we need a single parametric however (i.e. stroke-based shaders), we will simply use the closest T value

            tx = clamp(tx, 0.0f, 1.0f); // Clamp T so we do not draw anything past the edges of the line
            ty = clamp(ty, 0.0f, 1.0f);

            float txThickness = ((1.0f - tx)*thickness[pointIndex] + tx*thickness[nextIndex]) / 2.0f; // Line thickness at this T value
            float tyThickness = ((1.0f - ty)*thickness[pointIndex] + ty*thickness[nextIndex]) / 2.0f;


            float distX = distance( // Calculate distance for thickness checks
                pos,
                ((1.0f - tx) * position[pointIndex] + tx * position[nextIndex])
            );
            float distY = distance(
                pos,
                ((1.0f - ty) * position[pointIndex] + ty * position[nextIndex])
            );
            
            if (distX < txThickness || distY < tyThickness) {
                // Pick real parametric
                float dist = min(distX, distY);
                float t = 0.0f;
                if (dist == distX) {
                    t = tx;
                } else {
                    t = ty;
                }

                color = (float4)((float) closed[strokeIndex], 0, t, 1.0f);
                drawn = true; // Pixel is fully opaque, no further calculations needed
                outlineIndices[x + y * (*windowWidth)] = strokeIndex; // Set fill index
            }
        }

        lastPointIndex -= stroke_points[strokeIndex]; // We shouldn't iterate through any points of the previous stroke now
    }
    
    write_imagef(texture, (int2)(x, y), color);
}