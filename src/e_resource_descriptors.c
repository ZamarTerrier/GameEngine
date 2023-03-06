#include "e_resource_descriptors.h"

#include <vulkan/vulkan.h>

EIVertexInputAttributeDescription planeAttributeDescription[] = {

    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex2D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex2D, color) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex2D, texCoord) }
};

EIVertexInputAttributeDescription cubeAttributeDescription[] = {

    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex3D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex3D, normal) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex3D, color) },
    {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex3D, texCoord) }
};

EIVertexInputAttributeDescription modelAttributeDescription[] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ModelVertex3D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ModelVertex3D, normal) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ModelVertex3D, color) },
    {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(ModelVertex3D, texCoord) },
    {
        .binding = 0,
        .location = 4,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(ModelVertex3D, joints) },
    {
        .binding = 0,
        .location = 5,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(ModelVertex3D, weight) }
};

EIVertexInputAttributeDescription particle2DAttributeDescription[] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(ParticleVertex2D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32_SFLOAT,
        .offset = offsetof(ParticleVertex2D, size) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ParticleVertex2D, color) }
};

EIVertexInputAttributeDescription particle3DAttributeDescription[] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ParticleVertex3D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32_SFLOAT,
        .offset = offsetof(ParticleVertex3D, size) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ParticleVertex3D, color) }
};

EIVertexInputAttributeDescription TerrainAttributeDescription[] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(TerrainVertex, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(TerrainVertex, normal) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(TerrainVertex, texCoord) },
};

EIVertexInputBindingDescription Bind2DDescription = {
    .binding = 0,
    .stride = sizeof(Vertex2D),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

EIVertexInputBindingDescription Bind3DDescription = {
    .binding = 0,
    .stride = sizeof(Vertex3D),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

EIVertexInputBindingDescription BindModel3DDescription = {
    .binding = 0,
    .stride = sizeof(ModelVertex3D),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

EIVertexInputBindingDescription BindParticle2DDescription = {
    .binding = 0,
    .stride = sizeof(ParticleVertex2D),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

EIVertexInputBindingDescription BindParticle3DDescription = {
    .binding = 0,
    .stride = sizeof(ParticleVertex3D),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

EIVertexInputBindingDescription BindTerrainDescription = {
    .binding = 0,
    .stride = sizeof(TerrainVertex),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

