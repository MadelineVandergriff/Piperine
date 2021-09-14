#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in float depth;

layout(binding = 0) uniform UBO {
    vec3 fogColor;
} ubo;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(mix(fragColor, ubo.fogColor, clamp(pow(depth, 3) / 40.0, 0.0, 1.0)), 1.0);
    //outColor = vec4(ubo.fogColor, 1.0);
}