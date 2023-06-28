#include "terrain_object.h"

#include <vulkan/vulkan.h>

#include "engine.h"

#include "e_camera.h"
#include "graphicsObject.h"
#include "lightObject.h"
#include "render_texture.h"

#include "e_buffer.h"
#include "pipeline.h"
#include "e_texture.h"

#include "e_math.h"

#include "e_tools.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void TerrainObjectMakeDefaultParams(TerrainParam *tParam, uint32_t texture_width, uint32_t texture_height, uint32_t height_map_size)
{
    tParam->size_patch = 256;
    tParam->t_g_param.size_factor = 182;
    tParam->t_g_param.height_factor = 12;
    tParam->t_g_param.displaisment_factor = 512.0f;
    tParam->t_g_param.tesselation_factor = 0.75f;
    tParam->t_g_param.frequency = 1;
    tParam->t_g_param.amplitude = 1;
    tParam->t_g_param.octaves = 6;
    tParam->vertex_step = 3.0;
    tParam->t_t_param.height_map_scale = height_map_size;
    tParam->t_t_param.texture_scale = 180.0f;
    tParam->t_t_param.texture_width = texture_width;
    tParam->t_t_param.texture_height = texture_height;
    tParam->t_t_param.num_textures = 0;
    tParam->flags = ENGINE_TERRIAN_FLAGS_REPEATE_TEXTURE | ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE | ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS ;//| ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN;
}

void TerrainObjectMakeOldDefaultParams(TerrainParam *tParam, uint32_t texture_width, uint32_t texture_height, uint32_t height_map_size)
{
    tParam->size_patch = 500;
    tParam->t_g_param.size_factor = 300;
    tParam->t_g_param.height_factor = 120;
    tParam->t_g_param.displaisment_factor = 1.0f;
    tParam->t_g_param.tesselation_factor = 0.75f;
    tParam->t_g_param.frequency = 1;
    tParam->t_g_param.amplitude = 1;
    tParam->t_g_param.octaves = 6;
    tParam->vertex_step = 1.0;
    tParam->t_t_param.texture_scale = 160.0f;
    tParam->t_t_param.height_map_scale = height_map_size;
    tParam->t_t_param.texture_width = texture_width;
    tParam->t_t_param.texture_height = texture_height;
    tParam->t_t_param.num_textures = 0;
    tParam->flags = ENGINE_TERRIAN_FLAGS_REPEATE_TEXTURE | ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE | ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS ;//| ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN;
}

void TerrainObjectDescriptorTextureUpdate(TerrainObject *to, BluePrintDescriptor *descriptor)
{
    TextureBuffer tb;

    tb.multi_size = to->t_t_param.texture_scale;

    DescriptorUpdate(descriptor, &tb, sizeof(tb));
}

enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };

void TerrainObjectDescriptorTesselationUpdate(TerrainObject *to, BluePrintDescriptor *descriptor)
{
    Camera3D* cam = (Camera3D*) cam3D;
    RenderTexture *render = current_render;

    TesselationBuffer tb;
    memset(&tb, 0, sizeof(TesselationBuffer));

    tb.displacementFactor = to->t_g_param.displaisment_factor;
    tb.tessellationFactor = to->t_g_param.tesselation_factor;
    tb.tessellatedEdgeSize = 20.0f;

    tb.lightPos.y = -0.5f - tb.displacementFactor; // todo: Not uesed yet
    tb.viewportDim = vec2_f((float)WIDTH, (float)HEIGHT);

    mat4 proj = m4_perspective(render->width, render->height, 60.0f, 0.1f, 512.0f);
    mat4 view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), vec3_f(0.0f,1.0f, 0.0f));

    mat4 matrix = m4_mult(proj, view);

    tb.frustumPlanes[LEFT].x = matrix.m[0][3] + matrix.m[0][0];
    tb.frustumPlanes[LEFT].y = matrix.m[1][3] + matrix.m[1][0];
    tb.frustumPlanes[LEFT].z = matrix.m[2][3] + matrix.m[2][0];
    tb.frustumPlanes[LEFT].w = matrix.m[3][3] + matrix.m[3][0];

    tb.frustumPlanes[RIGHT].x = matrix.m[0][3] - matrix.m[0][0];
    tb.frustumPlanes[RIGHT].y = matrix.m[1][3] - matrix.m[1][0];
    tb.frustumPlanes[RIGHT].z = matrix.m[2][3] - matrix.m[2][0];
    tb.frustumPlanes[RIGHT].w = matrix.m[3][3] - matrix.m[3][0];

    tb.frustumPlanes[TOP].x = matrix.m[0][3] - matrix.m[0][1];
    tb.frustumPlanes[TOP].y = matrix.m[1][3] - matrix.m[1][1];
    tb.frustumPlanes[TOP].z = matrix.m[2][3] - matrix.m[2][1];
    tb.frustumPlanes[TOP].w = matrix.m[3][3] - matrix.m[3][1];

    tb.frustumPlanes[BOTTOM].x = matrix.m[0][3] + matrix.m[0][1];
    tb.frustumPlanes[BOTTOM].y = matrix.m[1][3] + matrix.m[1][1];
    tb.frustumPlanes[BOTTOM].z = matrix.m[2][3] + matrix.m[2][1];
    tb.frustumPlanes[BOTTOM].w = matrix.m[3][3] + matrix.m[3][1];

    tb.frustumPlanes[BACK].x = matrix.m[0][3] + matrix.m[0][2];
    tb.frustumPlanes[BACK].y = matrix.m[1][3] + matrix.m[1][2];
    tb.frustumPlanes[BACK].z = matrix.m[2][3] + matrix.m[2][2];
    tb.frustumPlanes[BACK].w = matrix.m[3][3] + matrix.m[3][2];

    tb.frustumPlanes[FRONT].x = matrix.m[0][3] - matrix.m[0][2];
    tb.frustumPlanes[FRONT].y = matrix.m[1][3] - matrix.m[1][2];
    tb.frustumPlanes[FRONT].z = matrix.m[2][3] - matrix.m[2][2];
    tb.frustumPlanes[FRONT].w = matrix.m[3][3] - matrix.m[3][2];


    for (int i = 0; i < 6; i++)
    {
        tb.frustumPlanes[i] = v4_normalize(tb.frustumPlanes[i]);
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

    uint16_t *some_map = buffer; //[size_texture];

    uint16_t temp = 0;

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

            iter = i * to->t_t_param.texture_width + j;

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

            some_map[iter] = num;
        }
    }

    TextureUpdate(to->texture_descr, some_map, size_texture * sizeof(uint16_t), 0);
}

void TerrainObjectGenerateTerrainHeightTextureMap(TerrainObject *to)
{
    uint32_t size_texture = to->t_t_param.height_map_scale * to->t_t_param.height_map_scale;

    to->height_map = calloc(size_texture, sizeof(uint16_t));

    uint16_t *heightMap = to->height_map;

    uint32_t temp = 0;

    int num = 2, iter = 0;

    float t_noise = 0;

    float x_del = (float)to->t_t_param.height_map_scale / to->width;
    float y_del = (float)to->t_t_param.height_map_scale / to->height;

    for(int i = 0;i < to->t_t_param.height_map_scale; i++)
    {
        float n_val_x = ((float)i * to->t_shift) / to->t_g_param.size_factor / x_del;

        for(int j = 0;j < to->t_t_param.height_map_scale; j++)
        {
            float n_val_y = ((float)j * to->t_shift) / to->t_g_param.size_factor / y_del;

            iter = i * to->t_t_param.height_map_scale + j;

            if(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN)
                t_noise = PerlinOctave2D(to->t_g_param.octaves, n_val_x, n_val_y, to->t_g_param.frequency, to->t_g_param.amplitude);
            else
                t_noise = SimplexOctave2D(to->t_g_param.octaves, n_val_x, n_val_y, to->t_g_param.frequency, to->t_g_param.amplitude);

            float s_val = (t_noise + 1.0) / 2;
            uint16_t t_val = UINT16_MAX * s_val;

            heightMap[iter] = -t_val;
        }
    }

    TextureUpdate(to->texture_height_map, heightMap, size_texture * sizeof(uint16_t), 0);
}

void TerrainObjectGenerateTerrainHeights(TerrainObject *to)
{

    vertexParam *vParam = &to->go.graphObj.shapes[0].vParam;

    TerrainVertex *verts = vParam->vertices;

    int iter = 0;

    to->height_map = calloc(vParam->verticesSize, sizeof(TerrainVertex));

    for(int i = 0;i < to->width; i++)
    {
        float n_x = ((float)i * to->t_shift)/ to->t_g_param.size_factor;
        for(int j = 0;j < to->height; j++)
        {
            float n_y = ((float)j * to->t_shift)/ to->t_g_param.size_factor;

            iter = i * to->width + j;

            float t_noise;

            if(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN)
                t_noise = PerlinOctave2D( to->t_g_param.octaves, n_x, n_y, to->t_g_param.frequency, to->t_g_param.amplitude);
            else
                t_noise = SimplexOctave2D( to->t_g_param.octaves, n_x, n_y, to->t_g_param.frequency, to->t_g_param.amplitude);

            /*if(t_noise <= -0.5f)
                 t_noise = -0.5f;*/

            verts[iter].position.y = verts[iter].position.y * t_noise * to->t_g_param.height_factor;
        }
    }


    vec3 norm;

    for(int i = 0;i < to->width; i++)
    {
        for(int j = 0;j < to->height; j++)
        {
            if((i > 0) && (i < to->width - 1) && (j > 0) && (j < to->height - 1))
            {
                iter = (i - 1) * to->width + j;
                float hL = verts[iter].position.y;

                iter = (i + 1) * to->width + j;
                float hR = verts[iter].position.y;

                iter = i * to->width + (j - 1);
                float hD = verts[iter].position.y;

                iter = i * to->width + (j + 1);
                float hU = verts[iter].position.y;

                norm.x = hL - hR;
                norm.y = hD - hU;
                norm.z = 2.0f;

                iter = i * to->width + j;
                verts[iter].normal = v3_norm(norm);

            }
        }
    }

    memcpy(to->height_map, verts, vParam->verticesSize * sizeof(TerrainVertex));

    BuffersUpdateVertex(vParam);
}

void TerrainObjectSetShadowDefaultDescriptor(TerrainObject *to, DrawParam *dParam)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(TextureBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)TerrainObjectDescriptorTextureUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(DirLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorDirLightsUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(PointLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorPointLightsUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(SpotLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorSpotLightsUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(LightStatusBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DLigtStatusBufferUpdate, 0);

    RenderTexture **renders = dir_shadow_array;

    if(num_dir_shadows > 1)
        BluePrintAddRenderImageArray(&to->go.graphObj.blueprints, nums, renders, num_dir_shadows);
    else
        BluePrintAddRenderImage(&to->go.graphObj.blueprints, nums, renders[0]);

    renders = point_shadow_array;

    if(num_point_shadows > 1)
        BluePrintAddRenderImageArray(&to->go.graphObj.blueprints, nums, renders, num_point_shadows);
    else
        BluePrintAddRenderImage(&to->go.graphObj.blueprints, nums, renders[0]);

    renders = spot_shadow_array;

    if(num_spot_shadows > 1)
        BluePrintAddRenderImageArray(&to->go.graphObj.blueprints, nums, renders, num_spot_shadows);
    else
        BluePrintAddRenderImage(&to->go.graphObj.blueprints, nums, renders[0]);

    if(!(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE))
    {
        to->go.images[0].flags = ENGINE_TEXTURE_FLAG_URGB;
        BluePrintAddTextureImage(&to->go.graphObj.blueprints, nums, &to->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);
    }else{

        //Карта для текстур
        GameObjectImage g_img;
        memset(&g_img, 0, sizeof(GameObjectImage));

        g_img.imgWidth = to->t_t_param.texture_width;
        g_img.imgHeight = to->t_t_param.texture_height;
        g_img.flags = ENGINE_TEXTURE_FLAG_URGB | ENGINE_TEXTURE_FLAG_SPECIFIC;

        to->texture_descr = BluePrintAddTextureImage(&to->go.graphObj.blueprints, nums, &g_img, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    BluePrintAddTextureImageArray(&to->go.graphObj.blueprints, nums, &to->go.images[1], to->t_t_param.num_textures);
    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_terrain_shadow_vert_spv_start, (size_t)(&_binary_shaders_terrain_shadow_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_terrain_shadow_frag_spv_start, (size_t)(&_binary_shaders_terrain_shadow_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;
}

void TerrainObjectAddDefault(TerrainObject *to, DrawParam *dParam)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(TesselationBuffer), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, (void *)TerrainObjectDescriptorTesselationUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(TextureBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)TerrainObjectDescriptorTextureUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(DirLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorDirLightsUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(PointLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorPointLightsUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(SpotLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorSpotLightsUpdate, 0);
    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(LightStatusBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DLigtStatusBufferUpdate, 0);

    //Height Map
    GameObjectImage g_img;
    memset(&g_img, 0, sizeof(GameObjectImage));

    if(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_OLD){
        g_img.imgWidth = 100;
        g_img.imgHeight = 100;
        g_img.flags = ENGINE_TEXTURE_FLAG_R16 | ENGINE_TEXTURE_FLAG_SPECIFIC;

    }else if(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS){
        g_img.imgWidth = to->t_t_param.height_map_scale;
        g_img.imgHeight = to->t_t_param.height_map_scale;
        g_img.flags = ENGINE_TEXTURE_FLAG_R16 | ENGINE_TEXTURE_FLAG_SPECIFIC;
    }else{
        g_img = to->heightMap;
    }

    to->texture_height_map = BluePrintAddTextureImage(&to->go.graphObj.blueprints, nums, &g_img, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

    //TextureMap
    if(!(to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE))
    {
        to->go.images[0].img_type = VK_FORMAT_R8G8B8A8_SRGB;
        BluePrintAddTextureImage(&to->go.graphObj.blueprints, nums, &to->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);
    }else{

        //Карта для текстур
        memset(&g_img, 0, sizeof(GameObjectImage));

        g_img.imgWidth = to->t_t_param.texture_width;
        g_img.imgHeight = to->t_t_param.texture_height;
        g_img.flags = ENGINE_TEXTURE_FLAG_R16_UINT | ENGINE_TEXTURE_FLAG_SPECIFIC;

        to->texture_descr = BluePrintAddTextureImage(&to->go.graphObj.blueprints, nums, &g_img, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    //Texture Array
    BluePrintAddTextureImageArray(&to->go.graphObj.blueprints, nums, &to->go.images[1], to->t_t_param.num_textures);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_terrain_vert_spv_start, (size_t)(&_binary_shaders_terrain_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_terrain_frag_spv_start, (size_t)(&_binary_shaders_terrain_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_terrain_tesc_spv_start, (size_t)(&_binary_shaders_terrain_tesc_spv_size), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_terrain_tese_spv_start, (size_t)(&_binary_shaders_terrain_tese_spv_size), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;
    //setting.poligonMode = VK_POLYGON_MODE_LINE;
    setting.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    setting.flags |= ENGINE_PIPELINE_FLAG_TESSELLATION_CONTROL_SHADER | ENGINE_PIPELINE_FLAG_TESSELLATION_EVALUATION_SHADER | ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE;
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_ALPHA);

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;

}

float TerrainObjectGetHeight(TerrainObject *to, uint32_t x, uint32_t y)
{
    uint32_t scale = to->t_t_param.height_map_scale / to->width;

    uint16_t *heightMap = to->height_map;

    int i_x = x * scale;
    int i_y = y * scale;
    i_x = e_max(0, e_min(i_x, (int)to->t_t_param.height_map_scale - 1));
    i_y = e_max(0, e_min(i_y, (int)to->t_t_param.height_map_scale - 1));
    i_x = i_x / scale;
    i_y = i_y / scale;

    return *(heightMap + (i_x + i_y * to->t_t_param.height_map_scale) * scale) / (float)UINT16_MAX * to->t_g_param.displaisment_factor;
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
    to->width = tParam->size_patch;
    to->height = tParam->size_patch;
    to->t_shift = rand() % UINT16_MAX;
    to->t_shift = to->t_shift / UINT16_MAX;

    vertexParam vParam;
    indexParam iParam;

    InitTerrain(&vParam, &iParam, tParam);

    GraphicsObjectSetVertex(&to->go.graphObj, vParam.vertices, vParam.verticesSize, sizeof(TerrainVertex), iParam.indices, iParam.indexesSize, sizeof(uint32_t));

    free(vParam.vertices);
    free(iParam.indices);

    if((to->flags & ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_OLD))
        TerrainObjectGenerateTerrainHeights(to);

    to->go.images = calloc(tParam->t_t_param.num_textures + 1, sizeof(GameObjectImage));
    to->go.num_images = tParam->t_t_param.num_textures + 1;

    if(strlen(tParam->texture_map) != 0)
    {
        int len = strlen(tParam->texture_map);
        to->heightMap.path = calloc(len + 1, sizeof(char));
        memcpy(to->heightMap.path, tParam->texture_map, len);
        to->heightMap.path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
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

void TerrainObjectApplyShadows(TerrainObject *to, DrawParam *dParam)
{
    RenderTexture **renders = dir_shadow_array;

    for(int i=0;i < num_dir_shadows;i++)
        GameObject3DAddShadowDescriptor(to, ENGINE_LIGHT_TYPE_DIRECTIONAL, renders[i], i);

    renders = point_shadow_array;

    for(int i=0;i < num_point_shadows;i++)
        GameObject3DAddOmiShadow(to, renders[i], i);

    renders = spot_shadow_array;

    for(int i=0;i < num_spot_shadows;i++)
        GameObject3DAddShadowDescriptor(to, ENGINE_LIGHT_TYPE_SPOT, renders[i], i);

    TerrainObjectSetShadowDefaultDescriptor(to, dParam);
}

void TerrainObjectDefaultInit(TerrainObject *to, DrawParam *dParam, TerrainParam *tParam)
{

    TerrainObjectInit(to, dParam, tParam);

    if(dParam->flags & ENGINE_DRAW_PARAM_FLAG_ADD_SHADOW)
        TerrainObjectApplyShadows(to, dParam);
    else
        TerrainObjectAddDefault(to, dParam);

    GameObject3DInitDraw(to);
}

void TerrainObjectAddTextureRender(TerrainObject *to, void *render)
{
    to->render = render;
}

