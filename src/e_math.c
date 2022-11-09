#include "e_math.h"

#include "math.h"

#include "e_resource.h"

mat4 edenMat= {1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 1, 0,
               0, 0, 0, 1};

float lerp(float a, float b, float t) { return a*(1.0f-t) + b*t; }

float clamp(float a, float d1, float d2){

  float res = min(max(a, d1), d2);

  return res;
}

mat3 rotateX(float theta) {
    theta *= M_PI / 180;  // convert to radians
    mat3 result;

    float c = cos(theta);
    float s = sin(theta);

    result.m[0][0] = 1;
    result.m[1][0] = 0;
    result.m[2][0] = 0;

    result.m[0][1] = 0;
    result.m[1][1] = c;
    result.m[2][1] = s;

    result.m[0][2] = 0;
    result.m[1][2] = -s;
    result.m[2][2] = c;

    return result;
}

// Rotation matrix around the Y axis.
mat3 rotateY(float theta) {
    theta *= M_PI / 180;  // convert to radians
    mat3 result;

    float c = cos(theta);
    float s = sin(theta);

    result.m[0][0] = c;
    result.m[1][0] = 0;
    result.m[2][0] = -s;

    result.m[0][1] = 0;
    result.m[1][1] = 1;
    result.m[2][1] = 0;

    result.m[0][2] = s;
    result.m[1][2] = 0;
    result.m[2][2] = c;

    return result;
}

// Rotation matrix around the Z axis.
mat3 rotateZ(float theta) {
    theta *= M_PI / 180;  // convert to radians
    mat3 result;

    float c = cos(theta);
    float s = sin(theta);

    result.m[0][0] = c;
    result.m[1][0] = s;
    result.m[2][0] = 0;

    result.m[0][1] = -s;
    result.m[1][1] = c;
    result.m[2][1] = 0;

    result.m[0][2] = 0;
    result.m[1][2] = 0;
    result.m[2][2] = 1;

    return result;
}

mat3 m3_mult(mat3 m1, mat3 m2){
    mat3 result;

    result.m[0][0] = (m1.m[0][0] * m2.m[0][0]) + (m1.m[0][1] * m2.m[1][0]) + (m1.m[0][2] * m2.m[2][0]);
    result.m[1][0] = (m1.m[0][0] * m2.m[0][1]) + (m1.m[0][1] * m2.m[1][1]) + (m1.m[0][2] * m2.m[2][1]);
    result.m[2][0] = (m1.m[0][0] * m2.m[0][2]) + (m1.m[0][1] * m2.m[1][2]) + (m1.m[0][2] * m2.m[2][2]);

    result.m[0][1] = (m1.m[1][0] * m2.m[0][0]) + (m1.m[1][1] * m2.m[1][0]) + (m1.m[1][2] * m2.m[2][0]);
    result.m[1][1] = (m1.m[1][0] * m2.m[0][1]) + (m1.m[1][1] * m2.m[1][1]) + (m1.m[1][2] * m2.m[2][1]);
    result.m[2][1] = (m1.m[1][0] * m2.m[0][2]) + (m1.m[1][1] * m2.m[1][2]) + (m1.m[1][2] * m2.m[2][2]);

    result.m[0][2] = (m1.m[2][0] * m2.m[2][0]) + (m1.m[2][1] * m2.m[1][0]) + (m1.m[2][2] * m2.m[2][0]);
    result.m[1][2] = (m1.m[2][0] * m2.m[0][1]) + (m1.m[2][1] * m2.m[1][1]) + (m1.m[2][2] * m2.m[2][1]);
    result.m[2][2] = (m1.m[2][0] * m2.m[0][2]) + (m1.m[2][1] * m2.m[1][2]) + (m1.m[2][2] * m2.m[2][2]);

    return result;
}

vec3 m3_v3_mult(mat3 m, vec3 v) {
    vec3 ret;
    float *pvec = &ret;
    for (int i = 0; i < 3; i++) {
        pvec[i] = v.x * m.m[0][i] + v.y * m.m[1][i] + v.z * m.m[2][i] + m.m[3][i];
    }
    return ret;
}

mat3 m3_scale_matrix(vec2 scale ){

    mat3 scale_mat;

    memset(&scale_mat, 0, sizeof(mat3));

    scale_mat.m[0][0] = scale.x;

    scale_mat.m[1][1] =  scale.y;

    scale_mat.m[2][2] = 1;

    return scale_mat;
}

mat3 m3_rotation_matrix(float degrees){
    mat3 r = rotateX(degrees);
    return r;
}

mat3 m3_translation_matrix(mat3 matrix, vec2 pos){

    mat3 result = matrix;

    result.m[2][0] = matrix.m[0][2] + pos.x;
    result.m[2][1] = matrix.m[1][2] + pos.y;

    return result;

}


vec2 v2_norm(vec2 v)
{
   float len = v2_length(v);
   if (len > 0)
       return (vec2){ v.x / len, v.y / len};
   else
       return (vec2){ 0, 0, 0};
}

vec3 v3_norm(vec3 v)
{
   float len = v3_length(v);
   if (len > 0)
       return (vec3){ v.x / len, v.y / len, v.z / len };
   else
       return (vec3){ 0, 0, 0};
}

vec3 v3_proj(vec3 v, vec3 onto)
{
   return v3_muls(onto, v3_dot(v, onto) / v3_dot(onto, onto));
}

vec3 v3_cross(vec3 a, vec3 b)
{
   return (vec3){
       a.y * b.z - a.z * b.y,
       a.z * b.x - a.x * b.z,
       a.x * b.y - a.y * b.x
   };

}

float v3_maxs(vec3 a){ return max(max(a.x, a.y), a.z); }
float v3_mins(vec3 a){ return min(min(a.x, a.y), a.z); }



vec3 v3_min(vec3 a, vec3 b){ return (vec3){ min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)}; }
vec3 v3_max(vec3 a, vec3 b){ return (vec3){ max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) }; }

float v2_length(vec2 v) { return sqrtf(v.x*v.x + v.y*v.y); }
vec2 v2_add(vec2 a, vec2 b) { return (vec2){ a.x + b.x, a.y + b.y}; }
vec2 v2_adds  (vec2 a, float s) { return (vec2){ a.x + s,   a.y + s }; }
vec2 v2_sub   (vec2 a, vec2 b) { return (vec2){ a.x - b.x, a.y - b.y }; }
vec2 v2_subs  (vec2 a, float s) { return (vec2){ a.x - s,   a.y - s }; }
vec2 v2_mul   (vec2 a, vec2 b) { return (vec2){ a.x * b.x, a.y * b.y }; }
vec2 v2_muls  (vec2 a, float s) { return (vec2){ a.x * s,   a.y * s  }; }
vec2 v2_div   (vec2 a, vec2 b) { return (vec2){ a.x / b.x, a.y / b.y }; }
vec2 v2_divs  (vec2 a, float s) { return (vec2){ a.x / s,   a.y / s  }; }

vec3 v3_add(vec3 a, vec3 b) { return (vec3){ a.x + b.x, a.y + b.y, a.z + b.z }; }
vec3 v3_adds  (vec3 a, float s) { return (vec3){ a.x + s,   a.y + s,   a.z + s   }; }
vec3 v3_sub   (vec3 a, vec3 b) { return (vec3){ a.x - b.x, a.y - b.y, a.z - b.z }; }
vec3 v3_subs  (vec3 a, float s) { return (vec3){ a.x - s,   a.y - s,   a.z - s   }; }
vec3 v3_mul   (vec3 a, vec3 b) { return (vec3){ a.x * b.x, a.y * b.y, a.z * b.z }; }
vec3 v3_muls  (vec3 a, float s) { return (vec3){ a.x * s,   a.y * s,   a.z * s   }; }
vec3 v3_div   (vec3 a, vec3 b) { return (vec3){ a.x / b.x, a.y / b.y, a.z / b.z }; }
vec3 v3_divs  (vec3 a, float s) { return (vec3){ a.x / s,   a.y / s,   a.z / s   }; }
float  v3_length(vec3 v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
float  v3_distance(vec3 v1, vec3 v2) { return sqrt(pow((v1.x - v2.x), 2) + pow((v1.y - v2.y), 2) + pow((v1.z - v2.z), 2)); }
float  v3_dot   (vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
bool v3_equal(vec3 a, vec3 b) { return (a.x == b.x) & (a.y == b.y) & (a.z == b.z); }
vec3 v3_lerp(vec3 a, vec3 b, float t) { return (vec3){lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t)}; }

vec3 v3_slerp(vec3 start, vec3 end, float percent)
{
  float dot = v3_dot(start, end);

  float result = clamp(dot, -1, 1);

  float theta = acos(result)*percent;

  vec3 RelativeVec = v3_sub(end, v3_muls(start, dot));

  RelativeVec = v3_norm(RelativeVec);
  // Orthonormal basis
  // The final result.
  return v3_add(v3_muls(start, cos(theta)), v3_muls(RelativeVec, sin(theta)));
}

vec4  v4_add(vec4 a, vec4 b) { return (vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
vec4  v4_sub(vec4 a, vec4 b) { return (vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }
vec4  v4_mul  (vec4 a, vec4 b) { return (vec4){ a.x * a.x,   a.y * a.y,   a.z * a.z,   a.w * a.w  }; }
vec4  v4_muls  (vec4 a, float s) { return (vec4){ a.x * s,   a.y * s,   a.z * s,   a.w * s   }; }
vec4  v4_mad(vec4 a, vec4 b, float c) { return (vec4){a.x + b.x * c, a.y + b.y * c, a.z + b.z * c, a.w + b.w * c}; }
vec4  v4_div(vec4 a, float b) { float v = 1.0f / b; return (vec4){a.x * v, a.y * v, a.z * v, a.w * v}; }
vec4  v4_neg(vec4 a) { return (vec4){-a.x, -a.y, -a.z, -a.w}; }
vec4  v4_inverse(vec4 a) { return v4_div((vec4){-a.x, -a.y, -a.z, a.w}, (a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w)); }
vec4  v4_inverse_normalized(vec4 a) { return (vec4){-a.x, -a.y, -a.z, a.w}; }
float v4_dot(vec4 a, vec4 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
float v4_length(vec4 a) { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w); }
vec4  v4_normalize(vec4 a) { float v = v4_length(a); v = v > 0 ? 1.0f / v : 0.0f; return (vec4){a.x * v, a.y * v, a.z * v, a.w * v}; }
bool v4_equal(vec4 a, vec4 b) { return (a.x == b.x) & (a.y == b.y) & (a.z == b.z) & (a.w == b.w); }

vec4 v4_lerp(vec4 a, vec4 b, float t){

    vec4 r;
    float t_ = 1 - t;
    r.x = t_*a.x + t*b.x;
    r.y = t_*a.y + t*b.y;
    r.z = t_*a.z + t*b.z;
    r.w = t_*a.w + t*b.w;
    r = v4_normalize(r);
    return r;
}


vec3 m4_v3_mult(mat4 m, vec3 v) {
    vec3 ret;
    float *pvec = &ret;
    for (int i = 0; i < 3; i++) {
        pvec[i] = v.x * m.m[0][i] + v.y * m.m[1][i] + v.z * m.m[2][i] + m.m[3][i];
    }
    return ret;
}

mat4 m4_rotation_quaternion(vec4 quaternion)
{

    vec4 q = quaternion;

    mat4 mat;

    mat.m[0][0] = (1 - 2 * q.y*q.y - 2 * q.z*q.z);
    mat.m[0][1] = (2 * q.x*q.y + 2 * q.z*q.w);
    mat.m[0][2] = (2 * q.x*q.z - 2 * q.y*q.w);
    mat.m[0][3] = 0.f;

    mat.m[1][0] = (2 * q.x*q.y - 2 * q.z*q.w);
    mat.m[1][1] = (1 - 2 * q.x*q.x - 2 * q.z*q.z);
    mat.m[1][2] = (2 * q.y*q.z + 2 * q.x*q.w);
    mat.m[1][3] = 0.f;

    mat.m[2][0] = (2 * q.x*q.z + 2 * q.y*q.w);
    mat.m[2][1] = (2 * q.y*q.z - 2 * q.x*q.w);
    mat.m[2][2] = (1 - 2 * q.x*q.x - 2 * q.y*q.y);
    mat.m[2][3] = 0.f;

    mat.m[3][0] = 0;
    mat.m[3][1] = 0;
    mat.m[3][2] = 0;
    mat.m[3][3] = 1.f;

    return mat;
}

mat4 m4_rotation_matrix(vec3 degrees)
{
  degrees.x *= M_PI / 180;  // convert to radians
  degrees.y *= M_PI / 180;
  degrees.z *= M_PI / 180;

  float sin_x = sin(degrees.x);
  float cos_x = cos(degrees.x);
  float sin_y = sin(degrees.y);
  float cos_y = cos(degrees.y);
  float sin_z = sin(degrees.z);
  float cos_z = cos(degrees.z);

  mat4 result;

  result.m[0][0] = cos_z * cos_y;
  result.m[1][0] = cos_z * sin_y * sin_x + sin_z * cos_x;
  result.m[2][0] = -cos_z * sin_y * cos_x + sin_z * sin_x;
  result.m[3][0] = 0;

  result.m[0][1] = -sin_z * cos_y;
  result.m[1][1] = -sin_z * sin_y * sin_x + cos_z * cos_x;
  result.m[2][1] = sin_z * sin_y * cos_x + cos_z * sin_x;
  result.m[3][1] = 0;

  result.m[0][2] = sin_y;
  result.m[1][2] = -cos_y * sin_x;
  result.m[2][2] = cos_y * cos_x;
  result.m[3][2] = 0;

  result.m[0][3] = 0;
  result.m[1][3] = 0;
  result.m[2][3] = 0;
  result.m[3][3] = 1;

  return result;
}

mat4 m4_transform(vec3 pos, vec3 scale, vec3 axis)
{
    mat4 mat;
    axis.x *= M_PI / 180;  // convert to radians
    axis.y *= M_PI / 180;
    axis.z *= M_PI / 180;

    float sin_x = sin(axis.x);
    float cos_x = cos(axis.x);
    float sin_y = sin(axis.y);
    float cos_y = cos(axis.y);
    float sin_z = sin(axis.z);
    float cos_z = cos(axis.z);

    mat.m[0][0] = scale.x *( cos_z * cos_y );
    mat.m[1][0] = scale.x *( cos_z * sin_y * sin_x + sin_z * cos_x );
    mat.m[2][0] = scale.x *( -cos_z * sin_y * cos_x + sin_z * sin_x );
    mat.m[3][0] = pos.x;

    mat.m[0][1] = scale.y *( -sin_z * cos_y );
    mat.m[1][1] = scale.y *( -sin_z * sin_y * sin_x + cos_z * cos_x );
    mat.m[2][1] = scale.y *( sin_z * sin_y * cos_x + cos_z * sin_x );
    mat.m[3][1] = pos.y;

    mat.m[0][2] = scale.z *( sin_y );
    mat.m[1][2] = scale.z *( -cos_y * sin_x );
    mat.m[2][2] = scale.z *( cos_y * cos_x );
    mat.m[3][2] = pos.z;

    mat.m[0][3] = 0;
    mat.m[1][3] = 0;
    mat.m[2][3] = 0;
    mat.m[3][3] = 1;

    return mat;
}

mat4 m4_transform_quaternion(vec3 translation, vec3 scale, vec4 rotation)
{
    vec4 q = rotation;
    float xx = q.x*q.x, xy = q.x*q.y, xz = q.x*q.z, xw = q.x*q.w;
    float yy = q.y*q.y, yz = q.y*q.z, yw = q.y*q.w;
    float zz = q.z*q.z, zw = q.z*q.w;
    float sx = 2.0f * scale.x, sy = 2.0f * scale.y, sz = 2.0f * scale.z;
    return mat4_rowsf(
        sx * (- yy - zz + 0.5f), sy * (- zw + xy), sz * (+ xz + yw), translation.x,
        sx * (+ xy + zw), sy * (- xx - zz + 0.5f), sz * (- xw + yz), translation.y,
        sx * (- yw + xz), sy * (+ xw + yz), sz * (- xx - yy + 0.5f), translation.z,
        0, 0, 0, 1
    );
}

mat4 m4_m4_rotation_matrix(mat4 mat, vec3 degrees)
{
  degrees.x *= M_PI / 180;  // convert to radians
  degrees.y *= M_PI / 180;
  degrees.z *= M_PI / 180;

  float sin_x = sin(degrees.x);
  float cos_x = cos(degrees.x);
  float sin_y = sin(degrees.y);
  float cos_y = cos(degrees.y);
  float sin_z = sin(degrees.z);
  float cos_z = cos(degrees.z);

  mat.m[0][0] = cos_z * cos_y;
  mat.m[1][0] = cos_z * sin_y * sin_x + sin_z * cos_x;
  mat.m[2][0] = -cos_z * sin_y * cos_x + sin_z * sin_x;
  mat.m[3][0] = mat.m[3][0];

  mat.m[0][1] = -sin_z * cos_y;
  mat.m[1][1] = -sin_z * sin_y * sin_x + cos_z * cos_x;
  mat.m[2][1] = sin_z * sin_y * cos_x + cos_z * sin_x;
  mat.m[3][1] = mat.m[3][1];

  mat.m[0][2] = sin_y;
  mat.m[1][2] = -cos_y * sin_x;
  mat.m[2][2] = cos_y * cos_x;
  mat.m[3][2] = mat.m[3][2];

  mat.m[0][3] = mat.m[0][3];
  mat.m[1][3] = mat.m[1][3];
  mat.m[2][3] = mat.m[2][3];
  mat.m[3][3] = mat.m[3][3];

  return mat;
}

mat4 m4_rotation_mat_quternion(mat4 m1, vec4 quaternion)
{
    vec4 q = quaternion;
    float xx = q.x*q.x, xy = q.x*q.y, xz = q.x*q.z, xw = q.x*q.w;
    float yy = q.y*q.y, yz = q.y*q.z, yw = q.y*q.w;
    float zz = q.z*q.z, zw = q.z*q.w;
    float sx = 2.0f * 1, sy = 2.0f * 1, sz = 2.0f * 1;
    return mat4_rowsf(
        sx * (- yy - zz + 0.5f), sy * (- zw + xy), sz * (+ xz + yw), m1.m[3][0],
        sx * (+ xy + zw), sy * (- xx - zz + 0.5f), sz * (- xw + yz), m1.m[3][1],
        sx * (- yw + xz), sy * (+ xw + yz), sz * (- xx - yy + 0.5f), m1.m[3][2],
        m1.m[0][3], m1.m[1][3], m1.m[2][3], 1
    );
}

mat4 m4_look_at(vec3 from, vec3 to, vec3 up) {
    vec3 z = v3_muls(v3_norm(v3_sub(to, from)), -1);
    vec3 x = v3_norm(v3_cross(up, z));
    vec3 y = v3_cross(z, x);

    return mat4_rowsf(
        x.x, x.y, x.z, -v3_dot(from, x),
        y.x, y.y, y.z, -v3_dot(from, y),
        z.x, z.y, z.z, -v3_dot(from, z),
        0,   0,   0,    1
    );
}

mat4 m4_translate(vec3 pos){

    mat4 mat = edenMat;

    mat.m[3][0] += pos.x;
    mat.m[3][1] += pos.y;
    mat.m[3][2] += pos.z;

    return mat;
}

mat4 m4_translate_mat(mat4 mat, vec3 pos){

    mat.m[3][0] = pos.x;
    mat.m[3][1] = pos.y;
    mat.m[3][2] = pos.z;

    return mat;
}

mat4 m4_translate_mat_add(mat4 mat, vec3 pos){

    mat.m[3][0] += pos.x;
    mat.m[3][1] += pos.y;
    mat.m[3][2] += pos.z;

    return mat;
}

mat4 m4_add(mat4 m1, mat4 m2){
    mat4 result;

    result.m[0][0] = m1.m[0][0] + m2.m[0][0];
    result.m[1][0] = m1.m[1][0] + m2.m[1][0];
    result.m[2][0] = m1.m[2][0] + m2.m[2][0];
    result.m[3][0] = m1.m[3][0] + m2.m[3][0];

    result.m[0][1] = m1.m[0][1] + m2.m[0][1];
    result.m[1][1] = m1.m[1][1] + m2.m[1][1];
    result.m[2][1] = m1.m[2][1] + m2.m[2][1];
    result.m[3][1] = m1.m[3][1] + m2.m[3][1];

    result.m[0][2] = m1.m[0][2] + m2.m[0][2];
    result.m[1][2] = m1.m[1][2] + m2.m[1][2];
    result.m[2][2] = m1.m[2][2] + m2.m[2][2];
    result.m[3][2] = m1.m[3][2] + m2.m[3][2];

    result.m[0][3] = m1.m[0][3] + m2.m[0][3];
    result.m[1][3] = m1.m[1][3] + m2.m[1][3];
    result.m[2][3] = m1.m[2][3] + m2.m[2][3];
    result.m[3][3] = m1.m[3][3] + m2.m[3][3];

    return result;
}

mat4 m4_mult(mat4 m1, mat4 m2){
    mat4 result;

    result.m[0][0] = (m1.m[0][0] * m2.m[0][0]) + (m1.m[0][1] * m2.m[1][0]) + (m1.m[0][2] * m2.m[2][0]) + (m1.m[0][3] * m2.m[3][0]);
    result.m[1][0] = (m1.m[0][0] * m2.m[0][1]) + (m1.m[0][1] * m2.m[1][1]) + (m1.m[0][2] * m2.m[2][1]) + (m1.m[0][3] * m2.m[3][1]);
    result.m[2][0] = (m1.m[0][0] * m2.m[0][2]) + (m1.m[0][1] * m2.m[1][2]) + (m1.m[0][2] * m2.m[2][2]) + (m1.m[0][3] * m2.m[3][2]);
    result.m[3][0] = (m1.m[0][0] * m2.m[0][3]) + (m1.m[0][1] * m2.m[1][3]) + (m1.m[0][2] * m2.m[2][3]) + (m1.m[0][3] * m2.m[3][3]);

    result.m[0][1] = (m1.m[1][0] * m2.m[0][0]) + (m1.m[1][1] * m2.m[1][0]) + (m1.m[1][2] * m2.m[2][0]) + (m1.m[1][3] * m2.m[3][0]);
    result.m[1][1] = (m1.m[1][0] * m2.m[0][1]) + (m1.m[1][1] * m2.m[1][1]) + (m1.m[1][2] * m2.m[2][1]) + (m1.m[1][3] * m2.m[3][1]);
    result.m[2][1] = (m1.m[1][0] * m2.m[0][2]) + (m1.m[1][1] * m2.m[1][2]) + (m1.m[1][2] * m2.m[2][2]) + (m1.m[1][3] * m2.m[3][2]);
    result.m[3][1] = (m1.m[1][0] * m2.m[0][3]) + (m1.m[1][1] * m2.m[1][3]) + (m1.m[1][2] * m2.m[2][3]) + (m1.m[1][3] * m2.m[3][3]);

    result.m[0][2] = (m1.m[2][0] * m2.m[2][0]) + (m1.m[2][1] * m2.m[1][0]) + (m1.m[2][2] * m2.m[2][0]) + (m1.m[2][3] * m2.m[3][0]);
    result.m[1][2] = (m1.m[2][0] * m2.m[0][1]) + (m1.m[2][1] * m2.m[1][1]) + (m1.m[2][2] * m2.m[2][1]) + (m1.m[2][3] * m2.m[3][1]);
    result.m[2][2] = (m1.m[2][0] * m2.m[0][2]) + (m1.m[2][1] * m2.m[1][2]) + (m1.m[2][2] * m2.m[2][2]) + (m1.m[2][3] * m2.m[3][2]);
    result.m[3][2] = (m1.m[2][0] * m2.m[0][3]) + (m1.m[2][1] * m2.m[1][3]) + (m1.m[2][2] * m2.m[2][3]) + (m1.m[2][3] * m2.m[3][3]);

    result.m[0][3] = (m1.m[3][0] * m2.m[0][0]) + (m1.m[3][1] * m2.m[1][0]) + (m1.m[3][2] * m2.m[2][0]) + (m1.m[3][3] * m2.m[3][0]);
    result.m[1][3] = (m1.m[3][0] * m2.m[0][1]) + (m1.m[3][1] * m2.m[1][1]) + (m1.m[3][2] * m2.m[2][1]) + (m1.m[3][3] * m2.m[3][1]);
    result.m[2][3] = (m1.m[3][0] * m2.m[0][2]) + (m1.m[3][1] * m2.m[1][2]) + (m1.m[3][2] * m2.m[2][2]) + (m1.m[3][3] * m2.m[3][2]);
    result.m[3][3] = (m1.m[3][0] * m2.m[0][3]) + (m1.m[3][1] * m2.m[1][3]) + (m1.m[3][2] * m2.m[2][3]) + (m1.m[3][3] * m2.m[3][3]);

    return result;
}

mat4 m4_scale_mat(vec3 scale){

    mat4 scale_mat;

    scale_mat = edenMat;

    scale_mat.m[0][0] = scale.x;
    scale_mat.m[1][1] = scale.y;
    scale_mat.m[2][2] = scale.z;

    return scale_mat;
}

mat4 m4_scale(mat4 mat, vec3 scale){
    mat4 scale_mat;

    scale_mat = edenMat;

    scale_mat.m[0][0] = scale.x;
    scale_mat.m[1][1] = scale.y;
    scale_mat.m[2][2] = scale.z;

    mat4 result = m4_mult(mat, scale_mat);

    return result;
}

mat4 m4_perspective(float fov_degrees, float near_plane, float far_plane)
{

  float aspect_ratio = ((float)swapChainExtent.width) / ((float) swapChainExtent.height);

  float range = far_plane  - near_plane;
  float tan_half_fov = tanf((fov_degrees * (M_PI / 180)) / 2.0);

  mat4 matrix;

  matrix.m[0][0] = 1.0f / (tan_half_fov * aspect_ratio);
  matrix.m[0][1] = 0.0f;
  matrix.m[0][2] = 0.0f;
  matrix.m[0][3] = 0.0f;

  matrix.m[1][0] = 0.0f;
  matrix.m[1][1] = 1.0f / tan_half_fov;
  matrix.m[1][2] = 0.0f;
  matrix.m[1][3] = 0.0f;

  matrix.m[2][0] = 0.0f;
  matrix.m[2][1] = 0.0f;
  matrix.m[2][2] = far_plane / (float) range;
  matrix.m[2][3] = 1.0f;

  matrix.m[3][0] = 0.0f;
  matrix.m[3][1] = 0.0f;
  matrix.m[3][2] = -(far_plane * near_plane) / (float) range;
  matrix.m[3][3] = 0.0f;

  return matrix;
}

mat4 mat4_mult_transform(mat4 m1, mat4 m2)
{
    for (int i = 0; i < 4; ++i)
    {
        float l0 = m1.m[i][0];
        float l1 = m1.m[i][1];
        float l2 = m1.m[i][2];

        float r0 = l0 * m2.m[0][0] + l1 * m2.m[1][0] + l2 * m2.m[2][0];
        float r1 = l0 * m2.m[0][1] + l1 * m2.m[1][1] + l2 * m2.m[2][1];
        float r2 = l0 * m2.m[0][2] + l1 * m2.m[1][2] + l2 * m2.m[2][2];

        m1.m[i][0] = r0;
        m1.m[i][1] = r1;
        m1.m[i][2] = r2;
    }

    m1.m[3][0] += m2.m[3][0];
    m1.m[3][1] += m2.m[3][1];
    m1.m[3][2] += m2.m[3][2];

    return m1;
}

mat4 mat4_colmnsf(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33)
{
    return (mat4){
        .m[0][0] = m00, .m[1][0] = m01, .m[2][0] = m02, .m[3][0] = m03,
        .m[0][1] = m10, .m[1][1] = m11, .m[2][1] = m12, .m[3][1] = m13,
        .m[0][2] = m20, .m[1][2] = m21, .m[2][2] = m22, .m[3][2] = m23,
        .m[0][3] = m30, .m[1][3] = m31, .m[2][3] = m32, .m[3][3] = m33
    };
}

mat4 mat4_rowsf(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33)
{
    return (mat4){
        .m[0][0] = m00, .m[1][0] = m10, .m[2][0] = m20, .m[3][0] = m30,
        .m[0][1] = m01, .m[1][1] = m11, .m[2][1] = m21, .m[3][1] = m31,
        .m[0][2] = m02, .m[1][2] = m12, .m[2][2] = m22, .m[3][2] = m32,
        .m[0][3] = m03, .m[1][3] = m13, .m[2][3] = m23, .m[3][3] = m33
    };
}
