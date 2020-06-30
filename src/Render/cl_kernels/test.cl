// Fill texture with single color
__kernel void test(write_only image2d_t bmp) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    float4 color = (float4)(x / 600.0f, y / 400.0f, 1.0f, 1.0f);
    
    write_imagef(bmp, (int2)(x, y), color);
}