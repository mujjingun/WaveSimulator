#version 430 core

layout (binding = 0, rgba32f) uniform image2D output_img;

layout (location = 0) out vec4 color;

uniform ivec2 size;

in float type;

float pick(float x, float v) {
    return exp(-((x - v) * 6) * ((x - v) * 6));
}

void main() {
    float th = .15;
    ivec2 coords = ivec2((gl_FragCoord.xy));
    float col = imageLoad(output_img, coords).r;
    col = tanh(col * 10) / 2 + .5;
    color = vec4(vec3(col), 1) * pick(type, 0)
        + vec4(0.5, 0.8, 0.5, 1) * pick(type, 2)
        + mix(vec4(0.7, 0.7, 1, 0), vec4(0.7, 0.7, 1, 0.1), type - 3) * step(3, type) * step(-4, -type);
}
