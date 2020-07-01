// Fill texture with single color
kernel void time_test(write_only image2d_t texture, global float* time) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    float a = (sin(x / 3 + cos(*time) * 10) + 1.0f) / 7;
    float4 color = (float4)(a, a, a / 2, 1.0f);
    
    write_imagef(texture, (int2)(x, y), color);
}