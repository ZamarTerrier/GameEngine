#include "primitiveObject.h"

#include <vulkan/vulkan.h>

#include "e_math.h"

#include "camera.h"

#include "buffers.h"
#include "texture.h"
#include "pipeline.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_descriptors.h"
#include "e_resource_export.h"

void PrimitiveObjectInitTexture(PrimitiveObject *po, DrawParam *dParam)
{
    po->go.images = calloc(3, sizeof(GameObjectImage));

    if(dParam == NULL)
        return;

    if(strlen(dParam->diffuse) != 0)
    {
        int len = strlen(dParam->diffuse);
        po->go.images[0].path = calloc(len + 1, sizeof(char));
        memcpy(po->go.images[0].path, dParam->diffuse, len);
        po->go.images[0].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    if(strlen(dParam->specular) != 0)
    {
        int len = strlen(dParam->specular);
        po->go.images[1].path = calloc(len + 1, sizeof(char));
        memcpy(po->go.images[1].path, dParam->specular, len);
        po->go.images[1].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }


    if(strlen(dParam->normal) != 0)
    {
        int len = strlen(dParam->normal);
        po->go.images[2].path = calloc(len + 1, sizeof(char));
        memcpy(po->go.images[2].path, dParam->normal, len);
        po->go.images[0].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

}

void PrimitiveObjectDestroy(PrimitiveObject *po)
{
    GameObject3DDestroy(po);

    free(po->params);
    po->params = NULL;
}



void PrimitiveObjectInit(PrimitiveObject *po, DrawParam dParam, char type, void *params){

    GameObject3DInit(po);

    GameObjectSetDestroyFunc(po, (void *)PrimitiveObjectDestroy);

    po->type = type;

    PlaneParam *pParam = (PlaneParam *)params;
    SphereParam *sParam = (SphereParam *)params;
    CubeSphereParam *csParam = (CubeSphereParam *)params;
    ConeParam *cParam = (ConeParam *)params;

    GraphicsObjectSetVertexSize(&po->go.graphObj, sizeof(Vertex3D), sizeof(uint32_t));

    int builded = false;

    po->params = NULL;
    switch(type)
    {
        case ENGINE_PRIMITIVE3D_LINE :
            GraphicsObjectSetVertex(&po->go.graphObj, lineVert, 2, NULL, 0);
            GameObject3DSetLinkedShape(po);
            break;
        case ENGINE_PRIMITIVE3D_TRIANGLE :
            GraphicsObjectSetVertex(&po->go.graphObj, triVert, 3, triIndx, 3);
            GameObject3DSetLinkedShape(po);
            break;
        case ENGINE_PRIMITIVE3D_QUAD :
            GraphicsObjectSetVertex(&po->go.graphObj, quadVert, 4, quadIndx, 6);
            GameObject3DSetLinkedShape(po);
            break;
        case ENGINE_PRIMITIVE3D_PLANE :
            InitPlane3D(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, pParam->sectorCount, pParam->stackCount);
            po->params = calloc(1, sizeof(PlaneParam));
            memcpy(po->params, params, sizeof(PlaneParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CUBE :
            GraphicsObjectSetVertex(&po->go.graphObj, cubeVert, 24, cubeIndx, 36);
            GameObject3DSetLinkedShape(po);
            break;
        case ENGINE_PRIMITIVE3D_CUBESPHERE :
            Cubesphere(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, csParam->radius, csParam->verperrow);
            po->params = calloc(1, sizeof(CubeSphereParam));
            memcpy(po->params, params, sizeof(CubeSphereParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_ICOSPHERE :
            IcoSphereGenerator(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, sParam->radius);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            break;
        case ENGINE_PRIMITIVE3D_SPHERE :
            SphereGenerator3D(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CONE :
            ConeGenerator(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, cParam->height, cParam->sectorCount, cParam->stackCount);
            po->params = calloc(1, sizeof(ConeParam));
            memcpy(po->params, params, sizeof(ConeParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_SKYBOX:
            SphereGenerator3D(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
    }

    if(builded)
    {
        BuffersCreateVertex(&po->go.graphObj.shape.vParam);
        BuffersCreateIndex(&po->go.graphObj.shape.iParam);
        BuffersUpdateVertex(&po->go.graphObj.shape.vParam);
        BuffersUpdateIndex(&po->go.graphObj.shape.iParam);
    }

    po->go.graphObj.local.descrCount = 0;

    BuffersAddUniformObject(&po->go.graphObj.local, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&po->go.graphObj.local, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT);

    PrimitiveObjectInitTexture(po, &dParam);

    TextureImageAdd(&po->go.graphObj.local, &po->go.images[0]);
    TextureImageAdd(&po->go.graphObj.local, &po->go.images[1]);
    TextureImageAdd(&po->go.graphObj.local, &po->go.images[2]);

    GraphicsObjectCreateDrawItems(&po->go.graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&po->go.graphObj, &setting);

    if(strlen(dParam.fragShader) > 0 && strlen(dParam.vertShader) > 0)
    {
        GraphicsObjectSetShadersPath(&po->go.graphObj, dParam.vertShader, dParam.fragShader);

        setting.vertShader = dParam.vertShader;
        setting.sizeVertShader = 0;
        setting.fragShader = dParam.fragShader;
        setting.sizeFragShader = 0;
        setting.topology = dParam.topology;
        setting.drawType = dParam.drawType;
        setting.fromFile = 1;
        GameObject3DAddSettingPipeline(po, &setting);

    }else{

        setting.vertShader = &_binary_shaders_3d_object_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_vert_spv_size);
        setting.fragShader = &_binary_shaders_3d_object_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_frag_spv_size);
        setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        setting.drawType = 0;
        setting.fromFile = 0;
        GameObject3DAddSettingPipeline(po, &setting);

        setting.vertShader = &_binary_shaders_3d_object_line_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_line_vert_spv_size);
        setting.fragShader = &_binary_shaders_3d_object_line_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_line_frag_spv_size);
        setting.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        setting.drawType = 0;
        setting.fromFile = 0;
        GameObject3DAddSettingPipeline(po, &setting);

    }

    PipelineCreateGraphics(&po->go.graphObj);

    if(type == ENGINE_PRIMITIVE3D_SKYBOX)
        Transform3DSetScale(po, -500, -500, -500);
}

void *PrimitiveObjectGetVertex(PrimitiveObject *po)
{
    return &po->go.graphObj.shape.vParam;
}

void PrimitiveObjectDiffuseTextureSetData(PrimitiveObject *po, void *data, uint32_t size_data, uint32_t offset)
{
    ShaderDescriptor *descriprots = po->go.graphObj.local.descriptors;

    TextureUpdate(&descriprots[2], data, size_data, offset);
}

void PrimitiveObjectDiffuseSetTexture(PrimitiveObject *po, const char *path)
{
    ShaderDescriptor *descriprots = po->go.graphObj.local.descriptors;

    TextureSetTexture(&descriprots[2], path);
}
