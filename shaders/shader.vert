#version 450

layout(binding = 0) uniform UniformBufferObject {
    vec2 pos;
} ubo;

layout(binding = 1) uniform ImgUniformParam {
    vec2 offset;
    vec2 scale;
} iup;


layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec2 outOffset;
layout(location = 3) out vec2 outScale;


void main() {
    gl_Position = vec4(ubo.pos + position, 0.0, 1.0);
    fragColor = color;
    fragTexCoord = inTexCoord;
    outOffset = iup.offset;
    outScale = iup.scale;
}
