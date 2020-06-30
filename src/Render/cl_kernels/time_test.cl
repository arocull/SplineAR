// Fill texture with single color
kernel void time_test(write_only image2d_t texture, global float* time) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    float4 color = (float4)(x / 1000.0f, y / 800.0f, (sin(*time) + 1.0f)/2, 0.5f);
    
    write_imagef(texture, (int2)(x, y), color);
}