#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out float depth;

layout( push_constant ) uniform constants {
    mat4 mvp;
} push;

void main() {
    gl_Position = push.mvp * vec4(inPosition, 1.0);
    fragColor = inColor;
    depth = gl_Position.z;
}