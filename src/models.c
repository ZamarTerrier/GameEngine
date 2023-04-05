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
#include "e_resource_descriptors.h"

//Описание вертекса
EIVertexInputBindingDescription ModelObject3DGetBindingDescription() {
    EIVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(ModelVertex3D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

void ModelDefaultDraw(ModelObject3D* mo, void *command){

    for(int i=0; i < mo->num_draw_nodes;i++){
        for(int j=0; j < mo->nodes[i].num_models; j++){

            ModelStruct *model = &mo->nodes[i].models[j];

            for(int l=0; l < model->graphObj.gItems.num_shader_packs;l++)
            {
                BluePrintPack *blue_pack = &model->graphObj.blueprints.blue_print_packs[l];

                if(blue_pack->render_point == current_render)
                {
                    ShaderPack *pack = &model->graphObj.gItems.shader_packs[l];

                    for(int k=0; k < pack->num_pipelines; k++){
                        vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[k].pipeline);

                        PipelineSetting *settings = &blue_pack->settings[k];

                        vertexParam *vParam = &model->graphObj.shapes[settings->vert_indx].vParam;
                        indexParam *iParam = &model->graphObj.shapes[settings->vert_indx].iParam;

                        if(settings->flags & ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW){
                            vkCmdSetViewport(command, 0, 1, &settings->viewport);
                            vkCmdSetScissor(command, 0, 1, &settings->scissor);
                        }

                        VkDeviceSize offsets[] = {0};
                        VkBuffer vertexBuffers[] = {vParam->vertexBuffer};

                        vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);

                        vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[k].layout, 0, 1, &pack->descriptor.descr_sets[imageIndex], 0, NULL);

                        if(settings->flags & ENGINE_PIPELINE_FLAG_DRAW_INDEXED){
                            vkCmdBindIndexBuffer(command, iParam->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                            vkCmdDrawIndexed(command, iParam->indexesSize, 1, 0, 0, 0);
                        }else
                            vkCmdDraw(command, vParam->verticesSize, 1, 0, 0);
                    }
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

            LightSpaceMatrix lsm;
            //mbo.model = edenMat;
            mbo.view = lsm.view = m4_look_at(some_light.position, v3_add(some_light.position, some_light.rotation), cameraUp);
            mbo.proj = lsm.proj = m4_ortho(-ORITO_SIZE, ORITO_SIZE, -ORITO_SIZE, ORITO_SIZE, -MAX_SHADOW_VIEW_DISTANCE, MAX_SHADOW_VIEW_DISTANCE);

            DescriptorUpdate(&mo->nodes[i].models[j].graphObj.blueprints, 1, 1, &lsm, sizeof(lsm));
            DescriptorUpdate(&mo->nodes[i].models[j].graphObj.blueprints, 0, 0, &mbo, sizeof(mbo));

            InvMatrixsBuffer imb = {};
            memset(&imb, 0, sizeof(InvMatrixsBuffer));

            DescriptorUpdate(&mo->nodes[i].models[j].graphObj.blueprints, 1, 2, &lsm, sizeof(lsm));

            LightBuffer3D lbo = {};
            memset(&lbo, 0, sizeof(LightBuffer3D));

            LightObjectFillLights(&lbo, mo->nodes[i].models[j].light_enable);

            DescriptorUpdate(&mo->nodes[i].models[j].graphObj.blueprints, 1, 3, &lbo, sizeof(lbo));
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

    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {

            ModelStruct *model = &mo->nodes[i].models[j];

            for(int l=0; l < model->graphObj.gItems.num_shader_packs;l++)
            {
                BluePrintPack *pack = &model->graphObj.blueprints.blue_print_packs[l];

                PipelineSetting *settings = pack->settings;


                for(int m=0; m < pack->num_settings;m++)
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
                GraphicsObjectCreateDrawItems(&model->graphObj);
                PipelineCreateGraphics(&model->graphObj);
            }
        }
    }
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

void ModelSetShadowDescriptor(ModelStruct *model, void *render)
{
    uint32_t num = model->graphObj.blueprints.num_blue_print_packs;
    model->graphObj.blueprints.blue_print_packs[num].render_point = render;

    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&model->graphObj, &setting);

    setting.vertShader = &_binary_shaders_depth_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_depth_vert_spv_size);
    setting.fragShader = &_binary_shaders_depth_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_depth_frag_spv_size);
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    setting.fromFile = 0;
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW);
    setting.vert_indx = 1;
    setting.cull_mode = VK_CULL_MODE_FRONT_BIT;

    ModelAddSettingPipeline(model, 0, setting);

    model->graphObj.blueprints.num_blue_print_packs ++;
}

void ModelSetDefaultDescriptor(ModelStruct *model, void *render, void *shadow)
{
    uint32_t num = model->graphObj.blueprints.num_blue_print_packs;
    model->graphObj.blueprints.blue_print_packs[num].render_point = render;

    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(LightSpaceMatrix), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(InvMatrixsBuffer), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT);

    BluePrintAddRenderImage(&model->graphObj.blueprints, num, shadow);
    BluePrintAddTextureImage(&model->graphObj.blueprints, num, model->diffuse);
    //TextureImageAdd(&model->graphObj.blueprints, model->specular);
    BluePrintAddTextureImage(&model->graphObj.blueprints, num, model->normal);

    PipelineSetting setting;

    PipelineSettingSetDefault(&model->graphObj, &setting);

    setting.vertShader = &_binary_shaders_model_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_model_vert_spv_size);
    setting.fragShader = &_binary_shaders_model_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_model_frag_spv_size);
    setting.fromFile = 0;
    setting.vert_indx = 0;

    ModelAddSettingPipeline(model, num, setting);

    model->graphObj.blueprints.num_blue_print_packs ++;
}

void ModelDefaultInit(ModelStruct *model, DrawParam *dParam){

    ModelSetShadowDescriptor(model, dParam->shadow);

    ModelSetDefaultDescriptor(model, dParam->render, dParam->shadow);

    {

        GraphicsObjectSetVertexSize(&model->graphObj, 1, sizeof(Vertex3D), sizeof(uint32_t));

        model->graphObj.shapes[1].bindingDescription = &Bind3DDescription;
        model->graphObj.shapes[1].attr = cubeAttributeDescription;
        model->graphObj.shapes[1].countAttr = 3;

        model->graphObj.shapes[1].iParam.indices = calloc(model->graphObj.shapes[0].iParam.indexesSize, sizeof(uint32_t));
        model->graphObj.shapes[1].iParam.indexesSize = model->graphObj.shapes[0].iParam.indexesSize;
        memcpy(model->graphObj.shapes[1].iParam.indices, model->graphObj.shapes[0].iParam.indices, sizeof(uint32_t) * model->graphObj.shapes[0].iParam.indexesSize);

        model->graphObj.shapes[1].vParam.vertices = calloc(model->graphObj.shapes[0].vParam.verticesSize, sizeof(Vertex3D));
        model->graphObj.shapes[1].vParam.verticesSize = model->graphObj.shapes[0].vParam.verticesSize;

        ModelVertex3D *m_verts = model->graphObj.shapes[0].vParam.vertices;
        Vertex3D *verts = model->graphObj.shapes[1].vParam.vertices;

        for(int i=0;i < model->graphObj.shapes[0].vParam.verticesSize;i++)
        {
            verts[i].position = m_verts[i].position;
            verts[i].normal = m_verts[i].normal;
            verts[i].texCoord = m_verts[i].texCoord;
        }

        BuffersCreateVertex(&model->graphObj.shapes[1].vParam);
        BuffersCreateIndex(&model->graphObj.shapes[1].iParam);
        BuffersUpdateVertex(&model->graphObj.shapes[1].vParam);
        BuffersUpdateIndex(&model->graphObj.shapes[1].iParam);

        model->graphObj.shapes[1].init = true;

        model->graphObj.num_shapes ++;
    }

    GraphicsObjectCreateDrawItems(&model->graphObj);

    PipelineCreateGraphics(&model->graphObj);

    model->light_enable = false;

}
