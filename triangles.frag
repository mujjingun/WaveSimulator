#version 430 core

layout (binding = 0, rgba32f) uniform image2D output_img;

layout (location = 0) out vec4 color;

in float type;

float pick(float x, float v) {
    return exp(-((x - v) * 6) * ((x - v) * 6));
}

void main() {
    float col = imageLoad(output_img, ivec2(gl_FragCoord.xy)).r;
    col = tanh(col * 10) / 2 + .5;
    //col = col / 2 + .5;
    color = vec4(vec3(col), 1) * pick(type, 0)
        + vec4(0.7, 1, 0.7, 1) * pick(type, 2)
        + vec4(0.7, 0.7, 1, 0.1) * pick(type, 3)
        + vec4(1, 0, 0, 1) * pick(type, 4);
}
