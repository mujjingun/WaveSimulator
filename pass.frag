#version 430 core

layout (binding = 0, rgba32f) uniform image2D input_img;
layout (binding = 1, rgba32f) uniform image2D output_img;
uniform float dt;

in float type;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 x = imageLoad(input_img, coords);

    vec4 x1 = imageLoad(input_img, coords + ivec2(-1,  0));
    vec4 x2 = imageLoad(input_img, coords + ivec2( 1,  0));
    vec4 y1 = imageLoad(input_img, coords + ivec2( 0, -1));
    vec4 y2 = imageLoad(input_img, coords + ivec2( 0,  1));
    vec4 d = 4 * x - (x1 + x2 + y1 + y2);

    const float k = 1e7;
    const float m = 1;
    const float b = 1e-3;
    float a = -k / m * (d.r + d.g * dt + .5 * d.b * pow(dt, 2));
    float v = x.g + .5 * (x.b + a) * dt - b * x.g;
    float y = x.r + x.g * dt + .5 * x.b * dt * dt;

    vec4 res = vec4(y, v, a, 0);
    res *= 1 - pow((type - 3), 4) / 8;

    imageStore(output_img, coords, res);
    //imageStore(output_img, coords, vec4(0, 0, 0, 0));
}
