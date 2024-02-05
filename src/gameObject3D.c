#include "gameObject3D.h"

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "e_camera.h"

#include "lightObject.h"
#include "render_texture.h"

#include "pipeline.h"
#include "e_buffer.h"
#include "e_texture.h"

#include "e_math.h"
#include "e_tools.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

#define VERTEX_BUFFER_BIND_ID 0
#define INSTANCE_BUFFER_BIND_ID 1

typedef void (*Update_Descriptor3D)(GameObject3D* go, BluePrintDescriptor *descriptor);

void GameObject3DDescriptorModelUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;

    RenderTexture *render = current_render;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    go->transform.model = m4_transform(go->transform.position, go->transform.scale, go->transform.rotation);

    mbo.model = go->transform.model;
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(render->width, render->height, render->persp_view_angle, render->persp_view_near, render->persp_view_distance);
    mbo.proj.m[1][1] *= -1;

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
}

void GameObject3DDirLightModelUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    RenderTexture *render = current_render;

    DirLightBuffer dlb = {};
    memset(&dlb, 0, sizeof(DirLightBuffer));

    LightObjectFillDirLights(&dlb);

    //uint32_t layer_indx = descriptor->indx_layer;

    mbo.model = m4_transform(go->transform.position, go->transform.scale, go->transform.rotation);
    mbo.view =  m4_look_at( dlb.dir[0].position, v3_add( dlb.dir[0].position, dlb.dir[0].direction ), cameraUp);

    if(render->flags & ENGINE_RENDER_FLAG_PERSPECTIVE){
        mbo.proj = m4_perspective(render->width, render->height, render->persp_view_angle, render->persp_view_near, render->persp_view_distance);
        mbo.proj.m[1][1] *= -1;
    }else if(render->flags & ENGINE_RENDER_FLAG_FRUSTRUM){
        mbo.proj = m4_frustum(-render->frust_side, render->frust_side, -render->frust_side, render->frust_side, render->frust_near, render->frust_far);
        mbo.proj.m[1][1] *= -1;
    }else
        mbo.proj = m4_ortho(-render->ortg_view_size, render->ortg_view_size, -render->ortg_view_size, render->ortg_view_size, -render->ortg_view_distance, render->ortg_view_distance);

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
}

void GameObject3DOmniLightModelUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    RenderTexture *render = current_render;

    PointLightBuffer plb = {};
    memset(&plb, 0, sizeof(PointLightBuffer));

    LightObjectFillPointLights(&plb);

    uint32_t layer_indx = descriptor->indx_layer;

    mbo.model = m4_transform(go->transform.position, go->transform.scale, go->transform.rotation);
    mbo.view = m4_look_at(plb.points[layer_indx].position, vec3_f(0, 0, 0), cameraUp);

    if(render->flags & ENGINE_RENDER_FLAG_PERSPECTIVE){
        mbo.proj = m4_perspective(render->width, render->height, render->persp_view_angle, render->persp_view_near, render->persp_view_distance);
        mbo.proj.m[1][1] *= -1;
    }else if(render->flags & ENGINE_RENDER_FLAG_FRUSTRUM){
        mbo.proj = m4_frustum(-render->frust_side, render->frust_side, -render->frust_side, render->frust_side, render->frust_near, render->frust_far);
        mbo.proj.m[1][1] *= -1;
    }else
        mbo.proj = m4_ortho(-render->ortg_view_size, render->ortg_view_size, -render->ortg_view_size, render->ortg_view_size, -render->ortg_view_distance, render->ortg_view_distance);

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
}

void GameObject3DSpotLightModelUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    RenderTexture *render = current_render;

    SpotLightBuffer slb = {};
    memset(&slb, 0, sizeof(SpotLightBuffer));

    LightObjectFillSpotLights(&slb);

    uint32_t layer_indx = descriptor->indx_layer;

    mbo.model = m4_transform(go->transform.position, go->transform.scale, go->transform.rotation);
    mbo.view = m4_look_at(slb.spots[layer_indx].position, v3_add(slb.spots[layer_indx].position, slb.spots[layer_indx].direction), cameraUp);

    if(render->flags & ENGINE_RENDER_FLAG_PERSPECTIVE){
        mbo.proj = m4_perspective(render->width, render->height, render->persp_view_angle, render->persp_view_near, render->persp_view_distance);
        mbo.proj.m[1][1] *= -1;
    }else if(render->flags & ENGINE_RENDER_FLAG_FRUSTRUM){
        mbo.proj = m4_frustum(-render->frust_side, render->frust_side, -render->frust_side, render->frust_side, render->frust_near, render->frust_far);
        mbo.proj.m[1][1] *= -1;
    }else
        mbo.proj = m4_ortho(-render->ortg_view_size, render->ortg_view_size, -render->ortg_view_size, render->ortg_view_size, -render->ortg_view_distance, render->ortg_view_distance);

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
}

void GameObject3DDescriptorDirLightsUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    RenderTexture *render = current_render;

    DirLightBuffer dlb = {};
    memset(&dlb, 0, sizeof(DirLightBuffer));

    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    LightObjectFillDirLights(&dlb);

    if(dir_shadow_array != NULL)
    {
        RenderTexture **renders = dir_shadow_array;

        for(int i=0;i < num_dir_shadows;i++)
        {
            dlb.mats[i].view = m4_look_at( dlb.dir[0].position, v3_add( dlb.dir[0].position, dlb.dir[0].direction), renders[i]->up_vector);

            if(renders[i]->flags & ENGINE_RENDER_FLAG_PERSPECTIVE){
                dlb.mats[i].proj = m4_perspective(renders[i]->width, renders[i]->height, renders[i]->persp_view_angle, renders[i]->persp_view_near, renders[i]->persp_view_distance);
                dlb.mats[i].proj.m[1][1] *= -1;
            }else if(renders[i]->flags & ENGINE_RENDER_FLAG_FRUSTRUM){
                dlb.mats[i].proj = m4_frustum(-renders[i]->frust_side, renders[i]->frust_side, -renders[i]->frust_side, renders[i]->frust_side, renders[i]->frust_near, renders[i]->frust_far);
                dlb.mats[i].proj.m[1][1] *= -1;
            }else
                dlb.mats[i].proj = m4_ortho(-renders[i]->ortg_view_size, renders[i]->ortg_view_size, -renders[i]->ortg_view_size, renders[i]->ortg_view_size, -renders[i]->ortg_view_distance, renders[i]->ortg_view_distance);
        }

        dlb.cascadeSplits.x = renders[3]->cascadeSplit;
        dlb.cascadeSplits.y = renders[2]->cascadeSplit;
        dlb.cascadeSplits.z = renders[1]->cascadeSplit;
        dlb.cascadeSplits.w = renders[0]->cascadeSplit;
    }

    DescriptorUpdate(descriptor, &dlb, sizeof(dlb));
}

void GameObject3DDescriptorPointLightsUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;

    PointLightBuffer plb = {};
    memset(&plb, 0, sizeof(PointLightBuffer));

    LightObjectFillPointLights(&plb);

    if(num_point_shadows > 0)
        for(int i=0;i < num_point_shadows;i++){
            plb.pos[i].light_pos = plb.points[i].position;
            plb.pos[i].view_pos = cam->position;
        }

    DescriptorUpdate(descriptor, &plb, sizeof(plb));
}

void GameObject3DDescriptorSpotLightsUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    SpotLightBuffer slb = {};
    memset(&slb, 0, sizeof(SpotLightBuffer));

    LightObjectFillSpotLights(&slb);

    if(spot_shadow_array != NULL)
    {

        RenderTexture **renders = spot_shadow_array;

        for(int i=0;i < num_spot_shadows;i++)
        {
            RenderTexture *spot = renders[i];

            slb.mats[i].view = m4_look_at(slb.spots[i].position, v3_add(slb.spots[i].position, slb.spots[i].direction), spot->up_vector);

            if(spot->flags & ENGINE_RENDER_FLAG_PERSPECTIVE){
                slb.mats[i].proj = m4_perspective(spot->width, spot->height, spot->persp_view_angle, spot->persp_view_near, spot->persp_view_distance);
                slb.mats[i].proj.m[1][1] *= -1;
            }else if(spot->flags & ENGINE_RENDER_FLAG_FRUSTRUM){
                slb.mats[i].proj = m4_frustum(-spot->frust_side, spot->frust_side, -spot->frust_side, spot->frust_side, spot->frust_near, spot->frust_far);
                slb.mats[i].proj.m[1][1] *= -1;
            }else
                slb.mats[i].proj = m4_ortho(-spot->ortg_view_size, spot->ortg_view_size, -spot->ortg_view_size, spot->ortg_view_size, -spot->ortg_view_distance, spot->ortg_view_distance);
        }
    }

    DescriptorUpdate(descriptor, &slb, sizeof(slb));
}

void GameObject3DLigtStatusBufferUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    LightStatusBuffer lsb;
    memset(&lsb, 0, sizeof(LightStatusBuffer));

    LightObjectFillLightStatus(&lsb);

    lsb.isEnable = (go->self.flags & ENGINE_GAME_OBJECT_FLAG_LIGHT);

    DescriptorUpdate(descriptor, &lsb, sizeof(lsb));
}

void GameObject3DLightPosUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    LightPosBuff lpb;

    PointLightBuffer plb = {};
    memset(&plb, 0, sizeof(PointLightBuffer));

    LightObjectFillPointLights(&plb);

    uint32_t layer_indx = descriptor->indx_layer;

    lpb.light_pos = plb.points[layer_indx].position;
    lpb.view_pos = Camera3DGetPosition();

    DescriptorUpdate(descriptor, &lpb, sizeof(lpb));
}

void GameObject3DSDFBufferUpdate(GameObject3D* go, BluePrintDescriptor *descriptor)
{
    SDFBuffer sdfb;

    RenderTexture *render = current_render;

    double time = glfwGetTime();

    sdfb.cam_pos = Camera3DGetPosition();
    sdfb.cam_rot = Camera3DGetRotation();

    sdfb.obj_pos = go->transform.position;
    sdfb.obj_rot = go->transform.rotation;

    sdfb.view = m4_look_at( sdfb.cam_pos, v3_add( sdfb.cam_pos, sdfb.cam_rot), vec3_f( 0, 1, 0));

    DescriptorUpdate(descriptor, &sdfb, sizeof(sdfb));
}

void GameObject3DDefaultUpdate(GameObject3D* go) {

    for(int i=0; i < go->graphObj.gItems.num_shader_packs;i++)
    {
        BluePrintPack *pack = &go->graphObj.blueprints.blue_print_packs[i];

        if(pack->render_point == current_render)
        {
            for(int j=0;j < pack->num_descriptors;j++)
            {
                BluePrintDescriptor *descriptor = &pack->descriptors[j];

                if(descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                {
                    Update_Descriptor3D update = descriptor->update;
                    update(go, descriptor);
                }
            }
        }
    }
}

void GameObject3DDefaultDraw(GameObject3D* go, void *command){

    for(int i=0; i < go->graphObj.gItems.num_shader_packs;i++)
    {
        BluePrintPack *blue_pack = &go->graphObj.blueprints.blue_print_packs[i];

        if(blue_pack->render_point == current_render)
        {
            RenderTexture *render = current_render;

            ShaderPack *pack = &go->graphObj.gItems.shader_packs[i];

            for(int j=0; j < pack->num_pipelines; j++){

                if(render->type == ENGINE_RENDER_TYPE_CUBEMAP){

                    mat4 res = MakeLookRender(render->currFrame, blue_pack->descriptors[j].indx_layer);

                    vkCmdPushConstants( command, pack->pipelines[j].layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), &res);
                }

                PipelineSetting *settings = &blue_pack->settings[j];

                vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].pipeline);

                if(settings->flags & ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW){
                    vkCmdSetViewport(command, 0, 1, &settings->viewport);
                    vkCmdSetScissor(command, 0, 1, &settings->scissor);
                }

                vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].layout, 0, 1, &pack->descriptor.descr_sets[imageIndex], 0, NULL);

                uint32_t num_instances = 1;
                uint32_t num_verts = go->graphObj.shapes[settings->vert_indx].vParam.verticesSize;
                if(num_verts > 0)
                {
                    VkBuffer vertexBuffers[] = {go->graphObj.shapes[settings->vert_indx].vParam.vertexBuffer};
                    VkDeviceSize offsets[] = {0};

                    // Binding point 0 : Mesh vertex buffer
                    vkCmdBindVertexBuffers(command, VERTEX_BUFFER_BIND_ID, 1, vertexBuffers, offsets);

                    if(go->num_instances > 0){
                        // Binding point 1 : Instance data buffer
                        vkCmdBindVertexBuffers(command, INSTANCE_BUFFER_BIND_ID, 1, &go->buffer.buffer, offsets);
                        num_instances = go->num_instances;
                    }
                }

                if(num_instances == 0)
                    continue;

                if(settings->flags & ENGINE_PIPELINE_FLAG_DRAW_INDEXED && go->graphObj.shapes[settings->vert_indx].iParam.indexesSize > 0){
                    vkCmdBindIndexBuffer(command, go->graphObj.shapes[settings->vert_indx].iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(command, go->graphObj.shapes[settings->vert_indx].iParam.indexesSize, num_instances, 0, 0, 0);
                }else
                    vkCmdDraw(command, go->graphObj.shapes[settings->vert_indx].vParam.verticesSize, num_instances, 0, 0);
            }
        }
    }
}

void GameObject3DInitDraw(GameObject3D *go)
{
    GraphicsObjectCreateDrawItems(&go->graphObj);

    PipelineCreateGraphics(&go->graphObj);
}

void GameObject3DAddShadowDescriptor(GameObject3D *go, uint32_t type, void *render, uint32_t layer_indx)
{
    uint32_t nums = go->graphObj.blueprints.num_blue_print_packs;
    go->graphObj.blueprints.blue_print_packs[nums].render_point = render;

    if(type == ENGINE_LIGHT_TYPE_DIRECTIONAL)
        BluePrintAddUniformObject(&go->graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDirLightModelUpdate, layer_indx);
    else
        BluePrintAddUniformObject(&go->graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DSpotLightModelUpdate, layer_indx);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_depth_vert_spv_start, (size_t)(&_binary_shaders_depth_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_depth_frag_spv_start, (size_t)(&_binary_shaders_depth_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW);
    //setting.flags |= ENGINE_PIPELINE_FLAG_BIAS;
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_FRONT_BIT;

    GameObject3DAddSettingPipeline(go, nums, &setting);

    go->graphObj.blueprints.num_blue_print_packs ++;
}

void GameObject3DAddOmiShadow(GameObject3D *go, void *render, uint32_t layer_indx)
{
    uint32_t num = go->graphObj.blueprints.num_blue_print_packs;
    go->graphObj.blueprints.blue_print_packs[num].render_point = render;


    BluePrintAddUniformObject(&go->graphObj.blueprints, num, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DOmniLightModelUpdate, layer_indx);
    BluePrintAddUniformObject(&go->graphObj.blueprints, num, sizeof(LightPosBuff), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DLightPosUpdate, layer_indx);

    BluePrintAddPushConstant(&go->graphObj.blueprints, num, sizeof(mat4), VK_SHADER_STAGE_VERTEX_BIT, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_depth_vert_omni_spv_start, (size_t)(&_binary_shaders_depth_vert_omni_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_depth_frag_omni_spv_start, (size_t)(&_binary_shaders_depth_frag_omni_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    setting.fromFile = 0;
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW);
    //setting.flags &= ~(ENGINE_PIPELINE_FLAG_ALPHA);
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_NONE;

    GameObject3DAddSettingPipeline(go, num, &setting);

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

    if(go->num_instances > 0)
        BuffersDestroyBuffer(go->buffer.buffer);
}

void GameObject3DInitTextures(GameObject3D *go, DrawParam *dParam)
{
    go->images = calloc(3, sizeof(GameObjectImage));

    if(dParam == NULL)
        return;

    if(strlen(dParam->diffuse) != 0)
    {
        int len = strlen(dParam->diffuse);
        go->images[0].path = calloc(len + 1, sizeof(char));
        memcpy(go->images[0].path, dParam->diffuse, len);
        go->images[0].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    if(strlen(dParam->normal) != 0)
    {
        int len = strlen(dParam->normal);
        go->images[1].path = calloc(len + 1, sizeof(char));
        memcpy(go->images[1].path, dParam->normal, len);
        go->images[1].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    if(strlen(dParam->specular) != 0)
    {
        int len = strlen(dParam->specular);
        go->images[2].path = calloc(len + 1, sizeof(char));
        memcpy(go->images[2].path, dParam->specular, len);
        go->images[2].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
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
    go->num_instances = 0;

    GraphicsObjectInit(&go->graphObj, ENGINE_VERTEX_TYPE_3D_OBJECT);

    go->graphObj.gItems.perspective = true;

    go->self.flags = 0;
}

void GameObject3DAddInstance(GameObject3D *go, VertexInstance3D vertex){

    if(go->num_instances + 1 >= MAX_INSTANCES){
        printf("Слишком много инстансов!\n");
        return;
    }

    go->instances[go->num_instances] = vertex;

    go->num_instances ++;
}

void GameObject3DSetInstance(GameObject3D *go, uint32_t indx, VertexInstance3D vertex){
    go->instances[indx] = vertex;
}

void GameObject3DRemoveInstance(GameObject3D *go, uint32_t indx){

    if(go->num_instances <= indx)
        return;

    VertexInstance3D instances[MAX_INSTANCES];
    memcpy(instances, go->instances, sizeof(VertexInstance3D) * MAX_INSTANCES);

    memset(go->instances, 0, sizeof(VertexInstance3D) * MAX_INSTANCES);

    go->num_instances --;

    uint32_t iter = 0;
    for(int i=0;i < go->num_instances;i++)
    {
        if(i != indx){
            go->instances[iter] = instances[i];
            iter++;
        }
    }
}

void GameObject3DInitInstances(GameObject3D *go){

    VkDeviceSize bufferSize;

    uint16_t num_verts = go->graphObj.shapes[0].vParam.verticesSize;
    GraphicsObjectInit(&go->graphObj, ENGINE_VERTEX_TYPE_3D_INSTANCE);

    num_verts = go->graphObj.shapes[0].vParam.verticesSize;
    memset(go->instances, 0, sizeof(VertexInstance3D) * MAX_INSTANCES);

    bufferSize = sizeof(VertexInstance3D) * MAX_INSTANCES;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &go->buffer.buffer, &go->buffer.buffer_memory, ENGINE_BUFFER_ALLOCATE_VERTEX);

}

void GameObject3DUpdateInstances(GameObject3D *go){

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDeviceSize bufferSize;

    bufferSize = sizeof(VertexInstance3D) * MAX_INSTANCES;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory, ENGINE_BUFFER_ALLOCATE_STAGING);

    //Изменение памяти
    void* data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memset(data, 0, bufferSize);
    memcpy(data, go->instances, (size_t) go->num_instances * sizeof(VertexInstance3D));
    vkUnmapMemory(e_device, stagingBufferMemory);

    //-------------

    BuffersCopy(stagingBuffer, go->buffer.buffer, bufferSize);

    BuffersDestroyBuffer(stagingBuffer);

}

void GameObject3DInitCopy(GameObject3D *to, GameObject3D *from)
{
    to->self = from->self;

    Transform3DInit(&to->transform);
    GraphicsObjectInit(&to->graphObj, from->graphObj.shapes[0].type);

    to->graphObj.gItems.perspective = true;

    to->self.flags = 0;

    memcpy(to->graphObj.shapes, from->graphObj.shapes, sizeof(Shape) * MAX_SHAPES);

    Blueprints *prints_to = &to->graphObj.blueprints;
    Blueprints *prints_from = &from->graphObj.blueprints;
    for(int i=0;i < prints_from->num_blue_print_packs;i++)
    {
        prints_to->blue_print_packs[i].render_point = prints_from->blue_print_packs[i].render_point;

        for(int j=0;j < prints_from->blue_print_packs[i].num_descriptors;j++)
        {
            BluePrintDescriptor *descriptor = &prints_from->blue_print_packs[i].descriptors[j];
            if(descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                BluePrintAddUniformObject(&to->graphObj.blueprints, i, descriptor->buffsize, descriptor->stageflag, descriptor->update, descriptor->indx_layer);
            else
                BluePrintAddTextureImage(&to->graphObj.blueprints, i, &descriptor->image, descriptor->stageflag);
        }

        for(int j=0;j < prints_from->blue_print_packs[i].num_settings;j++)
        {
            GameObject3DAddSettingPipeline(to, i, &prints_from->blue_print_packs[i].settings[j]);
        }

        prints_to->num_blue_print_packs ++;
    }

    GameObject3DInitDraw(to);
}

void GameObject3DEnableLight(GameObject3D *go, bool enable)
{
    if(enable)
        go->self.flags |= ENGINE_GAME_OBJECT_FLAG_LIGHT;
    else
        go->self.flags &= ~(ENGINE_GAME_OBJECT_FLAG_LIGHT);
}
