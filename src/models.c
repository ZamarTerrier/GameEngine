#include "models.h"

#include <vulkan/vulkan.h>

#include "camera.h"
#include "gameObject.h"
#include "lightObject.h"

#include "buffers.h"
#include "texture.h"

#include "e_math.h"

#include "pipeline.h"

#include "objLoader.h"
#include "fbxLoader.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

//Описание вертекса
EIVertexInputBindingDescription ModelObject3DGetBindingDescription() {
    EIVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(ModelVertex3D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

void ModelDefaultDraw(ModelObject3D* mo){

    for(int i=0; i < mo->num_draw_nodes;i++){
        for(int j=0; j < mo->nodes[i].num_models; j++){

            ModelStruct *model = &mo->nodes[i].models[j];

            for(int l=0; l < model->graphObj.gItems.num_shader_packs;l++)
            {
                ShaderPack *pack = &model->graphObj.gItems.shader_packs[l];

                for(int k=0; k < pack->num_pipelines; k++){
                    vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[l].pipeline);

                    PipelineSetting *settings = &model->graphObj.blueprints.blue_print_packs[i].settings[j];

                    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
                    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

                    VkBuffer vertexBuffers[] = {model->graphObj.shape.vParam.vertexBuffer};
                    VkDeviceSize offsets[] = {0};

                    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

                    vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[k].layout, 0, 1, &pack->descriptor.descr_sets[imageIndex], 0, NULL);

                    if(settings->flags & ENGINE_PIPELINE_FLAG_DRAW_INDEXED){
                        vkCmdBindIndexBuffer(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                        vkCmdDrawIndexed(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                    }else
                        vkCmdDraw(commandBuffers[imageIndex], model->graphObj.shape.vParam.verticesSize, 1, 0, 0);
                }
            }
        }
    }
}

void ModelDefaultUpdate(ModelObject3D* mo){

    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            Camera3D* cam = (Camera3D*) cam3D;

            ModelBuffer3D mbo = {};
            vec3 cameraUp = {0.0f,1.0f, 0.0f};

            mo->nodes[i].model = m4_transform(mo->transform.position, mo->transform.scale, mo->transform.rotation);

            mbo.model = mo->nodes[i].model;
            mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
            mbo.proj = m4_perspective(45.0f, 0.01f, MAX_CAMERA_VIEW_DISTANCE);
            mbo.proj.m[1][1] *= -1;

            DescriptorUpdate(&mo->nodes[i].models[j].graphObj.blueprints, 1, 0, &mbo, sizeof(mbo));

            InvMatrixsBuffer imb = {};
            memset(&imb, 0, sizeof(InvMatrixsBuffer));

            DescriptorUpdate(&mo->nodes[i].models[j].graphObj.blueprints, 1, 1, &imb, sizeof(imb));

            LightBuffer3D lbo = {};
            memset(&lbo, 0, sizeof(LightBuffer3D));

            LightObjectFillLights(&lbo, mo->nodes[i].models[j].light_enable);

            DescriptorUpdate(&mo->nodes[i].models[j].graphObj.blueprints, 1, 2, &lbo, sizeof(lbo));
        }
    }
}

void ModelClean(ModelObject3D* mo){
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            GraphicsObjectClean(&mo->nodes[i].models[j].graphObj);
        }
    }
}

//Не корректно
void ModelRecreate(ModelObject3D* mo){
    /*
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            ModelStruct *model = &mo->nodes[i].models[j];

            PipelineSetting *settings = model->graphObj.gItems.settings;

            for(int m=0; m < model->graphObj.gItems.settingsCount;m++)
            {
                settings[m].scissor.offset.x = 0;
                settings[m].scissor.offset.y = 0;
                settings[m].scissor.extent.height = HEIGHT;
                settings[m].scissor.extent.width = WIDTH;
                settings[m].viewport.x = 0;
                settings[m].viewport.y = 0;
                settings[m].viewport.height = HEIGHT;
                settings[m].viewport.width = WIDTH;
            }

            BuffersRecreateUniform(&model->graphObj.blueprints);
            GraphicsObjectCreateDrawItems(&model->graphObj, false);
            PipelineCreateGraphics(&model->graphObj, false);
        }
    }*/
}

//Не корректно
void ModelSetSomeViewport(ModelObject3D* mo, float x, float y, float height, float width)
{
    /*
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            ModelStruct *model = &mo->nodes[i].models[j];

            PipelineSetting *settings = (PipelineSetting *)model->graphObj.gItems.settings;

            for(int m=0; m < model->graphObj.gItems.settingsCount; m++)
            {
                settings[m].viewport.x = x;
                settings[m].viewport.y = y;
                settings[m].viewport.height = height;
                settings[m].viewport.width = width;
            }
        }
    }*/
}

void ModelDestroy(ModelObject3D* mo){
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            GraphicsObjectDestroy(&mo->nodes[i].models[j].graphObj);
        }
    }
}

void ModelAddSettingPipeline(ModelStruct* model, uint32_t indx_pack, PipelineSetting setting){


    uint32_t indx = model->graphObj.blueprints.blue_print_packs[indx_pack].num_settings;

    PipelineSetting *settings = &model->graphObj.blueprints.blue_print_packs[indx_pack].settings;

    memcpy(&settings[indx], &setting, sizeof(PipelineSetting));

    model->graphObj.blueprints.blue_print_packs[indx_pack].num_settings ++;
}

void ModelSetLightEnable(void *obj, bool enable)
{
    ModelObject3D *mo = (ModelObject3D *)obj;
    for(int i=0;i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            mo->nodes[i].models[j].light_enable = enable;
        }
    }
}

void ModelSetSelCameraEnable(void *obj, bool enable)
{
    ModelObject3D *mo = (ModelObject3D *)obj;
    for(int i=0;i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            mo->nodes[i].models[j].selfCamera = enable;
        }
    }
}

//Не корректно
void ModelDefaultInit(ModelStruct *model, DrawParam *dParam){

    /*
    model->graphObj.blueprints.num_blue_print_packs = 2;
    model->graphObj.blueprints.blue_print_packs[0].render_point = render_texture;
    model->graphObj.blueprints.blue_print_packs[1].render_point = window_render;

    BluePrintAddUniformObject(&model->graphObj.blueprints, 0, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);

    BluePrintAddUniformObject(&model->graphObj.blueprints, 1, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&model->graphObj.blueprints, 1, sizeof(LightSpaceMatrix), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&model->graphObj.blueprints, 1, sizeof(InvMatrixsBuffer), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&model->graphObj.blueprints, 1, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT);

    BluePrintAddTextureImage(&model->graphObj.blueprints, 1, model->diffuse);
    //TextureImageAdd(&model->graphObj.blueprints, model->specular);
    BluePrintAddTextureImage(&model->graphObj.blueprints, 1, model->normal);

    GraphicsObjectCreateDrawItems(&model->graphObj, true);

    PipelineSetting setting;

    PipelineSettingSetDefault(&model->graphObj, &setting);

    setting.vertShader = &_binary_shaders_depth_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_depth_vert_spv_size);
    setting.fragShader = &_binary_shaders_depth_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_depth_frag_spv_size);
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    setting.drawType = 0;
    setting.fromFile = 0;
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW);
    ModelAddSettingPipeline(model, 0, setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_model_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_model_vert_spv_size);
        setting.fragShader = &_binary_shaders_model_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_model_frag_spv_size);
        setting.fromFile = 0;
    }
    else
        GraphicsObjectSetShadersPath(&model->graphObj, dParam->vertShader, dParam->fragShader);

    ModelAddSettingPipeline(model, 1, setting);

    PipelineCreateGraphics(&model->graphObj, true);

    model->light_enable = false;*/

}
