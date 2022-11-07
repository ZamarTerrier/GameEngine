#include "primitiveObject.h"

#include "e_math.h"

#include "camera.h"

#include "e_resource.h"
#include "buffers.h"
#include "texture.h"
#include "pipeline.h"

void SkyBoxUpdate(GameObject3D* go) {

    if(go->graphObj.local.descriptors == NULL)
        return;

    Camera3D* cam = (Camera3D*) cam3D;
    void* data;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    go->transform.model = m4_translate_mat_add(m4_mult(m4_scale_mat(go->transform.scale), m4_rotation_matrix(go->transform.rotation)), go->transform.position);

    mbo.model = go->transform.model;
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.01f, 1000.0f);
    mbo.proj.m[1][1] *= -1;

    vkMapMemory(device, go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
    memcpy(data, &mbo, sizeof(mbo));
    vkUnmapMemory(device, go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);


    SomeData_e sbo = {};
    sbo.time = glfwGetTime() * 0.002f;

    vkMapMemory(device, go->graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(sbo), 0, &data);
    memcpy(data, &sbo, sizeof(sbo));
    vkUnmapMemory(device, go->graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);

}

void PrimitiveObjectInit(GameObject3D *go, DrawParam dParam, char type, void *params){

    GameObject3DInit(go);

    PlaneParam *pParam = (PlaneParam *)params;
    SphereParam *sParam = (SphereParam *)params;
    CubeSphereParam *csParam = (CubeSphereParam *)params;
    ConeParam *cParam = (ConeParam *)params;    
    TerrainParam *tParam = (TerrainParam *)params;

    switch(type)
    {
        case ENGINE_PRIMITIVE3D_LINE :
            GraphicsObjectSetVertex(&go->graphObj, lineVert, 2, NULL, 0);
            break;
        case ENGINE_PRIMITIVE3D_TRIANGLE :
            GraphicsObjectSetVertex(&go->graphObj, triVert, 3, triIndx, 3);
            break;
        case ENGINE_PRIMITIVE3D_QUAD :
            GraphicsObjectSetVertex(&go->graphObj, quadVert, 4, quadIndx, 6);
            break;
        case ENGINE_PRIMITIVE3D_PLANE :
            InitPlane3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, pParam->sectorCount, pParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_CUBE :
            Cubesphere(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, 1, 2);
            break;
        case ENGINE_PRIMITIVE3D_CUBESPHERE :
            Cubesphere(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, csParam->radius, csParam->verperrow);
            break;
        case ENGINE_PRIMITIVE3D_ICOSPHERE :
            IcoSphereGenerator(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, sParam->radius);
            break;
        case ENGINE_PRIMITIVE3D_SPHERE :
            SphereGenerator3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_CONE :
            ConeGenerator(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, cParam->height, cParam->sectorCount, cParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_SKYBOX:
            SphereGenerator3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_TERRAIN:
            InitTerrain3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, tParam->rows, tParam->colmns, tParam->cell_step);
            break;
    }

    GraphicsObjectSetShadersPath(&go->graphObj, dParam.vertShader, dParam.fragShader);

    go->graphObj.local.descrCount = 0;

    BuffersAddUniformObject(&go->graphObj.local, sizeof(ModelBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&go->graphObj.local, sizeof(LightBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    go->image = calloc(1, sizeof(ImageStruct));

    if(strlen(dParam.filePath) != 0)
    {
        int len = strlen(dParam.filePath);
        go->image->path = calloc(len + 1, sizeof(char));
        memcpy(go->image->path, dParam.filePath, len);
        go->image->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
        ImageAddTexture(&go->graphObj.local, go->image);
    }

    GameObject3DCreateDrawItems(&go->graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_3d_object_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_vert_spv_size);
        setting.fragShader = &_binary_shaders_3d_object_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject3DAddSettingPipeline(go, setting);

    PipelineCreateGraphics(&go->graphObj);

    if(type == ENGINE_PRIMITIVE3D_SKYBOX)
    {
        vec3 size;
        size.x = -500;
        size.y = -500;
        size.z = -500;
        Transform3DSetScale(go, size);
    }
}
