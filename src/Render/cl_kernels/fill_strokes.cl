// Fills empty background and overlays strokes ontop of it
kernel void fill_strokes(
    write_only image2d_t texture,  // Canvas texture
    global float* time,           // Current time
    global int* maxStrokes,       // Maximum number of strokes that can be drawn (for indexing)
    global int* windowWidth,      // Width of viewport in pixels
    global int* windowHeight,     // Height of viewport in pixels
    global int* outlineIndices,   // Indices of stroke outline bounds--used for calculating fills
    global char* closed,          // Is the stroke closed?
    global char* filled          // Is the stroke filled?
) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (outlineIndices[x + y * (*windowWidth)] >= 0) return; // There is a stroke drawn here, do not manipulate it

    bool drawn = false;

    // Draw from top of stroke/layer stack downward
    for (int strokeIndex = *maxStrokes - 1; strokeIndex >= 0 && !drawn; strokeIndex--) {
        if (!filled[strokeIndex] || !closed[strokeIndex]) continue;
        int leftOccurrences = 0;
        int rightOccurrences = 0;
        int topOccurrences = 0;
        int botOccurrences = 0;
        bool hitSpacer = true;

        for (int checkLeft = 0; checkLeft < x; checkLeft++) {
            if (outlineIndices[checkLeft + y * (*windowWidth)] == strokeIndex && hitSpacer) {
                if (hitSpacer) {
                    hitSpacer = false;
                    leftOccurrences++;
                }
            } else hitSpacer = true;
        }
        hitSpacer = true;

        if (leftOccurrences > 0 && fmod((float) leftOccurrences, 2) == 1) { // If statement nesting prevent running multiple for-loops if conditions are not met anyways (optimization)
            for (int checkRight = *windowWidth - 1; checkRight > x; checkRight--) {
                if (outlineIndices[checkRight + y * (*windowWidth)] == strokeIndex) {
                    if (hitSpacer) {
                        hitSpacer = false;
                        rightOccurrences++;
                    }
                } else hitSpacer = true;
            }
            hitSpacer = true;

            if (rightOccurrences > 0 && fmod((float) rightOccurrences, 2) == 1) {
                for (int checkTop = *windowHeight - 1; checkTop > y; checkTop--) {
                    if (outlineIndices[x + checkTop * (*windowWidth)] == strokeIndex) {
                        if (hitSpacer) {
                            hitSpacer = false;
                            topOccurrences++;
                        }
                    } else hitSpacer = true;
                }

                if (topOccurrences > 0 && fmod((float) topOccurrences, 2) == 1) {
                    for (int checkBot = 0; checkBot < y; checkBot++) {
                        if (outlineIndices[x + checkBot * (*windowWidth)] == strokeIndex) {
                            if (hitSpacer) {
                                hitSpacer = false;
                                botOccurrences++;
                            }
                        } else hitSpacer = true;
                    }

                    if (botOccurrences > 0 && fmod((float) botOccurrences, 2) == 1) {
                        write_imagef(texture, (int2)(x, y), (float4)((float) strokeIndex / (*maxStrokes), 0.0f, 0.0f, 1.0f));
                        drawn = true;
                    }
                }
            }
        }
    }
}