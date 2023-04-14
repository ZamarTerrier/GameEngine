#include "terrain_object.h"

#include <vulkan/vulkan.h>

#include "engine.h"

#include "camera.h"
#include "graphicsObject.h"
#include "lightObject.h"
#include "render_texture.h"

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

void TerrainObjectMakeDefaultParams(TerrainParam *tParam, uint32_t texture_width, uint32_t texture_height)
{
    tParam->columns = 500;
    tParam->rows = 500;
    tParam->t_g_param.size_factor = 300;
    tParam->t_g_param.height_factor = 60;
    tParam->t_g_param.frequency = 1;
    tParam->t_g_param.amplitude = 1;
    tParam->t_g_param.octaves = 6;
    tParam->vertex_step = 1.0;
    tParam->t_t_param.texture_scale = 80;
    tParam->t_t_param.texture_width = texture_width;
    tParam->t_t_param.texture_height = texture_height;
    tParam->t_t_param.num_textures = 0;
    tParam->flags = ENGINE_TERRIAN_FLAGS_REPEATE_TEXTURE | ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE | ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS ;//| ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN;
}

void TerrainObjectDescriptorTextureUpdate(TerrainObject *to, BluePrintDescriptor *descriptor)
{
    TextureBuffer tb;

    tb.multi_size = to->t_t_param.texture_scale;
    tb.num_textures = to->t_t_param.num_textures;

    for(int i=0;i < to->t_t_param.num_textures;i++)
    {
        tb.tex_colors[i].x = to->tex_colors[i].x;
        tb.tex_colors[i].y = to->tex_colors[i].y;
        tb.tex_colors[i].z = to->tex_colors[i].z;
        tb.tex_colors[i].w = to->tex_colors[i].w;
    }

    DescriptorUpdate(descriptor, &tb, sizeof(tb));
}

void TearrainDefaultDestroy(TerrainObject *to)
{
    free(to->height_map);

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
        float n_val_x = ((float)i * to->t_shift) / to->t_g_param.size_factor / x_del;

        for(int j = 0;j < to->t_t_param.texture_height; j++)
        {
            float n_val_y = ((float)j * to->t_shift) / to->t_g_param.size_factor / y_del;

            iter = j * to->t_t_param.texture_width + i;

            if(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN)
                t_noise = PerlinOctave2D(to->t_g_param.octaves, n_val_x, n_val_y, to->t_g_param.frequency, to->t_g_param.amplitude);
            else
                t_noise = SimplexOctave2D(to->t_g_param.octaves, n_val_x, n_val_y, to->t_g_param.frequency, to->t_g_param.amplitude);

            if(t_noise >= 0.5f)
                num = 2;
            else if(t_noise >= 0.0f)
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

    vertexParam *vParam = &to->go.graphObj.shapes[0].vParam;

    TerrainVertex *verts = vParam->vertices;

    int iter = 0;

    to->height_map = calloc(vParam->verticesSize, sizeof(TerrainVertex));

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

            verts[iter].position.y = t_noise * to->t_g_param.height_factor;
        }
    }


    vec3 norm;

    for(int i = 0;i <= to->width; i++)
    {

        for(int j = 0;j <= to->height; j++)
        {
            if(i > 0 && i < to->width && j > 0 && j < to->height)
            {
                iter = (i - 1) * (to->width + 1) + j;
                float hL = verts[iter].position.y;

                iter = (i + 1) * (to->width + 1) + j;
                float hR = verts[iter].position.y;

                iter = (i - 0) * (to->width + 1) + (j - 1);
                float hD = verts[iter].position.y;

                iter = (i - 0) * (to->width + 1) + (j + 1);
                float hU = verts[iter].position.y;

                norm.x = hL - hR;
                norm.y = hD - hU;
                norm.z = 2.0f;

                iter = i * (to->width + 1) + j;
                verts[iter].normal = norm;

            }

        }
    }

    memcpy(to->height_map, verts, vParam->verticesSize * sizeof(TerrainVertex));

    BuffersUpdateVertex(vParam);
}

void TerrainObjectInit(TerrainObject *to, DrawParam *dParam, TerrainParam *tParam)
{
    GameObjectSetUpdateFunc(to, (void *)GameObject3DDefaultUpdate);
    GameObjectSetDrawFunc(to, (void *)GameObject3DDefaultDraw);
    GameObjectSetCleanFunc(to, (void *)GameObject3DClean);
    GameObjectSetRecreateFunc(to, (void *)GameObject3DRecreate);
    GameObjectSetDestroyFunc(to, (void *)TearrainDefaultDestroy);

    to->go.self.obj_type = ENGINE_GAME_OBJECT_TYPE_3D;

    Transform3DInit(&to->go.transform);
    GraphicsObjectInit(&to->go.graphObj, ENGINE_VERTEX_TYPE_TERRAIN);

    to->go.graphObj.gItems.perspective = true;

    to->go.self.flags = 0;

    to->t_t_param = tParam->t_t_param;
    to->t_g_param = tParam->t_g_param;
    to->flags = tParam->flags;
    to->width = tParam->columns;
    to->height = tParam->rows;
    to->t_shift = rand() % UINT16_MAX;
    to->t_shift = to->t_shift / UINT16_MAX;

    GraphicsObjectSetVertexSize(&to->go.graphObj, 0, sizeof(TerrainVertex), sizeof(uint32_t));

    InitTerrain(&to->go.graphObj.shapes[0].vParam, &to->go.graphObj.shapes[0].iParam, tParam);

    to->go.graphObj.num_shapes = 1;

    BuffersCreateVertex(&to->go.graphObj.shapes[0].vParam);
    BuffersCreateIndex(&to->go.graphObj.shapes[0].iParam);
    BuffersUpdateVertex(&to->go.graphObj.shapes[0].vParam);
    BuffersUpdateIndex(&to->go.graphObj.shapes[0].iParam);

    if((to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS))
        TerrainObjectGenerateTerrainHeights(to);

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
    }

    for(int i=0;i < tParam->t_t_param.num_textures;i++)
    {
        to->tex_colors[i].x = rand() % 255;
        to->tex_colors[i].y = rand() % 255;
        to->tex_colors[i].z = rand() % 255;
        to->tex_colors[i].w = rand() % 255;
    }

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
    }

    to->go.graphObj.num_shapes = 1;
}

void TerrainObjectAddTextureRender(TerrainObject *to, void *render)
{
    to->render = render;
}

void TerrainObjectAddDefault2(TerrainObject *to, void *render, void *shadow)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(LightSpaceMatrix), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorLghtMatrixUpdate);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorLightObjectsUpdate);

    BluePrintAddRenderImage(&to->go.graphObj.blueprints, nums, shadow);
    BluePrintAddRenderImage(&to->go.graphObj.blueprints, nums, to->render);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    setting.vertShader = &_binary_shaders_terrain_vert_2_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_terrain_vert_2_spv_size);
    setting.fragShader = &_binary_shaders_terrain_frag_2_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_terrain_frag_2_spv_size);
    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;

}

void TerrainObjectAddDefault(TerrainObject *to, void *render, void *shadow)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(LightSpaceMatrix), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorLghtMatrixUpdate);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(TextureBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)TerrainObjectDescriptorTextureUpdate);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorLightObjectsUpdate);


    BluePrintAddRenderImage(&to->go.graphObj.blueprints, nums, shadow);

    if(!(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE))
    {
        to->go.images[0].flags = ENGINE_TEXTURE_FLAG_URGB;
        BluePrintAddTextureImage(&to->go.graphObj.blueprints, nums, &to->go.images[0]);
    }else{

        //Карта для текстур
        GameObjectImage g_img;
        memset(&g_img, 0, sizeof(GameObjectImage));

        g_img.imgWidth = to->t_t_param.texture_width;
        g_img.imgHeight = to->t_t_param.texture_height;
        g_img.flags = ENGINE_TEXTURE_FLAG_URGB | ENGINE_TEXTURE_FLAG_SPECIFIC;

        to->texture_descr = BluePrintAddTextureImage(&to->go.graphObj.blueprints, nums, &g_img);

    }

    BluePrintAddTextureImageArray(&to->go.graphObj.blueprints, nums, &to->go.images[1], to->t_t_param.num_textures);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    setting.vertShader = &_binary_shaders_terrain_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_terrain_vert_spv_size);
    setting.fragShader = &_binary_shaders_terrain_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_terrain_frag_spv_size);
    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;

}


void TerrainObjectUpdate(TerrainObject *terrain)
{


}
