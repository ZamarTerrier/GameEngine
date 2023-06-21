#ifndef TERRAIN_OBJECT_H
#define TERRAIN_OBJECT_H

#include "engine_includes.h"
#include "gameObject3D.h"

#define MAX_TEXTURE_STACK 21

typedef struct{
    uint32_t size_factor;
    uint32_t height_factor;
    float displaisment_factor;
    uint32_t amplitude;
    uint32_t frequency;
    uint32_t octaves;
} TerrainGeneratorParam;

typedef struct{
    uint32_t height_map_scale;
    uint32_t texture_scale;
    uint32_t texture_width;
    uint32_t texture_height;
    uint32_t num_textures;
} TerrainTextureParam;

typedef struct{
    uint32_t size_patch;
    uint32_t flags;
    float vertex_step;
    TerrainGeneratorParam t_g_param;
    TerrainTextureParam t_t_param;
    char *texture_map;
    char **textures;
} TerrainParam;

typedef struct{
    int multi_size;
    int num_textures;
    vec2 cam_posxz;
    vec4 tex_colors[MAX_TEXTURE_STACK];
} TextureBuffer;

enum ENGINE_TERRIAN_FLAGS{
    ENGINE_TERRIAN_FLAGS_CUSTOM_TEXTURE = 0x1,
    ENGINE_TERRIAN_FLAGS_MIXED_TEXTURE = 0x2,
    ENGINE_TERRIAN_FLAGS_REPEATE_TEXTURE = 0x4,
    ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE = 0x8,
    ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS = 0x10,
    ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_OLD = 0x20,
    ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS_PERLIN = 0x40,
};

typedef struct{
    GameObject3D go;
    uint32_t flags;
    char *height_map;
    uint32_t width;
    uint32_t height;
    float t_shift;
    TerrainTextureParam t_t_param;
    TerrainGeneratorParam t_g_param;
    vec4_u8 tex_colors[MAX_TEXTURE_STACK];
    BluePrintDescriptor *texture_descr;
    BluePrintDescriptor *texture_height_map;
    GameObjectImage heightMap;
    void *render;
} TerrainObject;

void TerrainObjectInit(TerrainObject *to, DrawParam *dParam, TerrainParam *tParam);
void TerrainObjectDefaultInit(TerrainObject *to, DrawParam *dParam, TerrainParam *tParam);

void TerrainObjectMakeDefaultParams(TerrainParam *tParam, uint32_t texture_width, uint32_t texture_height, uint32_t height_map_size);
void TerrainObjectMakeOldDefaultParams(TerrainParam *tParam, uint32_t texture_width, uint32_t texture_height, uint32_t height_map_size);

void TerrainObjectAddTextureRender(TerrainObject *to, void *render);

void TerrainObjectAddDefault(TerrainObject *to, DrawParam *dParam);

void TerrainObjectGenerateTerrainTextureMap(TerrainObject *to, void *buffer);
void TerrainObjectGenerateTerrainHeightTextureMap(TerrainObject *to, void *buffer);

uint32_t TerrainObjectGetTextureColor(TerrainObject *to, int index);

#endif // TERRAIN_OBJECT_H
