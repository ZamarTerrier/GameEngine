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


            if(display_draw)
            {
                for(int p=0;p < model->graphObj.gItems.pipelineCount;p++)
                {
                    vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, model->graphObj.gItems.pipelines[p].pipeline);

                    PipelineSetting *settings = &model->graphObj.gItems.settings[p];

                    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
                    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

                    VkBuffer vertexBuffers[] = {model->graphObj.shape.vParam.vertexBuffer};
                    VkDeviceSize offsets[] = {0};

                    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

                    vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, model->graphObj.gItems.pipelines[p].layout, 0, 1, &model->graphObj.gItems.descriptors.descr_sets[imageIndex], 0, NULL);

                    switch(settings->drawType){
                        case 0:
                            vkCmdBindIndexBuffer(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                            vkCmdDrawIndexed(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                            break;
                        case 1:
                            vkCmdDraw(commandBuffers[imageIndex], model->graphObj.shape.vParam.verticesSize, 1, 0, 0);
                            break;
                    }
                }
            }else{
                vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, model->graphObj.gItems.shadow.pipeline);

                vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, model->graphObj.gItems.shadow.layout, 0, 1, &model->graphObj.gItems.shadow_descr.descr_sets[imageIndex], 0, NULL);

                VkBuffer vertexBuffers[] = {model->graphObj.shape.vParam.vertexBuffer};
                VkDeviceSize offsets[] = {0};

                vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
            }
        }
    }
}

void ModelDefaultUpdate(ModelObject3D* mo){

    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            if(mo->nodes[i].models[j].graphObj.blueprints.descriptors == NULL)
                return;

            Camera3D* cam = (Camera3D*) cam3D;
            void* data;

            ModelBuffer3D mbo = {};
            vec3 cameraUp = {0.0f,1.0f, 0.0f};

            mo->nodes[i].model = m4_transform(mo->transform.position, mo->transform.scale, mo->transform.rotation);

            mbo.model = mo->nodes[i].model;
            mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
            mbo.proj = m4_perspective(45.0f, 0.01f, MAX_CAMERA_VIEW_DISTANCE);
            mbo.proj.m[1][1] *= -1;

            DescriptorUpdate(mo->nodes[i].models[j].graphObj.blueprints.descriptors, 0, &mbo, sizeof(mbo));

            InvMatrixsBuffer imb = {};
            memset(&imb, 0, sizeof(InvMatrixsBuffer));

            DescriptorUpdate(mo->nodes[i].models[j].graphObj.blueprints.descriptors, 1, &imb, sizeof(imb));

            LightBuffer3D lbo = {};
            memset(&lbo, 0, sizeof(LightBuffer3D));

            LightObjectFillLights(&lbo, mo->nodes[i].models[j].light_enable);

            DescriptorUpdate(mo->nodes[i].models[j].graphObj.blueprints.descriptors, 2, &lbo, sizeof(lbo));
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

void ModelRecreate(ModelObject3D* mo){
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
    }
}

void ModelSetSomeViewport(ModelObject3D* mo, float x, float y, float height, float width)
{
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
    }
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

void ModelAddSettingPipeline(ModelStruct* model, PipelineSetting setting){

    PipelineSetting *settings = model->graphObj.gItems.settings;

    memcpy(&settings[model->graphObj.gItems.settingsCount], &setting, sizeof(PipelineSetting));

    model->graphObj.gItems.settingsCount++;
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

void ModelDefaultInit(ModelStruct *model, DrawParam *dParam){

    BuffersAddUniformShadow(&model->graphObj.blueprints.shadow_descr, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);

    BuffersAddUniformObject(&model->graphObj.blueprints, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&model->graphObj.blueprints, sizeof(LightSpaceMatrix), VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&model->graphObj.blueprints, sizeof(InvMatrixsBuffer), VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&model->graphObj.blueprints, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT);

    TextureImageAdd(&model->graphObj.blueprints, model->diffuse);
    //TextureImageAdd(&model->graphObj.blueprints, model->specular);
    TextureImageAdd(&model->graphObj.blueprints, model->normal);

    GraphicsObjectCreateDrawItems(&model->graphObj, true);

    PipelineSetting setting;

    PipelineSettingSetDefault(&model->graphObj, &setting);

    model->graphObj.gItems.settingsCount = 0;

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_model_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_model_vert_spv_size);
        setting.fragShader = &_binary_shaders_model_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_model_frag_spv_size);
        setting.fromFile = 0;
        setting.drawWay = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
    else
        GraphicsObjectSetShadersPath(&model->graphObj, dParam->vertShader, dParam->fragShader);

    ModelAddSettingPipeline(model, setting);

    PipelineCreateGraphics(&model->graphObj, true);

    model->light_enable = false;

}
