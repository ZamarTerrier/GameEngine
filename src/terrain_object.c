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

uint32_t TerrainObjectGetTextureColor(TerrainObject *to, int index){
    return (to->tex_colors[index].w << 24) | (to->tex_colors[index].x << 16) | (to->tex_colors[index].y << 8) | (to->tex_colors[index].z);
}

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

    tb.multi_size = to->t_t_param.texture_scale;
    tb.num_textures = to->t_t_param.num_textures;

    for(int i=0;i < to->t_t_param.num_textures;i++)
    {
        tb.tex_colors[i].x = to->tex_colors[i].x;
        tb.tex_colors[i].y = to->tex_colors[i].y;
        tb.tex_colors[i].z = to->tex_colors[i].z;
        tb.tex_colors[i].w = to->tex_colors[i].w;
    }

    vkMapMemory(e_device, to->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tb), 0, &data);
    memcpy(data, &tb, sizeof(tb));
    vkUnmapMemory(e_device, to->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);


}

void TearrainDefaultDestroy(TerrainObject *to)
{
    GraphicsObjectDestroy(&to->go.graphObj);

}

void TerrainObjectGenerateTerrainTextureMap(TerrainObject *to, void *buffer)
{
    uint32_t size_texture = to->t_t_param.texture_width * to->t_t_param.texture_height;

    uint32_t *some_map = buffer; //[size_texture];

    uint32_t temp = 0;

    int num = 2, iter = 0;

    float t_noise = 0;

    float x_del = (float)to->t_t_param.texture_width / to->width;
    float y_del = (float)to->t_t_param.texture_height / to->height;

    for(int i = 0;i < to->t_t_param.texture_width; i++)
    {
        float n_val_x = (((float)i * to->t_shift) / x_del) / to->t_g_param.size_factor;
        for(int j = 0;j < to->t_t_param.texture_height; j++)
        {
            iter = j * to->t_t_param.texture_width + i;

            float n_val_y = (((float)j * to->t_shift) / y_del) / to->t_g_param.size_factor;

            if(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN)
                t_noise = PerlinOctave2D(to->t_g_param.octaves, n_val_x, n_val_y, to->t_g_param.frequency, to->t_g_param.amplitude);
            else
                t_noise = SimplexOctave2D(to->t_g_param.octaves, n_val_x, n_val_y, to->t_g_param.frequency, to->t_g_param.amplitude);

            if(t_noise >= 0.5f)
                num = 2;
            else if(t_noise > 0.0f)
                num = 1;
            else if(t_noise <= -0.5f)
                num = 0;
            else
                num = 3;

            temp = TerrainObjectGetTextureColor(to, num);

            some_map[iter] = temp;
        }
    }

    TextureUpdate(to->texture_descr, some_map, size_texture * sizeof(uint32_t), 0);
}

void TerrainObjectGenerateTerrainHeights(TerrainObject *to)
{

    vertexParam *vParam = &to->go.graphObj.shape.vParam;

    TerrainVertex *verts = vParam->vertices;

    int iter = 0;

    for(int i = 0;i <= to->width; i++)
    {
        float n_x = ((float)i * to->t_shift)/ to->t_g_param.size_factor;
        for(int j = 0;j <= to->height; j++)
        {
            float n_y = ((float)j * to->t_shift)/ to->t_g_param.size_factor;

            iter = i * (to->width + 1) + j;

            float t_noise;

            if(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN)
                t_noise = PerlinOctave2D( to->t_g_param.octaves, n_x, n_y, to->t_g_param.frequency, to->t_g_param.amplitude);
            else
                t_noise = SimplexOctave2D( to->t_g_param.octaves, n_x, n_y, to->t_g_param.frequency, to->t_g_param.amplitude);

            if(t_noise <= -0.5f)
                 t_noise = -0.5f;

            verts[iter].position.y = t_noise * to->t_g_param.height_factor;;
        }
    }

    BuffersUpdateVertex(vParam);
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

    to->t_t_param = tParam->t_t_param;
    to->t_g_param = tParam->t_g_param;
    to->flags = tParam->flags;
    to->width = tParam->columns;
    to->height = tParam->rows;
    to->t_shift = rand() % UINT16_MAX;
    to->t_shift = to->t_shift / UINT16_MAX;

    GraphicsObjectSetVertexSize(&to->go.graphObj, sizeof(TerrainVertex), sizeof(uint32_t));

    InitTerrain(&to->go.graphObj.shape.vParam, &to->go.graphObj.shape.iParam, tParam);

    BuffersCreateVertex(&to->go.graphObj.shape.vParam);
    BuffersCreateIndex(&to->go.graphObj.shape.iParam);
    BuffersUpdateVertex(&to->go.graphObj.shape.vParam);
    BuffersUpdateIndex(&to->go.graphObj.shape.iParam);

    if((to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS))
        TerrainObjectGenerateTerrainHeights(to);

    to->go.graphObj.local.descrCount = 0;

    BuffersAddUniformObject(&to->go.graphObj.local, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&to->go.graphObj.local, sizeof(TerrainBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);

    to->go.images = calloc(tParam->t_t_param.num_textures + 1, sizeof(GameObjectImage));
    to->go.num_images = tParam->t_t_param.num_textures + 1;

    if(!(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE))
    {
        if(strlen(tParam->texture_map) != 0)
        {
            int len = strlen(tParam->texture_map);
            to->go.images[0].path = calloc(len + 1, sizeof(char));
            memcpy(to->go.images[0].path, tParam->texture_map, len);
            to->go.images[0].path[len] = '\0';
            //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
        }

        TextureImageAdd(&to->go.graphObj.local, &to->go.images[0], 0, 0);
    }

    for(int i=0;i < tParam->t_t_param.num_textures;i++)
    {
        to->tex_colors[i].x = rand() % 255;
        to->tex_colors[i].y = rand() % 255;
        to->tex_colors[i].z = rand() % 255;
        to->tex_colors[i].w = rand() % 255;
    }

    if((to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE))
        to->texture_descr = TextureImageAdd(&to->go.graphObj.local, NULL, to->t_t_param.texture_width, to->t_t_param.texture_height);

    for(int i=0;i < tParam->t_t_param.num_textures;i++)
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
