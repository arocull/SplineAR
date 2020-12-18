// Fills empty background and overlays strokes ontop of it
kernel void fill_strokes(
    write_only image2d_t canvas,  // Canvas texture
    global float* time,           // Current time
    global int* maxStrokes,       // Maximum number of strokes that can be drawn (for indexing)
    global int* windowWidth,      // Width of viewport in pixels
    global int* windowHeight,     // Height of viewport in pixels
    global int* outlineIndices,   // Indices of stroke outline bounds--used for calculating fills
    global char* closed,          // Is the stroke closed?
    global char* filled,          // Is the stroke filled?
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    float2 pos = (float2)((float) x, (float) y);

    float4 color = (float4)(0.5f, 0.5f, 0.5f, 1.0f); // Default background color

    if (outlineIndices[x + y * (*windowWidth)] > -1) color = (float4)(1.0f, 0.0f, 0.0f, 1.0f);
    
    write_imagef(canvas, (int2)(x, y), color);
}