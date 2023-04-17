#ifndef E_BUFFERS_VARIABLES_H
#define E_BUFFERS_VARIABLES_H

#include "e_math_variables.h"

#define MAX_BONES 128
#define MAX_LIGHTS 16

#define MAX_LIGHT_MATRIX 16

typedef struct {
    vec2 position;
    vec2 rotation;
    vec2 scale;
} TransformBuffer2D;

typedef struct{
    vec2 origin;
    vec2 offset;
    vec2 rotation;
    vec2 scale;
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
    vec3 direction __attribute__ ((aligned (16)));

    vec3 ambient __attribute__ ((aligned (16)));
    vec3 diffuse __attribute__ ((aligned (16)));
    vec3 specular __attribute__ ((aligned (16)));
} DirLight;

typedef struct{
    int num_points;
    int num_spots;
    int light_react;
} LightStatus;

typedef struct{
    DirLight dir;
    PointLight lights[MAX_LIGHTS];
    SpotLight spots [MAX_LIGHTS];
    LightStatus status __attribute__ ((aligned (16)));
} LightBuffer3D;

typedef struct{
    vec3 light_pos __attribute__ ((aligned (16)));
    vec3 view_pos __attribute__ ((aligned (16)));
} LightPosBuff;

typedef struct{
    mat4 proj __attribute__ ((aligned (16)));
    mat4 view __attribute__ ((aligned (16)));
} LightMatrix;

typedef struct{
    LightMatrix mats[MAX_LIGHT_MATRIX];
    vec4 cascadeSplits __attribute__ ((aligned (16)));
} LightSpaceMatrix;

typedef struct{
    mat4 mats[MAX_BONES] __attribute__ ((aligned (16)));
    float size __attribute__ ((aligned (16)));
} InvMatrixsBuffer;

#endif // E_BUFFERS_VARIABLES_H
