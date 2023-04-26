#include "lightObject.h"

#include "e_resource_data.h"

void LightObjectFillDirLights(DirLightBuffer *blb)
{
    uint32_t num_dirs = 0;

    if(e_var_num_lights > 0)
    {
        LightObject **lights = e_var_lights;

        for(int i=0;i < e_var_num_lights; i++)
        {
            if (lights[i]->type == ENGINE_LIGHT_TYPE_DIRECTIONAL){
                blb->dir[num_dirs].ambient = lights[i]->ambient;
                blb->dir[num_dirs].diffuse = lights[i]->diffuse;
                blb->dir[num_dirs].specular = lights[i]->specular;
                blb->dir[num_dirs].direction = lights[i]->direction;
                blb->dir[num_dirs].position = lights[i]->position;

                num_dirs ++;
            }
        }
    }
}

void LightObjectFillPointLights(PointLightBuffer *plb)
{
    uint32_t num_points = 0;

    if(e_var_num_lights > 0)
    {
        LightObject **lights = e_var_lights;

        for(int i=0;i < e_var_num_lights; i++)
        {
            if (lights[i]->type == ENGINE_LIGHT_TYPE_POINT){
                plb->points[num_points].position = lights[i]->position;
                plb->points[num_points].constant = lights[i]->constant;
                plb->points[num_points].linear = lights[i]->linear;
                plb->points[num_points].quadratic = lights[i]->quadratic;
                plb->points[num_points].ambient = lights[i]->ambient;
                plb->points[num_points].diffuse = lights[i]->diffuse;
                plb->points[num_points].specular = lights[i]->specular;

                num_points ++;
            }
        }
    }
}

void LightObjectFillSpotLights(SpotLightBuffer *slb)
{
    uint32_t num_spots = 0;

    if(e_var_num_lights > 0)
    {
        LightObject **lights = e_var_lights;

        for(int i=0;i < e_var_num_lights; i++)
        {
            if (lights[i]->type == ENGINE_LIGHT_TYPE_SPOT){
                slb->spots[num_spots].position = lights[i]->position;
                slb->spots[num_spots].constant = lights[i]->constant;
                slb->spots[num_spots].linear = lights[i]->linear;
                slb->spots[num_spots].quadratic = lights[i]->quadratic;
                slb->spots[num_spots].ambient = lights[i]->ambient;
                slb->spots[num_spots].diffuse = lights[i]->diffuse;
                slb->spots[num_spots].specular = lights[i]->specular;
                slb->spots[num_spots].direction =  lights[i]->direction;
                slb->spots[num_spots].cutOff = lights[i]->cutOff;

                num_spots ++;
            }
        }
    }
}

void LightObjectFillLightStatus(LightStatusBuffer *lsb)
{
    if(e_var_num_lights > 0)
    {
        LightObject **lights = e_var_lights;

        for(int i=0;i < e_var_num_lights; i++)
        {
            switch(lights[i]->type){
                case ENGINE_LIGHT_TYPE_DIRECTIONAL:
                    lsb->num_dirs ++;
                    break;
                case ENGINE_LIGHT_TYPE_POINT:
                    lsb->num_points ++;
                    break;
                case ENGINE_LIGHT_TYPE_SPOT:
                    lsb->num_spots ++;
                    break;
            }
        }
    }
}

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
