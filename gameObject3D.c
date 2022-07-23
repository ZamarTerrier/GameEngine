#include "gameObject3D.h"

#include "camera.h"

#include "pipeline.h"
#include "texture.h"

float tiker = 0;

void initGameObject3D(GameObject3D *go){

    initTransform3D(&go->transform);
    initGraphicsObject3D(&go->graphObj);

    go->graphObj.local.uniformCount = 0;
    go->graphObj.local.texturesCount = 0;

    go->graphObj.gItems.perspective = true;
}

void GameObject3DAddTexture(GameObject3D* go, const char* file){

    int temp = go->graphObj.local.texturesCount;

    go->graphObj.local.texturesCount ++;

    if(temp == 0)
    {
        free(go->graphObj.local.textures);
        go->graphObj.local.textures = NULL;
        go->graphObj.local.textures = (Texture2D *) calloc( go->graphObj.local.texturesCount, sizeof(Texture2D));
    }
    else
        go->graphObj.local.textures = (Texture2D *) realloc(go->graphObj.local.textures, sizeof(Texture2D) * go->graphObj.local.texturesCount);

    go->graphObj.local.textures[temp] = createTexture(file);

}

void GameObject3DChangeTexture(GameObject3D* go, int elem, const char* file){

    destroyTexture(&go->graphObj.local.textures[elem]);

    go->graphObj.local.textures[elem] = createTexture(file);

}

void GameObject3DAddUniformObject(localParam* param, VkDeviceSize size){

    int temp = param->uniformCount;
    param->uniformCount ++;

    if(temp == 0)
    {
        free(param->uniformSizes);
        param->uniformSizes = NULL;
        param->uniformSizes = (VkDeviceSize *) calloc(param->uniformCount, sizeof(VkDeviceSize));
    }
    else
        param->uniformSizes = (VkDeviceSize *) realloc(param->uniformSizes, param->uniformCount * sizeof(VkDeviceSize));

    param->uniformSizes[temp] = size;

}

void GameObject3DCreateDrawItems(GameObject3D* go){

    go->graphObj.gItems.graphicsPipeline = (VkPipeline *) calloc(0, sizeof(VkPipeline));
    go->graphObj.gItems.pipelineLayout = (VkPipelineLayout *) calloc(0, sizeof(VkPipelineLayout));

    createUniformBuffers(&go->graphObj.local);

    uint32_t unionSize = go->graphObj.local.texturesCount + go->graphObj.local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(unionSize, sizeof(VkDescriptorType)) ;

    for(i=0;i < go->graphObj.local.uniformCount;i++)
    {
        types[i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    for(i=0;i < go->graphObj.local.texturesCount;i++)
    {
        types[i + go->graphObj.local.uniformCount] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }


    createDescriptorSetLayout(&go->graphObj.gItems, types, unionSize);
    createDescriptorPool(&go->graphObj.gItems, types, unionSize);
    createDescriptorSets(&go->graphObj.gItems, &go->graphObj.local);


    PipelineSetting* settings;

    go->graphObj.gItems.settings = realloc(go->graphObj.gItems.settings, 2 * sizeof(PipelineSetting));

    settings = (PipelineSetting *) go->graphObj.gItems.settings;

    /*
    settings[0].poligonMode = VK_POLYGON_MODE_LINE;
    settings[0].topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    settings[0].vertShader = go->graphObj.aShader.vertShader;
    settings[0].fragShader = "J:/Projects/Game/shaders/3DObject/line_frag.spv";
    createGraphicsPipeline(&go->graphObj);*/

    settings[0].poligonMode = VK_POLYGON_MODE_FILL;
    settings[0].topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    settings[0].vertShader = go->graphObj.aShader.vertShader;
    settings[0].fragShader = go->graphObj.aShader.fragShader;
    settings[0].drawType = 0;
    createGraphicsPipeline(&go->graphObj);

    settings[1].poligonMode = VK_POLYGON_MODE_FILL;
    settings[1].topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    settings[1].vertShader = "J:/Projects/Game/shaders/3DObject/outline_vert.spv";
    settings[1].fragShader = "J:/Projects/Game/shaders/3DObject/outline_frag.spv";
    settings[1].drawType = 1;
    createGraphicsPipeline(&go->graphObj);


    free(types);
    types = NULL;
}

void GameObject3DClean(GameObject3D* go){
    cleanGraphicsObject(&go->graphObj);
}

//Описание вертекса
VkVertexInputBindingDescription GameObject3DGetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex3D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

void GameObject3DDestroy(GameObject3D* go){

    destroyGraphicsObject(&go->graphObj);

    free(go);
    go = NULL;

}

void GameObject3DDraw(GameObject3D* go){

    for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){

        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.graphicsPipeline[i]);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vertex.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.index.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelineLayout[i], 0, 1, &go->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

        PipelineSetting* settings = (PipelineSetting *)go->graphObj.gItems.settings;

        switch(settings[i].drawType){
            case 0:
                vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.index.indexesSize, 1, 0, 0, 0);
                break;
            case 1:
                vkCmdDraw(commandBuffers[imageIndex], go->graphObj.shape.vertex.verticesSize, 1, 0, 0);
                break;
        }

    }

}

void GameObject3DUpdateUniformBuffer(GameObject3D* go) {

    Camera3D* cam = (Camera3D*) cam3D;
    void* data;

    ViewBuffer3D vuo = {};
    vuo.position = cam->position;
    vuo.rotation = cam->rotation;
    vuo.scale = cam->scale;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex], 0, sizeof(ViewBuffer3D), 0, &data);
    memcpy(data, &vuo, sizeof(ViewBuffer3D));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex]);

    TransformBuffer3D tbo = {};
    tbo.position = go->transform.position;
    tbo.rotation = go->transform.rotation;
    tbo.scale = go->transform.scale;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[1][imageIndex], 0, sizeof(TransformBuffer3D), 0, &data);
    memcpy(data, &tbo, sizeof(TransformBuffer3D));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[1][imageIndex]);

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};
    mat4 edenMat = mat4_f(1,0,0,0,
                          0,1,0,0,
                          0,0,1,0,
                          0,0,0,1);

    mbo.model = m4_translate(m4_rotation_matrix(m4_scale_mat(go->transform.scale), go->transform.rotation), go->transform.position);
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.1f, 100.0f);
    mbo.proj.m[1][1] *= -1;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[2][imageIndex], 0, sizeof(mbo), 0, &data);
    memcpy(data, &mbo, sizeof(mbo));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[2][imageIndex]);

}
