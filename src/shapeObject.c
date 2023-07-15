#include "shapeObject.h"

#include <vulkan/vulkan.h>

#include "shader_builder.h"

#include "e_buffer.h"
#include "e_camera.h"

#include "e_blue_print.h"

#include "e_math.h"

#include "e_resource_data.h"
#include "e_resource_export.h"
#include "e_resource_engine.h"

void ShapeObjectCreateQuad(ShapeObject *so, QuadParams *param)
{
    Vertex2D *verts = calloc(4, sizeof(Vertex2D));

    verts[0].position.x = -param->size / WIDTH;
    verts[0].position.y = -param->size / HEIGHT;
    verts[0].texCoord.x = 0;
    verts[0].texCoord.y = 0;

    verts[1].position.x = param->size / WIDTH;
    verts[1].position.y = -param->size / HEIGHT;
    verts[1].texCoord.x = 1.0f;
    verts[1].texCoord.y = 0;

    verts[2].position.x = param->size / WIDTH;
    verts[2].position.y = param->size / HEIGHT;
    verts[2].texCoord.x = 1.0f;
    verts[2].texCoord.y = 1.0f;

    verts[3].position.x = -param->size / WIDTH;
    verts[3].position.y = param->size / HEIGHT;
    verts[3].texCoord.x = 0;
    verts[3].texCoord.y = 1.0f;

    for(int i=0;i < 4;i++)
    {
        verts[i].color = param->color;
    }

    uint32_t *tIndx = calloc(6, sizeof(uint32_t));

    uint32_t indx[] = {
      0, 1, 2, 2, 3, 0
    };

    memcpy(tIndx, indx, 6 * sizeof(uint32_t));

    GraphicsObjectSetVertex(&so->go.graphObj, verts, 4, sizeof(Vertex2D), tIndx, 6, sizeof(uint32_t));
}

void ShapeObjectCreateCircle(ShapeObject *so, CircleParams *param)
{
    int freq = param->freq;

    float stackAngle, sectorAngle;

    if(freq < 3)
        freq = 3;


    uint32_t num_verts = (freq + 1);
    Vertex2D *verts = calloc( num_verts, sizeof(Vertex2D));

    float angInc = 2 * M_PI / freq;

    verts[0].position.x = 0;
    verts[0].position.y = 0;

    verts[0].texCoord.x = 0.5f;
    verts[0].texCoord.y = 0.5f;

    verts[0].color = param->color;

    float divW = param->radius / WIDTH;
    float divH = param->radius / HEIGHT;

    for(int i=1;i <= freq;i++)
    {
        Vertex2D *vert = &verts[i];

        float x = cos(i * angInc);
        float y = sin(i * angInc);

        vert->position.x = x * divW;
        vert->position.y = y * divH;

        vert->texCoord.x = (float)(x + 1.0f) / 2;
        vert->texCoord.y = (float)(y + 1.0f) / 2;

        vert->color = param->color;
    }

    uint32_t indx_size = (freq + 1) * 3;
    uint32_t *indx = calloc( indx_size, sizeof(uint32_t));

    int k0 = 0, k1 = 1, k2 = 2, iter = 0;
    for(int i = 0; i <= freq; ++i)
    {
        indx[iter] = k0;
        indx[iter + 1] = k1;
        indx[iter + 2] = k2;

        k1 ++;
        k2 ++;

        if(k2 == num_verts)
            k2 = 1;

        iter +=3;
    }

    GraphicsObjectSetVertex(&so->go.graphObj, verts, num_verts, sizeof(Vertex2D), indx, indx_size, sizeof(uint32_t));
}

void ShapeObjectCreateShape(ShapeObject *so, ShapeParams *param)
{

    if(param->num_positions <= 2)
        return;

    uint32_t num_verts = param->num_positions + 1;
    Vertex2D *verts = calloc( num_verts, sizeof(Vertex2D));

    vec2 center = {0, 0};

    vec2 w_size = { WIDTH, HEIGHT };

    vec2 min = param->positions[0];
    vec2 max = param->positions[0];
    for(int i=1; i < param->num_positions; i++)
    {
        if(min.x > param->positions[i].x)
            min.x = param->positions[i].x;
        else if(min.y > param->positions[i].y)
            min.y = param->positions[i].y;

        if(max.x < param->positions[i].x)
            max.x = param->positions[i].x;
        else if(max.y < param->positions[i].y)
            max.y = param->positions[i].y;
    }

    min = v2_div(min, w_size);
    max = v2_div(max, w_size);

    for(int i=0; i < param->num_positions; i++)
    {
        Vertex2D *vert = &verts[i + 1];

        vert->position = v2_div(param->positions[i], w_size);
        vert->texCoord.x = vert->position.x / max.x + min.x;
        vert->texCoord.y = vert->position.y / max.y + min.y;
        vert->color = param->color;

        center = v2_add(center, param->positions[i]);
    }

    center = v2_divs(center, param->num_positions);

    verts[0].position = v2_div(center, w_size);
    verts[0].texCoord.x = 0.5f;
    verts[0].texCoord.y = 0.5f;
    verts[0].color = param->color;


    uint32_t indx_size = (param->num_positions + 1) * 3;
    uint32_t *indx = calloc( indx_size, sizeof(uint32_t));

    int k0 = 0, k1 = 1, k2 = 2, iter = 0;
    for(int i = 0; i < param->num_positions; ++i)
    {
        indx[iter] = k0;
        indx[iter + 1] = k1;
        indx[iter + 2] = k2;

        k1 ++;
        k2 ++;

        if(k2 == num_verts)
            k2 = 1;

        iter +=3;
    }

    GraphicsObjectSetVertex(&so->go.graphObj, verts, num_verts, sizeof(Vertex2D), indx, indx_size, sizeof(uint32_t));
}

void ShapeObjectCreateLine(ShapeObject *so, LineParams *line)
{
    Vertex2D *verts = calloc(2, sizeof(Vertex2D));


    vec2 position;

    verts[0].position.x = line->position.x / WIDTH;
    position.x = verts[0].position.x + (line->direction.x * (line->length / WIDTH));

    verts[0].position.y = line->position.y / HEIGHT;
    position.y = verts[0].position.y + (line->direction.y * (line->length / HEIGHT));

    verts[0].color = line->color;

    verts[1].position = position;
    verts[1].color = line->color;


    GraphicsObjectSetVertex(&so->go.graphObj, verts, 2, sizeof(Vertex2D), NULL, 0, sizeof(uint32_t));
}

void ShapeObjectInit(ShapeObject *so, DrawParam *dParam, ShapeType type, void *param)
{
    GameObject2DInit(so);

    switch(type)
    {
        case ENGINE_SHAPE_OBJECT_LINE:
            ShapeObjectCreateLine(so, param);
            break;
        case ENGINE_SHAPE_OBJECT_QUAD:
            ShapeObjectCreateQuad(so, param);
            break;
        case ENGINE_SHAPE_OBJECT_CIRCLE:
            ShapeObjectCreateCircle(so, param);
            break;
        case ENGINE_SHAPE_OBJECT_SHAPE:
            ShapeObjectCreateShape(so, param);
            break;
        default:
            printf("Error Shape Type!\n");
            break;
    }

    so->type = type;

    so->go.image = calloc(1, sizeof(GameObjectImage));

    if(strlen(dParam->diffuse) != 0)
    {
        int len = strlen(dParam->diffuse);
        so->go.image->path = calloc(len + 1, sizeof(char));
        memcpy(so->go.image->path, dParam->diffuse, len);
        so->go.image->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    if(strlen(dParam->specular) != 0)
    {
        int len = strlen(dParam->specular);
        so->go.image->path = calloc(len + 1, sizeof(char));
        memcpy(so->go.image->path, dParam->specular, len);
        so->go.image->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    so->go.graphObj.num_shapes = 1;
}

void ShapeObjectAddDefault(ShapeObject *so, void *render)
{
    uint32_t nums = so->go.graphObj.blueprints.num_blue_print_packs;
    so->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&so->go.graphObj.blueprints, nums, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject2DTransformBufferUpdate, 0);
    BluePrintAddUniformObject(&so->go.graphObj.blueprints, nums, sizeof(ImageBufferObjects), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject2DImageBuffer, 0);

    BluePrintAddTextureImage(&so->go.graphObj.blueprints, nums, so->go.image, VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&so->go.graphObj, &setting);

    ShaderBuilder some_shader;

    ShaderBuilderInit(&some_shader, SHADER_TYPE_VERTEX);
    ShaderBuilderMake(&some_shader);

    PipelineSettingSetShader(&setting, &_binary_shaders_sprite_vert_spv_start, (size_t)(&_binary_shaders_sprite_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_sprite_frag_spv_start, (size_t)(&_binary_shaders_sprite_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.flags |= ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE;

    if(so->type == ENGINE_SHAPE_OBJECT_LINE)
    {
        setting.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);
    }

    GameObject2DAddSettingPipeline(so, nums, &setting);

    so->go.graphObj.blueprints.num_blue_print_packs ++;
}

void ShapeObjectAddShaderBuilder(ShapeObject *so, void *render, void *vertex, void *fragment)
{
    ShaderBuilder *vert = vertex;
    ShaderBuilder *frag = fragment;

    uint32_t nums = so->go.graphObj.blueprints.num_blue_print_packs;
    so->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&so->go.graphObj.blueprints, nums, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject2DTransformBufferUpdate, 0);

    ShaderStructConstr uniform_arr[] = {
        {SHADER_VARIABLE_TYPE_VECTOR, 2, 0, "position"},
        {SHADER_VARIABLE_TYPE_VECTOR, 2, 0, "rotation"},
        {SHADER_VARIABLE_TYPE_VECTOR, 2, 0, "scale"},
    };

    uint32_t uniform = ShaderBuilderAddUniform(vert, uniform_arr, 3, "TransformBufferObjects");

    uint32_t posit = ShaderBuilderAddIOData(vert, SHADER_VARIABLE_TYPE_VECTOR, 0, NULL, 2, "position", 0);
    uint32_t clr_indx = ShaderBuilderAddIOData(vert, SHADER_VARIABLE_TYPE_VECTOR, 0, NULL, 3, "color", 1);
    uint32_t txt_indx = ShaderBuilderAddIOData(vert, SHADER_VARIABLE_TYPE_VECTOR, 0, NULL, 2, "inTexCoord", 2);

    uint32_t clr_dst = ShaderBuilderAddIOData(vert, SHADER_VARIABLE_TYPE_VECTOR, SHADER_DATA_FLAG_OUTPUT, NULL, 3, "fragColor", 0);
    uint32_t txt_dst = ShaderBuilderAddIOData(vert, SHADER_VARIABLE_TYPE_VECTOR, SHADER_DATA_FLAG_OUTPUT, NULL, 2, "fragTexCoord", 1);

    ShaderBuilderAddFuncAdd(vert, &vert->main_point_index->labels[0], uniform, posit, 2, vert->gl_struct_indx);
    ShaderBuilderAddFuncMove(vert, &vert->main_point_index->labels[0], clr_indx, 3, clr_dst);
    ShaderBuilderAddFuncMove(vert, &vert->main_point_index->labels[0], txt_indx, 2, txt_dst);

    BluePrintAddUniformObject(&so->go.graphObj.blueprints, nums, sizeof(ImageBufferObjects), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject2DImageBuffer, 0);


    uint32_t fragColor = ShaderBuilderAddIOData(frag, SHADER_VARIABLE_TYPE_VECTOR, 0, NULL, 3, "fragColor", 0);
    uint32_t outColor = ShaderBuilderAddIOData(frag, SHADER_VARIABLE_TYPE_VECTOR, SHADER_DATA_FLAG_OUTPUT, NULL, 4, "outColor", 1);

    ShaderBuilderAddFuncMove(frag, &frag->main_point_index->labels[0], fragColor, 4, outColor);

    BluePrintAddTextureImage(&so->go.graphObj.blueprints, nums, so->go.image, VK_SHADER_STAGE_FRAGMENT_BIT);


    ShaderBuilderMake(vertex);
    ShaderBuilderMake(frag);

    PipelineSetting setting;

    PipelineSettingSetDefault(&so->go.graphObj, &setting);


    //PipelineSettingSetShader(&setting, &_binary_shaders_sprite_vert_spv_start, (size_t)(&_binary_shaders_sprite_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);

    PipelineSettingSetShader(&setting, vert->code, vert->size * sizeof(uint32_t), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_sprite_frag_spv_start, (size_t)(&_binary_shaders_sprite_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.flags |= ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE;

    if(so->type == ENGINE_SHAPE_OBJECT_LINE)
    {
        setting.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);
    }

    GameObject2DAddSettingPipeline(so, nums, &setting);

    so->go.graphObj.blueprints.num_blue_print_packs ++;
}

void ShapeObjectInitDefault(ShapeObject *so, DrawParam *dParam, ShapeType type, void *param)
{
    ShapeObjectInit(so, dParam, type, param);
    ShapeObjectAddDefault(so, dParam->render);
    GameObject2DInitDraw(so);
}
