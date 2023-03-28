#ifndef E_BLUE_PRINT_H
#define E_BLUE_PRINT_H

#include "engine_includes.h"

#include "e_pipeline_variables.h"

typedef struct{
    void** uniformBuffers;
    void** uniformBuffersMemory;
    uint32_t size;
} UniformStruct;

typedef struct{
    void *texture;
    void *texture_array;
    uint32_t num_textures;
    uint32_t max_textures;
    UniformStruct uniform;
    uint32_t descrType;
    uint32_t descrCount;
    uint32_t stageflag;
    uint32_t size;
    uint32_t buffsize;
    void *render_image;
    GameObjectImage *image;
} BluePrintDescriptor;

typedef struct{
    PipelineSetting settings[MAX_UNIFORMS];
    uint32_t num_settings;
    BluePrintDescriptor descriptors[MAX_UNIFORMS];
    uint32_t num_descriptors;
    void *render_point;
} BluePrintPack;

typedef struct{
    BluePrintPack blue_print_packs[MAX_BLUE_PRINTS];
    BluePrintDescriptor shadow_descr;
    uint32_t isShadow;
    uint32_t num_blue_print_packs;
} Blueprints;

void BluePrintAddUniformObject(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags);
void BluePrintAddUniformShadow(BluePrintDescriptor *descriptor, uint64_t size, uint32_t flags);

void BluePrintRecreateUniform(BluePrintPack *pack);

void BluePrintAddRenderImage(Blueprints *blueprints, uint32_t indx_pack, void *render);

BluePrintDescriptor *BluePrintAddTextureImage(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *image);

#endif // E_BLUE_PRINT_H
