// Draw screen UV over texture
__kernel void screen_uv(write_only image2d_t bmp) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    float4 color = (float4)(x / 1000.0f, y / 800.0f, 1.0f, 1.0f);
    
    write_imagef(bmp, (int2)(x, y), color);
}