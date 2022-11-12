#include "lightObject.h"

#include "e_resource.h"

void LightObjectInit(LightObject *lo, ELightType type)
{
    memset(lo, 0, sizeof(LightObject));

    lo->type = type;

    lo->linear = 1;
    lo->constant = 1.0f;
    lo->quadratic = 0.5f;
}

void LightObjectSetPosition(LightObject *lo, float x, float y, float z)
{
    lo->position.x = x;
    lo->position.y = y;
    lo->position.z = z;
}

vec3 LightObjectGetPosition(LightObject *lo)
{
    return lo->position;
}


void LightObjectSetDirection(LightObject *lo, float x, float y, float z)
{
    lo->direction.x = x;
    lo->direction.y = y;
    lo->direction.z = z;
}

vec3 LightObjectGetDirection(LightObject *lo)
{
    return lo->direction;
}

void LightObjectSetAmbientColor(LightObject *lo, float r, float g, float b)
{
    lo->ambient.x = r;
    lo->ambient.y = g;
    lo->ambient.z = b;
}

void LightObjectSetDiffuseColor(LightObject *lo, float r, float g, float b)
{
    lo->diffuse.x = r;
    lo->diffuse.y = g;
    lo->diffuse.z = b;
}

void LightObjectSetSpecularColor(LightObject *lo, float r, float g, float b)
{
    lo->specular.x = r;
    lo->specular.y = g;
    lo->specular.z = b;
}

void LightObjectDraw(LightObject *lo)
{
    LightObject **lights = e_var_lights;

    if(e_var_num_lights > 0)
    {
        for(int i=0; i < e_var_num_lights; i++)
            if(lights[i] == lo)
                return;
    }

    if(e_var_num_lights + 1 > 32)
        return;

    e_var_num_lights++;

    e_var_lights = realloc(e_var_lights, e_var_num_lights * sizeof(LightObject* ));
    e_var_lights[e_var_num_lights - 1] = lo;

}
