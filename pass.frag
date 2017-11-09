#version 430 core

layout (binding = 0, rgba32f) uniform image2D input_img;
layout (binding = 1, rgba32f) uniform image2D output_img;
uniform float time;
uniform float omega;

in float type;

layout (location = 0) out vec4 color;

float pick(float x, float v) {
    return exp(-((x - v) * 6) * ((x - v) * 6));
}

void main() {
    vec4 x = imageLoad(input_img, ivec2(gl_FragCoord.xy));
    vec4 d1 = x - imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2(-1,  0));
    vec4 d2 = x - imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2( 1,  0));
    vec4 d3 = x - imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2( 0, -1));
    vec4 d4 = x - imageLoad(input_img, ivec2(gl_FragCoord.xy) + ivec2( 0,  1));
    vec4 d = d1 + d2 + d3 + d4;
    const float k = 1e7;
    const float m = 1;
    const float dt = .0001;
    const float b = 1e-3;
    float a = -k / m * (d.r + d.g * dt + .5 * d.b * pow(dt, 2));
    a = (x.b + a) / 2;
    float v = -k / m * (d.r * dt + .5 * d.g * pow(dt, 2) + 1. / 6. * d.b * pow(dt, 3)) + x.g - b * x.g;
    float y = -k / m * (.5 * d.r * pow(dt, 2) + 1. / 6. * d.g * pow(dt, 3)) + v * dt + x.r;
    vec4 result = vec4(y, v, a, 0) * pick(type, 0)
                + vec4(sin(time * omega), omega * cos(time * omega), -pow(omega, 2) * sin(time * omega), 0) * pick(type, 1)
                + vec4(0, 0, 0, 0) * pick(type, 2);
    imageStore(output_img, ivec2(gl_FragCoord.xy), result);
    //imageStore(output_img, ivec2(gl_FragCoord.xy), x);
}
