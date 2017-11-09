#version 430 core

layout (location = 0) in vec4 vPosition;
out float type;

void main() {
    type = vPosition.z;
    gl_Position = vec4(vPosition.xy, 0, 1);
}
