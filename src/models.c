#include "models.h"

#include <vulkan/vulkan.h>

#include "camera.h"
#include "gameObject.h"
#include "lightObject.h"

#include "buffers.h"
#include "texture.h"

#include "math.h"
#include "e_math.h"

#include "pipeline.h"

#include "render_texture.h"

#include "glTFLoader.h"
#include "objLoader.h"
#include "fbxLoader.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"
#include "e_resource_descriptors.h"

typedef void (*Update_Func)(ModelObject3D* mo, ModelNode *node, BluePrintDescriptor *descriptor);

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
                    RenderTexture *render = current_render;

                    ShaderPack *pack = &model->graphObj.gItems.shader_packs[l];


                    for(int k=0; k < pack->num_pipelines; k++){

                        if(render->type == ENGINE_RENDER_TYPE_CUBEMAP)
                            vkCmdPushConstants( command, pack->pipelines[k].layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), &render->view);

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

void ModelModelBufferUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;

    glTFStruct *gltf = mo->obj;

    RenderTexture *render = current_render;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    engine_gltf_node *node = &gltf->nodes[mo->nodes[indx_node].id_node];

    mo->nodes[indx_node].model = mat4_mult_transform(node->global_matrix, m4_transform(mo->transform.position, mo->transform.scale, mo->transform.rotation));


    mbo.model = mo->nodes[indx_node].model;
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(cam->view_angle, cam->view_near, cam->view_distance);
    mbo.proj.m[1][1] *= -1;

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
}

void ModelLightModelUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor)
{
    glTFStruct *gltf = mo->obj;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    RenderTexture *render = current_render;

    engine_gltf_node *node = &gltf->nodes[mo->nodes[indx_node].id_node];

    mo->nodes[indx_node].model = mat4_mult_transform(node->global_matrix, m4_transform(mo->transform.position, mo->transform.scale, mo->transform.rotation));

    mbo.model = mo->nodes[indx_node].model;

    mbo.view = m4_look_at(render->position, v3_add(render->position, render->rotation), render->up_vector);

    if((render->flags & ENGINE_RENDER_FLAG_PERSPECTIVE))
        mbo.proj = m4_perspective(render->persp_view_angle, render->persp_view_near, render->persp_view_distance);
    else
        mbo.proj = m4_ortho(-render->ortg_view_size, render->ortg_view_size, -render->ortg_view_size, render->ortg_view_size, -render->ortg_view_distance, render->ortg_view_distance);

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
}

void ModelLightModelUpdate2(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor)
{
    glTFStruct *gltf = mo->obj;

    Camera3D* cam = (Camera3D*) cam3D;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    RenderTexture *render = current_render;

    engine_gltf_node *node = &gltf->nodes[mo->nodes[indx_node].id_node];

    mo->nodes[indx_node].model = mat4_mult_transform(node->global_matrix, m4_transform(mo->transform.position, mo->transform.scale, mo->transform.rotation));

    mbo.model = mo->nodes[indx_node].model;
    mbo.view = m4_look_at(cam->position, v3_add(render->position, render->rotation), cameraUp);

    if((render->flags & ENGINE_RENDER_FLAG_PERSPECTIVE))
        mbo.proj = m4_perspective(render->persp_view_angle, render->persp_view_near, render->persp_view_distance);
    else
        mbo.proj = m4_ortho(-render->ortg_view_size, render->ortg_view_size, -render->ortg_view_size, render->ortg_view_size, -render->ortg_view_distance, render->ortg_view_distance);

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
}

void ModelLightMatrixUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor)
{
    RenderTexture *render = current_render;

    LightSpaceMatrix lsm;
    memset(&lsm, 0, sizeof(LightSpaceMatrix));

    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    RenderTexture **renders = shadow_array;

    for(int i=0;i < num_shadows;i++)
    {
        lsm.mats[i].view = m4_look_at(renders[i]->position, v3_add(renders[i]->position, renders[i]->rotation), renders[i]->up_vector);

        if((render->flags & ENGINE_RENDER_FLAG_PERSPECTIVE))
           lsm.mats[i].proj = m4_perspective(renders[i]->persp_view_angle, renders[i]->persp_view_near, renders[i]->persp_view_distance);
        else
            lsm.mats[i].proj = m4_ortho(-renders[i]->ortg_view_size, renders[i]->ortg_view_size, -renders[i]->ortg_view_size, renders[i]->ortg_view_size, -renders[i]->ortg_view_distance, renders[i]->ortg_view_distance);

        lsm.cascadeSplits[i] = renders[i]->cascadeSplit;
    }

    DescriptorUpdate(descriptor, &lsm, sizeof(lsm));
}

void ModelInvMatrixBuffer(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor)
{
    glTFStruct *glTF = mo->obj;

    InvMatrixsBuffer imb = {};
    memset(&imb, 0, sizeof(InvMatrixsBuffer));

    for(int k=0;k < glTF->num_join_mats;k++)
      imb.mats[k] = glTF->joint_mats[k].join_mat;

    imb.size = glTF->num_join_mats;

    DescriptorUpdate(descriptor, &imb, sizeof(imb));

    //Временно
    /*
    InvMatrixsBuffer imb = {};
    memset(&imb, 0, sizeof(InvMatrixsBuffer));

    DescriptorUpdate(descriptor, &imb, sizeof(imb));*/
}

void ModelDescriptorLightObjectsUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor)
{
    LightBuffer3D lbo = {};
    memset(&lbo, 0, sizeof(LightBuffer3D));

    LightObjectFillLights(&lbo, (mo->self.flags & ENGINE_GAME_OBJECT_FLAG_LIGHT));

    DescriptorUpdate(descriptor, &lbo, sizeof(lbo));
}

void ModelDescriptorLightPosUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;

    LightPosBuff lpb;

    lpb.light_pos = (vec3){ 0, 0, 0};
    lpb.view_pos = cam->position;

    DescriptorUpdate(descriptor, &lpb, sizeof(lpb));
}

void ModelDefaultUpdate(ModelObject3D *mo)
{
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            ModelStruct *obj = &mo->nodes[i].models[j];

            for(int l=0;l < obj->graphObj.blueprints.num_blue_print_packs;l++)
            {
                BluePrintPack *pack = &obj->graphObj.blueprints.blue_print_packs[l];

                if(pack->render_point == current_render)
                {
                    for(int k=0;k < pack->num_descriptors;k++)
                    {
                        BluePrintDescriptor *descriptor = &pack->descriptors[k];

                        if(descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                        {
                            Update_Func update = descriptor->update;
                            update(mo, i, descriptor);
                        }
                    }
                }

            }
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

    if(enable)
        mo->self.flags |= ENGINE_GAME_OBJECT_FLAG_LIGHT;
    else
        mo->self.flags &= ~(ENGINE_GAME_OBJECT_FLAG_LIGHT);
}

void ModelSetSelCameraEnable(void *obj, bool enable)
{
    ModelObject3D *mo = (ModelObject3D *)obj;

    if(enable)
        mo->self.flags |= ENGINE_GAME_OBJECT_FLAG_SELF_CAMERA;
    else
        mo->self.flags &= ~(ENGINE_GAME_OBJECT_FLAG_SELF_CAMERA);
}

void ModelSetShadowDescriptor(ModelStruct *model, void *render)
{
    uint32_t num = model->graphObj.blueprints.num_blue_print_packs;
    model->graphObj.blueprints.blue_print_packs[num].render_point = render;

    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)ModelLightModelUpdate);

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

    ModelAddSettingPipeline(model, num, setting);

    model->graphObj.blueprints.num_blue_print_packs ++;
}

void ModelSetOmniShadowDescriptor(ModelStruct *model, void *render)
{
    uint32_t num = model->graphObj.blueprints.num_blue_print_packs;
    model->graphObj.blueprints.blue_print_packs[num].render_point = render;

    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)ModelLightModelUpdate2);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(LightPosBuff), VK_SHADER_STAGE_VERTEX_BIT, (void *)ModelDescriptorLightPosUpdate);

    BluePrintAddPushConstant(&model->graphObj.blueprints, num, sizeof(mat4), VK_SHADER_STAGE_VERTEX_BIT, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&model->graphObj, &setting);

    setting.vertShader = &_binary_shaders_depth_vert_omni_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_depth_vert_omni_spv_size);
    setting.fragShader = &_binary_shaders_depth_frag_omni_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_depth_frag_omni_spv_size);
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    setting.fromFile = 0;
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW);
    //setting.flags &= ~(ENGINE_PIPELINE_FLAG_ALPHA);
    setting.vert_indx = 1;
    setting.cull_mode = VK_CULL_MODE_NONE;

    ModelAddSettingPipeline(model, num, setting);

    model->graphObj.blueprints.num_blue_print_packs ++;
}

void ModelSetDefaultDescriptor(ModelStruct *model, void *render, void *shadow, void *cube_shadow)
{
    uint32_t num = model->graphObj.blueprints.num_blue_print_packs;
    model->graphObj.blueprints.blue_print_packs[num].render_point = render;

    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)ModelModelBufferUpdate);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(InvMatrixsBuffer), VK_SHADER_STAGE_VERTEX_BIT, (void *)ModelInvMatrixBuffer);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(LightPosBuff), VK_SHADER_STAGE_VERTEX_BIT, (void *)ModelDescriptorLightPosUpdate);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)ModelDescriptorLightObjectsUpdate);
    BluePrintAddUniformObject(&model->graphObj.blueprints, num, sizeof(LightSpaceMatrix), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)ModelLightMatrixUpdate);

    shadow_array = calloc(num_shadows, sizeof(RenderTexture *));

    RenderTexture **array = shadow_array;

    RenderTexture *renders = shadow;

    for(int i=0;i < num_shadows;i++)
        array[i] = &renders[i];

    BluePrintAddRenderImageArray(&model->graphObj.blueprints, num, array, num_shadows);
    BluePrintAddRenderImage(&model->graphObj.blueprints, num, cube_shadow);

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

    num_shadows = dParam->num_shadow;

    RenderTexture *renders = dParam->shadow;

    for(int i=0;i < num_shadows;i++)
        ModelSetShadowDescriptor(model, &renders[i]);

    ModelSetOmniShadowDescriptor(model, dParam->cube_shadow);

    ModelSetDefaultDescriptor(model, dParam->render, dParam->shadow, dParam->cube_shadow);

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

}
