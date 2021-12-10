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
    struct{
        VkBuffer** uniformBuffers;
        VkDeviceMemory** uniformBuffersMemory;
        VkDeviceSize* uniformSizes;
        uint32_t uniformCount;
        uint32_t texturesCount;
        Texture2D* textures;
    } local;
    GraphicItems gItems;
    aboutShader aShader;
    Shape shape;
    vec2 pos;
    vec2 scale;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
} GameObject;

GameObject* initGameObject();

VkVertexInputBindingDescription getBindingDescription();

attrDescr getAttributeDescriptions();

void addTexture(GameObject* go, const char* file);

void addUniformObject(GameObject* go, VkDeviceSize size);

void cleanGameObject(GameObject* go);

void createDrawningParams(GameObject* go);

void destroyGameObject(GameObject* go);

void gameObjectDraw(GameObject* go);

void recreateDrawningParams(GameObject* go);

void updateUniformBuffer(GameObject* go);

void setPosGameObject(GameObject* go, vec2 pos);

vec2 getPosGameObject(GameObject* go);

void setScaleGameObject(GameObject* go, vec2 scale);

vec2 getScaleGameObject(GameObject* go);

vec2 getSizeGameObject(GameObject* go);

#endif // GAMEOBJECT_H
