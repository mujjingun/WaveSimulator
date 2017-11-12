#version 430 core

layout (binding = 0, rgba32f) uniform image2D input_img;
layout (binding = 1, rgba32f) uniform image2D output_img;
uniform float time;
uniform float dt;
uniform float omega;

in float type;

layout (location = 0) out vec4 color;

// returns 1 if x is close to v
// 0 otherwise
float pick(float x, float v) {
    return exp(-pow((x - v) * 16, 2));
}

void main() {
    vec4 x = imageLoad(input_img, ivec2(gl_FragCoord.xy));
    vec4 cx = imageLoad(output_img, ivec2(gl_FragCoord.xy));
    cx *= .99;

    const float amp = 1;

    vec4 result = vec4(amp * sin(time * omega),
                       amp * omega * cos(time * omega),
                       amp * -pow(omega, 2) * sin(time * omega), 0) * pick(type, 1) // OSC
                + vec4(0, 0, 0, 0) * pick(type, 2) // Fixed wall
                + cx * step(3, type) // No reflection
                + vec4(1, 0, 0, 0) * step(4, type); // Pulse
    imageStore(output_img, ivec2(gl_FragCoord.xy), result);
}

