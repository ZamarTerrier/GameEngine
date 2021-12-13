#version 450

layout(binding = 3) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec2 inOffset;
layout(location = 3) in vec2 inScale;


layout(location = 0) out vec4 outColor;


void main() {

    float color = texture(texSampler, (fragTexCoord + inOffset) * inScale).r;

    outColor = vec4(color);
}
