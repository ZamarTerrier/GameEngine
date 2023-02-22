#include "gameObject3D.h"

#include <vulkan/vulkan.h>

#include "camera.h"

#include "lightObject.h"

#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

#include "e_math.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void GameObject3DDefaultUpdate(GameObject3D* go) {

    if(go->graphObj.local.descriptors == NULL)
        return;

    Camera3D* cam = (Camera3D*) cam3D;
    void* data;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    go->transform.model = m4_translate_mat(m4_mult(m4_scale_mat(go->transform.scale), m4_rotation_matrix(go->transform.rotation)), go->transform.position);

    mbo.model = go->transform.model;
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.01f, MAX_CAMERA_VIEW_DISTANCE);
    mbo.proj.m[1][1] *= -1;

    vkMapMemory(e_device, go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
    memcpy(data, &mbo, sizeof(mbo));
    vkUnmapMemory(e_device, go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);


    LightBuffer3D lbo = {};
    memset(&lbo, 0, sizeof(LightBuffer3D));

    if(e_var_num_lights > 0 && go->enable_light)
    {
        LightObject **lights = e_var_lights;

        for(int i=0;i < e_var_num_lights; i++)
        {

            switch (lights[i]->type) {
                case ENGINE_LIGHT_TYPE_DIRECTIONAL:
                    lbo.dir.ambient = lights[i]->ambient;
                    lbo.dir.diffuse = lights[i]->diffuse;
                    lbo.dir.specular = lights[i]->specular;
                    lbo.dir.direction = lights[i]->direction;
                    break;
                case ENGINE_LIGHT_TYPE_POINT:
                    lbo.num_points++;

                    lbo.lights[lbo.num_points - 1].position = lights[i]->position;
                    lbo.lights[lbo.num_points - 1].constant = lights[i]->constant;
                    lbo.lights[lbo.num_points - 1].linear = lights[i]->linear;
                    lbo.lights[lbo.num_points - 1].quadratic = lights[i]->quadratic;
                    lbo.lights[lbo.num_points - 1].ambient = lights[i]->ambient;
                    lbo.lights[lbo.num_points - 1].diffuse = lights[i]->diffuse;
                    lbo.lights[lbo.num_points - 1].specular = lights[i]->specular;

                    break;
                case ENGINE_LIGHT_TYPE_SPOT:
                    lbo.num_spots++;

                    lbo.lights[lbo.num_spots - 1].position = lights[i]->position;
                    lbo.lights[lbo.num_spots - 1].constant = lights[i]->constant;
                    lbo.lights[lbo.num_spots - 1].linear = lights[i]->linear;
                    lbo.lights[lbo.num_spots - 1].quadratic = lights[i]->quadratic;
                    lbo.lights[lbo.num_spots - 1].ambient = lights[i]->ambient;
                    lbo.lights[lbo.num_spots - 1].diffuse = lights[i]->diffuse;
                    lbo.lights[lbo.num_spots - 1].specular = lights[i]->specular;
                    lbo.spots[lbo.num_spots - 1].direction =  lights[i]->direction;
                    lbo.spots[lbo.num_spots - 1].cutOff = lights[i]->cutOff;
                    break;
                default:
                    break;
            }
        }
    }

    lbo.light_react = go->enable_light;

    vkMapMemory(e_device, go->graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(lbo), 0, &data);
    memcpy(data, &lbo, sizeof(lbo));
    vkUnmapMemory(e_device, go->graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);

}

void GameObject3DDefaultDraw(GameObject3D* go){

    for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){//go->graphObj.gItems.pipelineCount; i++){

        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.graphicsPipeline[i]);

        PipelineSetting *settings = &go->graphObj.gItems.settings[i];

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vParam.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelineLayout[i], 0, 1, &go->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

        switch(settings->drawType){
            case 0:
                vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                break;
            case 1:
                vkCmdDraw(commandBuffers[imageIndex], go->graphObj.shape.vParam.verticesSize, 1, 0, 0);
                break;
        }

    }

}

void GameObject3DAddSettingPipeline(GameObject3D* go, void *arg){
    PipelineSetting* setting = arg;

    PipelineSetting* settings = go->graphObj.gItems.settings;
    memcpy(&settings[go->graphObj.gItems.settingsCount], setting, sizeof(PipelineSetting));

    go->graphObj.gItems.settingsCount++;

}

void GameObject3DClean(GameObject3D* go){
    GraphicsObjectClean(&go->graphObj);
}

void GameObject3DRecreate(GameObject3D* go){

    PipelineSetting *settings = (PipelineSetting *)go->graphObj.gItems.settings;

    for(int i=0; i < go->graphObj.gItems.settingsCount;i++)
    {
        settings[i].scissor.offset.x = 0;
        settings[i].scissor.offset.y = 0;
        settings[i].scissor.extent.height = HEIGHT;
        settings[i].scissor.extent.width = WIDTH;
        settings[i].viewport.x = 0;
        settings[i].viewport.y = 0;
        settings[i].viewport.height = HEIGHT;
        settings[i].viewport.width = WIDTH;
    }

    BuffersRecreateUniform(&go->graphObj.local);
    GraphicsObjectCreateDrawItems(&go->graphObj);
    PipelineCreateGraphics(&go->graphObj);
}

void GameObject3DDestroy(GameObject3D* go){

    GraphicsObjectDestroy(&go->graphObj);

    if(go->diffuse != NULL)
    {
        free(go->diffuse->path);

        if(go->diffuse->size > 0)
            free(go->diffuse->buffer);

        free(go->diffuse);
    }

    if(go->specular != NULL)
    {
        free(go->specular->path);

        if(go->specular->size > 0)
            free(go->specular->buffer);

        free(go->specular);
    }

    if(go->normal != NULL)
    {
        free(go->normal->path);

        if(go->normal->size > 0)
            free(go->normal->buffer);

        free(go->normal);
    }

    if(!go->graphObj.shape.linked)
    {
        if(go->graphObj.shape.vParam.verticesSize)
            free(go->graphObj.shape.vParam.vertices);

        if(go->graphObj.shape.iParam.indexesSize)
            free(go->graphObj.shape.iParam.indices);
    }
}

void GameObject3DSetLinkedShape(GameObject3D *go)
{
    go->graphObj.shape.linked = true;
}

void GameObject3DInit(GameObject3D *go){

    GameObjectSetUpdateFunc(go, (void *)GameObject3DDefaultUpdate);
    GameObjectSetDrawFunc(go, (void *)GameObject3DDefaultDraw);
    GameObjectSetCleanFunc(go, (void *)GameObject3DClean);
    GameObjectSetRecreateFunc(go, (void *)GameObject3DRecreate);
    GameObjectSetDestroyFunc(go, (void *)GameObject3DDestroy);

    Transform3DInit(&go->transform);
    GraphicsObjectInit(&go->graphObj, ENGINE_VERTEX_TYPE_3D_OBJECT);

    go->graphObj.gItems.perspective = true;

    go->graphObj.shape.linked = false;
    go->enable_light = false;
    go->wired = false;
}

void GameObject3DEnableLight(GameObject3D *go, bool enable)
{
    go->enable_light = enable;
}
