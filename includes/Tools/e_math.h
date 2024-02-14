#ifndef E_MATH_H
#define E_MATH_H

#include "Variabels/engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define e_min( a, b ) ((a) < (b) ? (a) : (b))
#define e_max( a, b ) ((a) > (b) ? (a) : (b))

#define INTR_EPS 1E-6

extern vec3 e_vec3_origin;

extern mat4 edenMat;

float lerp(float a, float b, float t);
float clamp(float a, float d1, float d2);
int get_sig(float val);

mat3 rotateX(float theta);
// Rotation matrix around the Y axis.
mat3 rotateY(float theta);
// Rotation matrix around the Z axis.
mat3 rotateZ(float theta);

mat3 m3_scale_matrix(vec2 scale );
mat3 m3_rotation_matrix(vec3 rotation);
mat3 m3_translation_matrix(mat3 matrix, vec2 pos);
mat3 m3_mult(mat3 m1, mat3 m2);
vec3 m3_v3_mult(mat3 m, vec3 v);

vec2 vec2_f(float x, float y);

float v2_cross(vec2 a, vec2 b);
vec2 v2_add(vec2 a, vec2 b);
vec2 v2_adds  (vec2 a, float s);
vec2 v2_sub   (vec2 a, vec2 b);
vec2 v2_subs  (vec2 a, float s);
vec2 v2_mul   (vec2 a, vec2 b);
vec2 v2_muls  (vec2 a, float s);
vec2 v2_div   (vec2 a, vec2 b);
vec2 v2_divs  (vec2 a, float s);
vec2 v2_norm(vec2 v);
float v2_length(vec2 v);
float v2_dot (vec2 a, vec2 b);
float  v2_distance(vec2 v1, vec2 v2);

vec3 vec3_f(float x, float y, float z);

float v3_maxs(vec3 a);
float v3_mins(vec3 a);
vec3 v3_abs(vec3 a);

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
vec3 v3_to(vec3 from, vec3 to, float t);
float v3_point_segment_dist(const vec3 *P, const vec3 *x0, const vec3 *b, vec3 *witness);
float v3_point_tri_dist(const vec3 *P, const vec3 *x0, const vec3 *B, const vec3 *C, vec3 *witness);

vec4 vec4_f(float x, float y, float z, float w);

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

mat3 mat3_f();

vec3 m4_v3_mult(mat4 m, vec3 v);
vec4 m4_v4_mult(mat4 mat, vec4 v);

mat4 m4_rotate(mat4 mat, float angle, vec3 axis);
mat4 m4_rotated(mat4 mat, float angle, vec3 axis) ;
mat4 m4_rotate_make(mat4 mat, float angle, vec3 axis);
mat4 m4_mul_rot(mat4 mat1, mat4 mat2) ;
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
mat4 m4_ortho(float top, float bottom, float left, float right, float zFar, float zNear);
mat4 m4_frustum(float left, float right, float bottom, float top, float nearZ, float farZ) ;
mat4 m4_perspective(uint32_t width, uint32_t height, float vertical_field_of_view_in_deg, float near_view_distance, float far_view_distance);
mat4 mat4_colmnsf(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33);
mat4 mat4_rowsf(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33);
mat4 m4_inv(mat4 mat);

#ifdef __cplusplus
}
#endif

#endif // E_MATH_H
