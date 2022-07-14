#include "gameObject3D.h"

#include "camera.h"

#include "texture.h"

float tiker = 0;

void initGameObject3D(GameObject3D *go){

    initTransform3D(&go->transform);
    initGraphicsObject3D(&go->graphObj);

    go->graphObj.local.uniformCount = 0;
    go->graphObj.local.texturesCount = 0;
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

    createUniformBuffers(&go->graphObj.local);

    uint32_t unionSize = go->graphObj.local.texturesCount + go->graphObj.local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(unionSize,sizeof(VkDescriptorType)) ;

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

    vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.graphicsPipeline);

    VkBuffer vertexBuffers[] = {go->graphObj.shape.vertex.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.index.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelineLayout, 0, 1, &go->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

    vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.index.indexesSize, 1, 0, 0, 0);
}

void GameObject3DUpdateUniformBuffer(GameObject3D* go) {

    Camera* cam = (Camera*) camObj;

    ModelBuffer3D ubo = {};
    mat4 temp = mat4_f(1,0,0,0,
                       0,1,0,0,
                       0,0,1,0,
                       0,0,0,1);

    ubo.model = m4_translate(m4_rotation_matrix(temp, go->transform.rotation), go->transform.position);//,
    ubo.view = temp;//m4_look_at((vec3){2.0f, 2.0f, 2.0f}, (vec3){1.0f, 1.0f, 1.0f}, (vec3){0.0f, 0.0f, 1.0f});
    ubo.proj = m4_perspective(75.0f, 1, 100);

    void* data;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex]);

}
