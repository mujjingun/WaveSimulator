#version 430 core

layout (binding = 0, rgba32f) uniform image2D input_img;
layout (binding = 1, rgba32f) uniform image2D output_img;
in float type;

layout (location = 0) out vec4 color;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    vec4 cx = imageLoad(output_img, coords);
    const float b = 1e-3;
    cx *= 1 - pow((type - 3), 4) / 8 - b;
    imageStore(output_img, coords, cx);
}

