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
#include "e_resource_export.h"

void PrimitiveObjectInitTexture(PrimitiveObject *po, DrawParam *dParam)
{
    po->go.diffuse = calloc(1, sizeof(GameObjectImage));
    po->go.specular = calloc(1, sizeof(GameObjectImage));
    po->go.normal = calloc(1, sizeof(GameObjectImage));

    if(dParam == NULL)
        return;

    if(strlen(dParam->diffuse) != 0)
    {
        int len = strlen(dParam->diffuse);
        po->go.diffuse->path = calloc(len + 1, sizeof(char));
        memcpy(po->go.diffuse->path, dParam->diffuse, len);
        po->go.diffuse->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    if(strlen(dParam->specular) != 0)
    {
        int len = strlen(dParam->specular);
        po->go.specular->path = calloc(len + 1, sizeof(char));
        memcpy(po->go.specular->path, dParam->specular, len);
        po->go.specular->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }


    if(strlen(dParam->normal) != 0)
    {
        int len = strlen(dParam->normal);
        po->go.normal->path = calloc(len + 1, sizeof(char));
        memcpy(po->go.normal->path, dParam->normal, len);
        po->go.normal->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

}

void PrimitiveObjectInit(PrimitiveObject *po, DrawParam dParam, char type, void *params){

    GameObject3DInit(po);

    PlaneParam *pParam = (PlaneParam *)params;
    SphereParam *sParam = (SphereParam *)params;
    CubeSphereParam *csParam = (CubeSphereParam *)params;
    ConeParam *cParam = (ConeParam *)params;    
    TerrainParam *tParam = (TerrainParam *)params;

    GraphicsObjectSetVertexSize(&po->go.graphObj, sizeof(Vertex3D), sizeof(uint32_t));

    int builded = false;

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
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CUBE :
            GraphicsObjectSetVertex(&po->go.graphObj, cubeVert, 24, cubeIndx, 36);
            GameObject3DSetLinkedShape(po);
            break;
        case ENGINE_PRIMITIVE3D_CUBESPHERE :
            Cubesphere(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, csParam->radius, csParam->verperrow);
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_ICOSPHERE :
            IcoSphereGenerator(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, sParam->radius);
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_SPHERE :
            SphereGenerator3D(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CONE :
            ConeGenerator(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, cParam->height, cParam->sectorCount, cParam->stackCount);
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_SKYBOX:
            SphereGenerator3D(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_TERRAIN:
            InitTerrain3D(&po->go.graphObj.shape.vParam, &po->go.graphObj.shape.iParam, tParam->rows, tParam->colmns, tParam->cell_step);
            builded = true;
            break;
    }

    if(builded)
    {
        BuffersCreateVertex(&po->go.graphObj.shape.vParam, po->go.graphObj.shape.vParam.verticesSize);
        BuffersCreateIndex(&po->go.graphObj.shape.iParam, po->go.graphObj.shape.iParam.indexesSize);
        BuffersUpdateVertex(&po->go.graphObj.shape.vParam);
        BuffersUpdateIndex(&po->go.graphObj.shape.iParam);
    }

    GraphicsObjectSetShadersPath(&po->go.graphObj, dParam.vertShader, dParam.fragShader);

    po->go.graphObj.local.descrCount = 0;

    BuffersAddUniformObject(&po->go.graphObj.local, sizeof(ModelBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&po->go.graphObj.local, sizeof(LightBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    PrimitiveObjectInitTexture(po, &dParam);

    ImageAddTexture(&po->go.graphObj.local, po->go.diffuse);
    ImageAddTexture(&po->go.graphObj.local, po->go.specular);
    ImageAddTexture(&po->go.graphObj.local, po->go.normal);

    GraphicsObjectCreateDrawItems(&po->go.graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&po->go.graphObj, &setting);

    setting.obj_type = ENGINE_TYPE_PRIMITIVE_OBJECT;
    setting.vertShader = &_binary_shaders_3d_object_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_vert_spv_size);
    setting.fragShader = &_binary_shaders_3d_object_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_frag_spv_size);
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    setting.drawType = 0;
    setting.fromFile = 0;
    GameObject3DAddSettingPipeline(po, &setting);

    setting.obj_type = ENGINE_TYPE_PRIMITIVE_OBJECT_LINE;
    setting.vertShader = &_binary_shaders_3d_object_line_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_line_vert_spv_size);
    setting.fragShader = &_binary_shaders_3d_object_line_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_line_frag_spv_size);
    setting.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    setting.drawType = 0;
    setting.fromFile = 0;

    GameObject3DAddSettingPipeline(po, &setting);

    PipelineCreateGraphics(&po->go.graphObj);

    if(type == ENGINE_PRIMITIVE3D_SKYBOX)
        Transform3DSetScale(po, -500, -500, -500);
}
