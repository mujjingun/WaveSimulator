#version 430 core

layout (binding = 0, rgba32f) uniform image2D input_img;
layout (binding = 1, rgba32f) uniform image2D output_img;
uniform float dt;

in float type;

void main() {
    vec4 x = imageLoad(input_img, ivec2(gl_FragCoord.xy));

    vec4 d1 = x-imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2(-1,  0));
    vec4 d2 = x-imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2( 1,  0));
    vec4 d3 = x-imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2( 0, -1));
    vec4 d4 = x-imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2( 0,  1));
    vec4 d = d1 + d2 + d3 + d4;

    const float k = 1e7;
    const float m = 1;
    const float b = 1e-3;
    float a = -k / m * (d.r + d.g * dt + .5 * d.b * pow(dt, 2));
    float v = x.g + .5 * (x.b + a) * dt - b * x.g;
    float y = x.r + x.g * dt + .5 * x.b * dt * dt;

    imageStore(output_img, ivec2(gl_FragCoord.xy), vec4(y, v, a, 0));
}
