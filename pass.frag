#version 430 core

layout (binding = 0, rgba32f) uniform image2D input_img;
layout (binding = 1, rgba32f) uniform image2D output_img;
uniform float dt;

in float type;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 x = imageLoad(input_img, coords);

    vec4 dx1 = x-imageLoad(input_img, coords + ivec2(-1,  0));
    vec4 dx2 = x-imageLoad(input_img, coords + ivec2( 1,  0));
    vec4 dy1 = x-imageLoad(input_img, coords + ivec2( 0, -1));
    vec4 dy2 = x-imageLoad(input_img, coords + ivec2( 0,  1));
    vec4 d = dx1 + dx2 + dy1 + dy2;

    const float k = 1e7;
    const float m = 1;
    const float b = 1e-3;
    float a = -k / m * (d.r + d.g * dt + .5 * d.b * pow(dt, 2));
    float v = x.g + .5 * (x.b + a) * dt - b * x.g;
    float y = x.r + x.g * dt + .5 * x.b * dt * dt;

    imageStore(output_img, coords, vec4(y, v, a, 0));
    //imageAtomicExchange(spinlock_img, coords, 1);
}
