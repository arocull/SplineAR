// UV Coordinate on scale of 0 to 1 that is clamped at the edges
constant sampler_t uv = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
// Actual pixel coordinates for writing to the image
constant sampler_t coords = CLK_NORMALIZED_COORDS_false | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

kernel void test(write_only image2d_t bmp) {
    // int2 coord = (int2)(get_global_id(0) / 2, get_global_id(1) / 2);

    write_imagef(bmp, coords, val);
}