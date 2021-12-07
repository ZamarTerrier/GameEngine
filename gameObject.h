#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "stdinclude.h"
#include "resource.h"
#include "bindDesciption.h"
#include "pipeline.h"
#include "texture.h"

typedef struct{
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet* descriptorSets;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorPool descriptorPool;
} GraphicItems;

typedef struct{
    const char* vertShader;
    const char* fragShader;
    VkVertexInputBindingDescription bindingDescription;
    VkVertexInputAttributeDescription* attr;
    size_t countAttr;
} aboutShader;

typedef struct{
    struct{
        Vertex* vertices;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t verticesSize;
    } vertex;
    struct{
        uint32_t* indices;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexesSize;
    } index;
} Shape;

typedef struct{
    Texture2D* textures;
    struct{
        VkBuffer** uniformBuffers;
        VkDeviceMemory** uniformBuffersMemory;
        uint32_t uniformCount;
        uint32_t texturesCount;
    } local;
    vec2 pos;
    GraphicItems gItems;
    aboutShader aShader;
    Shape shape;
} GameObject;

GameObject initGameObject();

VkVertexInputBindingDescription getBindingDescription();

attrDescr getAttributeDescriptions();

void destroyGameObject(GameObject* go);

void gameObjectDraw(GameObject* go);

void recreateDrawningParams(GameObject* go);

void updateUniformBuffer(GameObject* go);

#endif // GAMEOBJECT_H
