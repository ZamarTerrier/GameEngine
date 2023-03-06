#include "terrain_object.h"

#include <vulkan/vulkan.h>

#include "camera.h"
#include "graphicsObject.h"

#include "buffers.h"
#include "pipeline.h"
#include "texture.h"

#include "e_math.h"

#include "tools.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void TerrainDefaultUpdate(TerrainObject *to)
{
    if(to->go.graphObj.local.descriptors == NULL)
        return;

    Camera3D* cam = (Camera3D*) cam3D;
    void* data;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    to->go.transform.model = m4_translate_mat(m4_mult(m4_scale_mat(to->go.transform.scale), m4_rotation_matrix(to->go.transform.rotation)), to->go.transform.position);

    mbo.model = to->go.transform.model;
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.01f, MAX_CAMERA_VIEW_DISTANCE);
    mbo.proj.m[1][1] *= -1;

    vkMapMemory(e_device, to->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
    memcpy(data, &mbo, sizeof(mbo));
    vkUnmapMemory(e_device, to->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

    TerrainBuffer tb;

    tb.multi_size = to->texture_scale;
    tb.num_textures = to->num_textures;

    vkMapMemory(e_device, to->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tb), 0, &data);
    memcpy(data, &tb, sizeof(tb));
    vkUnmapMemory(e_device, to->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);


}

void TearrainDefaultDestroy(TerrainObject *to)
{
    GraphicsObjectDestroy(&to->go.graphObj);

}

void TerrainObjectInit(TerrainObject *to, DrawParam *dParam, TerrainParam *tParam)
{
    GameObjectSetUpdateFunc(to, (void *)TerrainDefaultUpdate);
    GameObjectSetDrawFunc(to, (void *)GameObject3DDefaultDraw);
    GameObjectSetCleanFunc(to, (void *)GameObject3DClean);
    GameObjectSetRecreateFunc(to, (void *)GameObject3DRecreate);
    GameObjectSetDestroyFunc(to, (void *)TearrainDefaultDestroy);

    Transform3DInit(&to->go.transform);
    GraphicsObjectInit(&to->go.graphObj, ENGINE_VERTEX_TYPE_TERRAIN);

    to->go.graphObj.gItems.perspective = true;

    to->go.graphObj.shape.linked = false;
    to->go.enable_light = false;
    to->go.wired = false;

    GraphicsObjectSetVertexSize(&to->go.graphObj, sizeof(TerrainVertex), sizeof(uint32_t));

    InitTerrain(&to->go.graphObj.shape.vParam, &to->go.graphObj.shape.iParam, tParam);

    BuffersCreateVertex(&to->go.graphObj.shape.vParam);
    BuffersCreateIndex(&to->go.graphObj.shape.iParam);
    BuffersUpdateVertex(&to->go.graphObj.shape.vParam);
    BuffersUpdateIndex(&to->go.graphObj.shape.iParam);

    to->go.graphObj.local.descrCount = 0;

    BuffersAddUniformObject(&to->go.graphObj.local, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&to->go.graphObj.local, sizeof(TerrainBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);

    to->go.images = calloc(tParam->num_textures + 1, sizeof(GameObjectImage));
    to->go.num_images = tParam->num_textures + 1;
    to->texture_scale = tParam->cell_step;
    to->num_textures = tParam->num_textures;

    if(strlen(tParam->texture_map) != 0)
    {
        int len = strlen(tParam->texture_map);
        to->go.images[0].path = calloc(len + 1, sizeof(char));
        memcpy(to->go.images[0].path, tParam->texture_map, len);
        to->go.images[0].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    TextureImageAdd(&to->go.graphObj.local, &to->go.images[0], 0, 0);

    for(int i=0;i < tParam->num_textures;i++)
    {
        if(strlen(tParam->textures[i]) != 0)
        {
            int len = strlen(tParam->textures[i]);
            to->go.images[i + 1].path = calloc(len + 1, sizeof(char));
            memcpy(to->go.images[i + 1].path, tParam->textures[i], len);
            to->go.images[i + 1].path[len] = '\0';
            //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
        }

        TextureImageAdd(&to->go.graphObj.local, &to->go.images[i + 1], 0, 0);
    }

    GraphicsObjectCreateDrawItems(&to->go.graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    if(dParam != NULL && strlen(dParam->fragShader) > 0 && strlen(dParam->vertShader) > 0)
    {
        GraphicsObjectSetShadersPath(&to->go.graphObj, dParam->vertShader, dParam->fragShader);

        setting.vertShader = dParam->vertShader;
        setting.sizeVertShader = 0;
        setting.fragShader = dParam->fragShader;
        setting.sizeFragShader = 0;
        setting.topology = dParam->topology;
        setting.drawType = dParam->drawType;
        setting.fromFile = 1;
        GameObject3DAddSettingPipeline(to, &setting);

    }else{

        setting.vertShader = &_binary_shaders_terrain_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_terrain_vert_spv_size);
        setting.fragShader = &_binary_shaders_terrain_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_terrain_frag_spv_size);
        setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        setting.drawType = 0;
        setting.fromFile = 0;
        GameObject3DAddSettingPipeline(to, &setting);

    }

    PipelineCreateGraphics(&to->go.graphObj);
}
