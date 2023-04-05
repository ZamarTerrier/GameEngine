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

    Camera3D* cam = (Camera3D*) cam3D;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};
    vec3 zero = {0.0f, 0.0f, 0.0f};

    go->transform.model = m4_translate_mat(m4_mult(m4_scale_mat(go->transform.scale), m4_rotation_matrix(go->transform.rotation)), go->transform.position);

    mbo.model = go->transform.model;
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.01f, MAX_CAMERA_VIEW_DISTANCE);
    mbo.proj.m[1][1] *= -1;

    DescriptorUpdate(&go->graphObj.blueprints, 1, 0, &mbo, sizeof(mbo));

    LightSpaceMatrix lsm;
    //mbo.model = edenMat;
    mbo.view = lsm.view = m4_look_at(some_light.position, v3_add(some_light.position, some_light.rotation), cameraUp);
    mbo.proj = lsm.proj = m4_ortho(-ORITO_SIZE, ORITO_SIZE, -ORITO_SIZE, ORITO_SIZE, -MAX_SHADOW_VIEW_DISTANCE, MAX_SHADOW_VIEW_DISTANCE);

    DescriptorUpdate(&go->graphObj.blueprints, 1, 1, &lsm, sizeof(lsm));
    DescriptorUpdate(&go->graphObj.blueprints, 0, 0, &mbo, sizeof(mbo));

    LightBuffer3D lbo = {};
    memset(&lbo, 0, sizeof(LightBuffer3D));

    LightObjectFillLights(&lbo, go->enable_light);

    DescriptorUpdate(&go->graphObj.blueprints, 1, 2, &lbo, sizeof(lbo));
}

void GameObject3DDefaultDraw(GameObject3D* go, void *command){

    for(int i=0; i < go->graphObj.gItems.num_shader_packs;i++)
    {
        BluePrintPack *pack = &go->graphObj.blueprints.blue_print_packs[i];

        if(pack->render_point == current_render)
        {
            ShaderPack *pack = &go->graphObj.gItems.shader_packs[i];

            for(int j=0; j < pack->num_pipelines; j++){

                PipelineSetting *settings = &go->graphObj.blueprints.blue_print_packs[i].settings[j];

                vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].pipeline);

                if(settings->flags & ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW){
                    vkCmdSetViewport(command, 0, 1, &settings->viewport);
                    vkCmdSetScissor(command, 0, 1, &settings->scissor);
                }

                vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].layout, 0, 1, &pack->descriptor.descr_sets[imageIndex], 0, NULL);

                VkBuffer vertexBuffers[] = {go->graphObj.shapes[settings->vert_indx].vParam.vertexBuffer};
                VkDeviceSize offsets[] = {0};

                vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);

                if(settings->flags & ENGINE_PIPELINE_FLAG_DRAW_INDEXED){
                    vkCmdBindIndexBuffer(command, go->graphObj.shapes[settings->vert_indx].iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(command, go->graphObj.shapes[settings->vert_indx].iParam.indexesSize, 1, 0, 0, 0);
                }else
                    vkCmdDraw(command, go->graphObj.shapes[settings->vert_indx].vParam.verticesSize, 1, 0, 0);
            }
        }
    }
}

void GameObject3DInitDraw(GameObject3D *go)
{
    GraphicsObjectCreateDrawItems(&go->graphObj);

    PipelineCreateGraphics(&go->graphObj);
}

void GameObject3DAddShadowDescriptor(GameObject3D *go, void *render)
{
    uint32_t nums = go->graphObj.blueprints.num_blue_print_packs;
    go->graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&go->graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    setting.vertShader = &_binary_shaders_depth_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_depth_vert_spv_size);
    setting.fragShader = &_binary_shaders_depth_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_depth_frag_spv_size);
    setting.fromFile = 0;
    setting.vert_indx = 0;

    setting.flags &= ~(ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW);
    //setting.flags |= ENGINE_PIPELINE_FLAG_BIAS;
    setting.cull_mode = VK_CULL_MODE_NONE;

    GameObject3DAddSettingPipeline(go, nums, &setting);

    go->graphObj.blueprints.num_blue_print_packs ++;
}

void GameObject3DAddSettingPipeline(GameObject3D* go, uint32_t indx_pack, void *arg){
    PipelineSetting* setting = arg;

    uint32_t indx = go->graphObj.blueprints.blue_print_packs[indx_pack].num_settings;

    PipelineSetting* settings = &go->graphObj.blueprints.blue_print_packs[indx_pack].settings;
    memcpy(&settings[indx], setting, sizeof(PipelineSetting));

    go->graphObj.blueprints.blue_print_packs[indx_pack].num_settings ++;

}

void GameObject3DClean(GameObject3D* go){
    GraphicsObjectClean(&go->graphObj);
}

void GameObject3DRecreate(GameObject3D* go){

    for(int i=0; i < go->graphObj.gItems.num_shader_packs;i++)
    {
        BluePrintPack *pack = &go->graphObj.blueprints.blue_print_packs[i];

        PipelineSetting *settings = pack->settings;

        for(int i=0; i < pack->num_settings;i++)
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
    }

    BuffersRecreateUniform(&go->graphObj.blueprints);

    GraphicsObjectCreateDrawItems(&go->graphObj);
    PipelineCreateGraphics(&go->graphObj);
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

    for(int i=0; i < go->graphObj.num_shapes; i++)
    {
        if(go->graphObj.shapes[i].vParam.verticesSize)
            free(go->graphObj.shapes[i].vParam.vertices);

        if(go->graphObj.shapes[i].iParam.indexesSize)
            free(go->graphObj.shapes[i].iParam.indices);
    }
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

    go->enable_light = false;
    go->wired = false;
}

void GameObject3DEnableLight(GameObject3D *go, bool enable)
{
    go->enable_light = enable;
}
