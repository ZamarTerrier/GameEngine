#ifndef TERRAIN_OBJECT_H
#define TERRAIN_OBJECT_H

#include "engine_includes.h"
#include "gameObject3D.h"

#define MAX_TEXTURE_STACK 21

typedef struct{
    uint32_t columns;
    uint32_t rows;
    uint32_t cell_step;
    uint32_t scale_hight_factor;
    uint32_t scale_size_factor;
    uint32_t texture_width;
    uint32_t texture_height;
    char *texture_map;
    char **textures;
    uint32_t num_textures;
    uint32_t flags;
} TerrainParam;

typedef struct{
    int multi_size;
    int num_textures;
    vec2 some_val;
    vec4 tex_colors[MAX_TEXTURE_STACK];
} TerrainBuffer;

enum ENGINE_TERRIAN_FLAGS{
    ENGINE_TERRIAN_FLAGS_CUSTOM_TEXTURE = 0x1,
    ENGINE_TERRIAN_FLAGS_MIXED_TEXTURE = 0x2,
    ENGINE_TERRIAN_FLAGS_REPEATE_TEXTURE = 0x4,
    ENGINE_TERRIAN_FLAGS_GENERATE_TEXTURE = 0x8,
    ENGINE_TERRIAN_FLAGS_GENERATE_HEIGHTS = 0x10,
};

typedef struct{
    GameObject3D go;
    uint32_t flags;
    uint32_t num_textures;
    uint32_t texture_scale;
    uint32_t texture_width;
    uint32_t texture_height;
    uint32_t width;
    uint32_t height;
    uint32_t scale_hight_factor;
    uint32_t scale_size_factor;
    vec4_u8 tex_colors[MAX_TEXTURE_STACK];
} TerrainObject;

void TerrainObjectInit(TerrainObject *to, DrawParam *dParam, TerrainParam *tParam);

#endif // TERRAIN_OBJECT_H
