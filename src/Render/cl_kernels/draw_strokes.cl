// Fills empty background and overlays strokes ontop of it
kernel void draw_strokes(
    write_only image2d_t texture, // Texture to write to
    global float* time,         // Current time
    global int* stroke_id,      // ID of the stroke--if negative, ignore
    global int* stroke_points,  // Number of points on the stroke
    global float2* positions,   // Positions of stroke points
    global float2* directions,  // Directions of stroke points
    global float* thickness     // Thickness of stroke at points
) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    float4 color = (float4)(0.5f, 0.5f, 0.5f, 0.0f); // Default background color
    bool drawnStroke = false;

    for (int i = 19; i >= 0 && !drawnStroke; i--) { // 20 is currently the maximum number of strokes; TODO: Pipe number from config into shader
        if (stroke_id[i] >= 0) {    // Test all valid strokes (draws strokes last in order)
            for (int a = 0; a < stroke_points[i]; a++) {
                int index = 5 * i + a;
                float2 pos = positions[index];
                float dist = sqrt(pown(pos.x - x, 2) + pown(pos.y - y, 2));

                // Test if point is within range
                if (dist <= thickness[index]) {
                    color = (float4)(0.0f, 1.0f, 0.0f, 1.0f);
                    drawnStroke = true;
                    break;
                }
            }
        }
    }
    
    write_imagef(texture, (int2)(x, y), color);
}