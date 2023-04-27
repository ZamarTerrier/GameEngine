#ifndef E_BLUE_PRINT_H
#define E_BLUE_PRINT_H

#include "engine_includes.h"

#include "e_pipeline_variables.h"

typedef enum{
    ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE = 0x1,
    ENGINE_BLUE_PRINT_FLAG_ARRAY_IMAGE = 0x2,
    ENGINE_BLUE_PRINT_FLAG_LINKED_UNIFORM = 0x4,
    ENGINE_BLUE_PRINT_FLAG_LINKED_TEXTURE = 0x8,
} EngineBluePrintFlag;

typedef struct{
    void** uniformBuffers;
    void** uniformBuffersMemory;
    uint32_t size;
} UniformStruct;

typedef struct{
    void *update;
    void **textures;
    uint32_t num_textures;
    uint32_t max_textures;
    UniformStruct *uniform;
    uint32_t descrType;
    uint32_t descrCount;
    uint32_t stageflag;
    uint32_t size;
    uint32_t buffsize;
    uint32_t flags;
    uint32_t indx_layer;
    GameObjectImage *image;
} BluePrintDescriptor;

typedef struct{
    uint32_t stageFlags;
    uint32_t offset;
    uint32_t size;
} BluePrintPushConstant;

typedef struct{
    PipelineSetting settings[MAX_UNIFORMS];
    uint32_t num_settings;
    BluePrintDescriptor descriptors[MAX_UNIFORMS];
    uint32_t num_descriptors;
    BluePrintPushConstant push_constants[MAX_UNIFORMS];
    uint32_t num_push_constants;
    void *render_point;
} BluePrintPack;

typedef struct{
    BluePrintPack blue_print_packs[MAX_BLUE_PRINTS];
    uint32_t num_blue_print_packs;
    uint32_t isShadow;
} Blueprints;

void BluePrintAddPushConstant(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t stage, uint32_t offset);

BluePrintDescriptor *BluePrintAddExistUniformStorage(Blueprints *blueprints, uint32_t indx_pack, uint32_t flags, UniformStruct *uniform, void *update_func, uint32_t layer_indx);
BluePrintDescriptor *BluePrintAddUniformStorage(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags, void *update_func, uint32_t layer_indx);
void BluePrintAddUniformObject(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags, void *update_func, uint32_t layer_indx);

void BluePrintRecreateUniform(BluePrintPack *pack);

void BluePrintAddRenderImageCube(Blueprints *blueprints, uint32_t indx_pack, uint32_t indx_cube, void *obj);
void BluePrintAddRenderImageVector(Blueprints *blueprints, uint32_t indx_pack, void *obj, uint32_t size);
void BluePrintAddRenderImageArray(Blueprints *blueprints, uint32_t indx_pack, void *obj, uint32_t size);
void BluePrintAddRenderImage(Blueprints *blueprints, uint32_t indx_pack, void *render);
void BluePrintAddTextureImageArray(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *images, uint32_t size);

BluePrintDescriptor *BluePrintAddExistTextureImage(Blueprints *blueprints, uint32_t indx_pack, void *texture);
BluePrintDescriptor *BluePrintAddTextureImage(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *image);

#endif // E_BLUE_PRINT_H
