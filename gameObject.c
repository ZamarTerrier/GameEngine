#include "gameObject.h"

#include "buffers.h"
#include "camera.h"
#include <math.h>

void initGameObject2D(GameObject2D* go)
{
    initTransform2D(&go->transform);
    initGraphicsObject(&go->graphObj);

    go->graphObj.local.uniformCount = 0;
    go->graphObj.local.texturesCount = 0;

}

void addTexture(GameObject2D* go, const char* file){

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

void changeTexture(GameObject2D* go, int elem, const char* file){

    destroyTexture(&go->graphObj.local.textures[elem]);

    go->graphObj.local.textures[elem] = createTexture(file);

}

void addUniformObject(localParam* param, VkDeviceSize size){

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

void addSettingPipeline(GameObject2D* go, PipelineSetting setting){
    PipelineSetting* settings;

    go->graphObj.gItems.settingsCount++;
    go->graphObj.gItems.settings = realloc(go->graphObj.gItems.settings, go->graphObj.gItems.settingsCount * sizeof(PipelineSetting));

    settings = (PipelineSetting *) go->graphObj.gItems.settings;

    memcpy(&settings[go->graphObj.gItems.settingsCount - 1], &setting, sizeof(PipelineSetting));

}

void createDrawItemsGameObject(GameObject2D* go){

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

    PipelineSetting settings;

    /*settings.poligonMode = VK_POLYGON_MODE_LINE;
    settings.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    settings.vertShader = go->graphObj.aShader.vertShader;
    settings.fragShader = "J:/Projects/Game/shaders/3DObject/line_frag.spv";
    addSettingPipeline(go, settings);
    createGraphicsPipeline(&go->graphObj);*/

    settings.poligonMode = VK_POLYGON_MODE_FILL;
    settings.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    settings.vertShader = go->graphObj.aShader.vertShader;
    settings.fragShader = go->graphObj.aShader.fragShader;
    addSettingPipeline(go, settings);

    createGraphicsPipeline(&go->graphObj);

    free(types);
    types = NULL;
}

void cleanGameObject(GameObject2D* go){
    cleanGraphicsObject(&go->graphObj);
}

//Описание вертекса
VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex2D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

void destroyGameObject(GameObject2D* go){

    destroyGraphicsObject(&go->graphObj);

    free(go);
    go = NULL;

}

void gameObjectDraw(GameObject2D* go){

    for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){
        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.graphicsPipeline[i]);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vertex.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.index.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelineLayout[i], 0, 1, &go->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

        vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.index.indexesSize, 1, 0, 0, 0);
    }
}

void updateUniformBuffer(GameObject2D* go) {

    Camera2D* cam = (Camera2D*) cam2D;
    void* data;

    ViewBuffer2D vuo = {};
    vuo.position = cam->position;
    vuo.rotation = cam->rotation;
    vuo.scale = cam->scale;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex], 0, sizeof(vuo), 0, &data);
    memcpy(data, &vuo, sizeof(vuo));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex]);

    TransformBuffer2D ubo = {};
    ubo.position = go->transform.position;
    ubo.rotation = go->transform.rotation;
    ubo.scale = go->transform.scale;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[1][imageIndex], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[1][imageIndex]);

    ImgBuffer iup = {};
    iup.imgOffset = go->transform.img.offset;
    iup.imgScale = go->transform.img.scale;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[2][imageIndex], 0, sizeof(iup), 0, &data);
    memcpy(data, &iup, sizeof(iup));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[2][imageIndex]);

}

vec2 getSizeGameObject(GameObject2D* go)
{
    vec2 size;
    size.x = fabs((go->graphObj.shape.vertex.vertices[1].position.x - go->graphObj.shape.vertex.vertices[3].position.x) * (viewSize.x * diffSize.x)) * go->transform.scale.x;
    size.y = fabs((go->graphObj.shape.vertex.vertices[1].position.y - go->graphObj.shape.vertex.vertices[3].position.y) * (viewSize.y * diffSize.y)) * go->transform.scale.y;
    return size;
}
