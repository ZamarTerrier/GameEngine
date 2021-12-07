#version 450

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D texSampler2;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
float _Rotation = 45.0f;

void main() {

    vec4 color = texture(texSampler, fragTexCoord);

    float sinX = sin ( _Rotation );
    float cosX = cos ( _Rotation );
    float sinY = sin ( _Rotation );
    mat2 rotationMatrix = mat2( cosX, -sinX, sinY, cosX);

    vec4 color2 = texture(texSampler2, (fragTexCoord + vec2(0,-0.2)) * rotationMatrix);
    vec4 color3 = color + color2;

    if(color3.a < 0.1)
        discard;

    outColor = vec4(fragColor * color3.rgb, 1.0);
}
