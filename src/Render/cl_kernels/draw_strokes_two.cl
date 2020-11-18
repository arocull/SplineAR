// Fills empty background and overlays strokes ontop of it
kernel void draw_strokes(
    write_only image2d_t texture, // Canvas texture
    global float* time,           // Current time
    global int* maxStrokes,
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

    float4 color = (float4)(0.5f, 0.5f, 0.5f, 0.0f); // Default background color

    for (int strokeIndex = 0, pointIndex = 0, lastPointIndex = 0; strokeIndex < (*maxStrokes); strokeIndex++) {
        if (stroke_points[strokeIndex] <= 0) continue; // If there are no points to draw, don't attempt to draw them

        for (; pointIndex < (lastPointIndex + stroke_points[strokeIndex]); pointIndex++) {
            float2 pos = position[pointIndex];
            float dist = sqrt(pown(pos.x - x, 2) + pown(pos.y - y, 2));
            // Test if point is within range
            if (dist <= thickness[pointIndex]) {
                color = (float4)(0.0f, 1.0f, 0.0f, 1.0f);
                break;
            }
        }
        lastPointIndex = pointIndex;
    }
    
    write_imagef(texture, (int2)(x, y), color);
}