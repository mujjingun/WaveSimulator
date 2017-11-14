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
    ivec2 coords = ivec2(gl_FragCoord.xy);

    const float amp = 1;
    vec4 result = vec4(amp * sin(time * omega),
                       amp * omega * cos(time * omega),
                       amp * -pow(omega, 2) * sin(time * omega), 0) * pick(type, 1) // OSC
                + vec4(0, 0, 0, 1) * pick(type, 2) // Fixed wall
                + vec4(1, 0, 0, 0) * pick(type, 10); // Pulse

    imageStore(output_img, coords, result);
}

