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

    if(go->graphObj.blueprints.descriptors == NULL)
        return;

    Camera3D* cam = (Camera3D*) cam3D;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};
    vec3 zero = {0.0f, 0.0f, 0.0f};

    go->transform.model = m4_translate_mat(m4_mult(m4_scale_mat(go->transform.scale), m4_rotation_matrix(go->transform.rotation)), go->transform.position);

    mbo.model = go->transform.model;
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.01f, MAX_CAMERA_VIEW_DISTANCE);
    mbo.proj.m[1][1] *= -1;

    DescriptorUpdate(go->graphObj.blueprints.descriptors, 0, &mbo, sizeof(mbo));

    LightSpaceMatrix lsm;
    //mbo.model = edenMat;
    mbo.view = lsm.view = m4_look_at(some_light.position, v3_add(some_light.position, some_light.rotation), cameraUp);
    mbo.proj = lsm.proj = m4_ortho(-ORITO_SIZE, ORITO_SIZE, -ORITO_SIZE, ORITO_SIZE, -MAX_CAMERA_VIEW_DISTANCE, MAX_CAMERA_VIEW_DISTANCE);

    DescriptorUpdate(go->graphObj.blueprints.descriptors, 1, &lsm, sizeof(lsm));
    DescriptorUpdate(&go->graphObj.blueprints.shadow_descr, 0, &mbo, sizeof(mbo));

    LightBuffer3D lbo = {};
    memset(&lbo, 0, sizeof(LightBuffer3D));

    LightObjectFillLights(&lbo, go->enable_light);

    DescriptorUpdate(go->graphObj.blueprints.descriptors, 2, &lbo, sizeof(lbo));
}

void GameObject3DDefaultDraw(GameObject3D* go){

    if(display_draw)
    {
        for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){//go->graphObj.gItems.pipelineCount; i++){

            PipelineSetting *settings = &go->graphObj.gItems.settings[i];

            vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelines[i].pipeline);

            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

            vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelines[i].layout, 0, 1, &go->graphObj.gItems.descriptors.descr_sets[imageIndex], 0, NULL);

            VkBuffer vertexBuffers[] = {go->graphObj.shape.vParam.vertexBuffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

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
    }else{
        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.shadow.pipeline);

        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.shadow.layout, 0, 1, &go->graphObj.gItems.shadow_descr.descr_sets[imageIndex], 0, NULL);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vParam.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
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

    BuffersRecreateUniform(&go->graphObj.blueprints);
    GraphicsObjectCreateDrawItems(&go->graphObj, false);
    PipelineCreateGraphics(&go->graphObj, false);
}

void GameObject3DDestroy(GameObject3D* go){

    GraphicsObjectDestroy(&go->graphObj);

    for(int i=0;i < go->num_images;i++)
    {
        free(go->images[i].path);

        if(go->images[i].size > 0)
            free(go->images[i].buffer);
    }

    free(go->images);

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

    go->self.obj_type = ENGINE_GAME_OBJECT_TYPE_3D;

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
