#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec2 pos;
} ubo;

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;


void main() {
    gl_Position = ubo.model * vec4(ubo.pos + position, 0.0, 1.0);
    fragColor = color;
    fragTexCoord = inTexCoord;
}
