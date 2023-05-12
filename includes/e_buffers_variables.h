#ifndef E_BUFFERS_VARIABLES_H
#define E_BUFFERS_VARIABLES_H

#include "e_math_variables.h"

#define MAX_BONES 128
#define MAX_LIGHTS 16

typedef struct {
    vec2 position;
    vec2 rotation;
    vec2 scale;
} TransformBuffer2D;

typedef struct{
    vec2 offset;
    vec2 rotation;
} ImageBufferObjects;

typedef struct{
    mat4 model;
    mat4 view;
    mat4 proj;
} ModelBuffer3D;

typedef struct {
    vec3 position __attribute__ ((aligned (16)));

    float constant;
    float linear;
    float quadratic;

    vec3 ambient __attribute__ ((aligned (16)));
    vec3 diffuse __attribute__ ((aligned (16)));
    vec3 specular __attribute__ ((aligned (16)));
} PointLight;

typedef struct {
    vec3 position __attribute__ ((aligned (16)));
    vec3  direction __attribute__ ((aligned (16)));

    float constant;
    float linear;
    float quadratic;
    float cutOff;

    vec3 ambient __attribute__ ((aligned (16)));
    vec3 diffuse __attribute__ ((aligned (16)));
    vec3 specular __attribute__ ((aligned (16)));
} SpotLight;

typedef struct{
    vec3 position __attribute__ ((aligned (16)));
    vec3 direction __attribute__ ((aligned (16)));

    vec3 ambient __attribute__ ((aligned (16)));
    vec3 diffuse __attribute__ ((aligned (16)));
    vec3 specular __attribute__ ((aligned (16)));
} DirLight;

typedef struct{
    mat4 proj;
    mat4 view;
} LightMatrix;

typedef struct{
    vec3 light_pos __attribute__ ((aligned (16)));
    vec3 view_pos __attribute__ ((aligned (16)));
} LightPosBuff;

typedef struct{
    DirLight dir[MAX_LIGHTS];
    LightMatrix mats[MAX_LIGHTS];
    vec4 cascadeSplits __attribute__ ((aligned (16)));
} DirLightBuffer;

typedef struct{
    PointLight points[MAX_LIGHTS];
    LightPosBuff pos[MAX_LIGHTS];
} PointLightBuffer;

typedef struct{
    SpotLight spots[MAX_LIGHTS];
    LightMatrix mats[MAX_LIGHTS];
} SpotLightBuffer;

typedef struct{
    int num_dirs;
    int num_points;
    int num_spots;
    int isEnable;
} LightStatusBuffer;

typedef struct{
    mat4 mats[MAX_BONES] __attribute__ ((aligned (16)));
    float size __attribute__ ((aligned (16)));
} InvMatrixsBuffer;

#endif // E_BUFFERS_VARIABLES_H
