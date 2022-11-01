#ifndef E_MATH_H
#define E_MATH_H

#include "stdinclude.h"

#define min( a, b ) ((a) < (b) ? (a) : (b))
#define max( a, b ) ((a) > (b) ? (a) : (b))

extern mat4 edenMat;

float lerp(float a, float b, float t);

float clamp(float a, float d1, float d2);

mat3 rotateX(float theta);
// Rotation matrix around the Y axis.
mat3 rotateY(float theta);
// Rotation matrix around the Z axis.
mat3 rotateZ(float theta);

mat3 m3_scale_matrix(vec2 scale );
mat3 m3_rotation_matrix(float degrees);
mat3 m3_translation_matrix(mat3 matrix, vec2 pos);
mat3 m3_mult(mat3 m1, mat3 m2);
vec3 m3_v3_mult(mat3 m, vec3 v);

vec2 v2_add(vec2 a, vec2 b);
vec2 v2_adds  (vec2 a, float s);
vec2 v2_sub   (vec2 a, vec2 b);
vec2 v2_subs  (vec2 a, float s);
vec2 v2_mul   (vec2 a, vec2 b);
vec2 v2_muls  (vec2 a, float s);
vec2 v2_div   (vec2 a, vec2 b);
vec2 v2_divs  (vec2 a, float s);
float v2_length(vec2 v);
vec2 v2_norm(vec2 v);

float v3_maxs(vec3 a);
float v3_mins(vec3 a);
vec3 v3_min(vec3 a, vec3 b);
vec3 v3_max(vec3 a, vec3 b);
vec3 v3_add   (vec3 a, vec3 b);
vec3 v3_adds  (vec3 a, float s);
vec3 v3_sub   (vec3 a, vec3 b);
vec3 v3_subs  (vec3 a, float s);
vec3 v3_mul   (vec3 a, vec3 b);
vec3 v3_muls  (vec3 a, float s);
vec3 v3_div   (vec3 a, vec3 b);
vec3 v3_divs  (vec3 a, float s);
float  v3_distance(vec3 v1, vec3 v2);
float  v3_length(vec3 v);
vec3 v3_norm  (vec3 v);
float  v3_dot   (vec3 a, vec3 b);
vec3 v3_proj  (vec3 v, vec3 onto);
vec3 v3_cross (vec3 a, vec3 b);
float  v3_angle_between(vec3 a, vec3 b);
bool v3_equal(vec3 a, vec3 b);
vec3 v3_lerp(vec3 a, vec3 b, float t);
vec3 v3_slerp(vec3 start, vec3 end, float percent);

vec4  v4_add(vec4 a, vec4 b) ;
vec4  v4_sub(vec4 a, vec4 b) ;
vec4  v4_mul (vec4 a, vec4 b);
vec4  v4_muls (vec4 a, float s);
vec4  v4_mad(vec4 a, vec4 b, float c);
vec4  v4_div(vec4 a, float b);
vec4  v4_neg(vec4 a) ;
vec4  v4_inverse(vec4 a) ;
vec4  v4_inverse_normalized(vec4 a);
float v4_dot(vec4 a, vec4 b);
float v4_length(vec4 a);
vec4  v4_normalize(vec4 a);
bool v4_equal(vec4 a, vec4 b) ;
vec4 v4_lerp(vec4 a, vec4 b, float t);


vec3 m4_v3_mult(mat4 m, vec3 v);

mat4 mat4_mult_transform(mat4 m1, mat4 m2);
mat4 m4_transform_quaternion(vec3 translation, vec3 scale, vec4 rotation);
mat4 m4_transform(vec3 pos, vec3 scale, vec3 axis);
mat4 m4_translate(vec3 pos);
mat4 m4_translate_mat_add(mat4 mat, vec3 pos);
mat4 m4_translate_mat(mat4 mat, vec3 pos);
mat4 m4_m4_rotation_matrix(mat4 mat, vec3 degrees);
mat4 m4_rotation_mat_quternion(mat4 m1, vec4 quaternion);
mat4 m4_rotation_matrix(vec3 degrees);
mat4 m4_rotation_quaternion(vec4 quaternion);
mat4 m4_look_at (vec3 from, vec3 to, vec3 up);

mat4 m4_scale_mat(vec3 scale);
mat4 m4_scale(mat4 mat, vec3 scale);
mat4 m4_add(mat4 m1, mat4 m2);
mat4 m4_mult(mat4 m1, mat4 m2);
mat4 m4_perspective(float vertical_field_of_view_in_deg, float near_view_distance, float far_view_distance);
mat4 mat4_colmnsf(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33);
mat4 mat4_rowsf(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33);


#endif // E_MATH_H
