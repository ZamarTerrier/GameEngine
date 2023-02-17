#ifndef LIGHTOBJECT_H
#define LIGHTOBJECT_H

#include "engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    ENGINE_LIGHT_TYPE_DIRECTIONAL,
    ENGINE_LIGHT_TYPE_POINT,
    ENGINE_LIGHT_TYPE_SPOT
} ELightType;

typedef struct{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float cutOff;

    char type;
}LightObject;

void LightObjectInit(LightObject *lo, ELightType type);

void LightObjectSetPosition(LightObject *lo, float x, float y, float z);
vec3 LightObjectGetPosition(LightObject *lo);

void LightObjectSetDirection(LightObject *lo, float x, float y, float z);
vec3 LightObjectGetDirection(LightObject *lo);

void LightObjectSetAmbientColor(LightObject *lo, float r, float g, float b);
void LightObjectSetDiffuseColor(LightObject *lo, float r, float g, float b);
void LightObjectSetSpecularColor(LightObject *lo, float r, float g, float b);

void LightObjectDraw(LightObject *lo);

#ifdef __cplusplus
}
#endif

#endif // LIGHTOBJECT_H
