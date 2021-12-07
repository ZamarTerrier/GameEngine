#ifndef TOOLS_H
#define TOOLS_H

#include "stdinclude.h"

#include "resource.h"

VkCommandBuffer beginSingleTimeCommands();

void endSingleTimeCommands(VkCommandBuffer commandBuffer);

bool isComplete(QueueFamilyIndices self);

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

shader readFile(const char* filename);

VkShaderModule createShaderModule(shader shdr);

mat4 m4_look_at (vec3 from, vec3 to, vec3 up);

vec3 v3_add   (vec3 a, vec3 b);
vec3 v3_adds  (vec3 a, float s);
vec3 v3_sub   (vec3 a, vec3 b);
vec3 v3_subs  (vec3 a, float s);
vec3 v3_mul   (vec3 a, vec3 b);
vec3 v3_muls  (vec3 a, float s);
vec3 v3_div   (vec3 a, vec3 b);
vec3 v3_divs  (vec3 a, float s);
float  v3_length(vec3 v);
vec3 v3_norm  (vec3 v);
float  v3_dot   (vec3 a, vec3 b);
vec3 v3_proj  (vec3 v, vec3 onto);
vec3 v3_cross (vec3 a, vec3 b);
float  v3_angle_between(vec3 a, vec3 b);

mat4 mat4_f(
        float m00, float m10, float m20, float m30,
        float m01, float m11, float m21, float m31,
        float m02, float m12, float m22, float m32,
        float m03, float m13, float m23, float m33
        );

vec3 v3_norm(vec3 v);

vec3 v3_proj(vec3 v, vec3 onto);

vec3 v3_cross(vec3 a, vec3 b);

mat4 m4_perspective(float vertical_field_of_view_in_deg, float aspect_ratio, float near_view_distance, float far_view_distance);

#endif // TOOLS_H
