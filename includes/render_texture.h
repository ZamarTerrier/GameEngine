#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "engine_includes.h"

#include "gameObject.h"

#define MAX_SURFACES 10

typedef enum{
    ENGINE_RENDER_TYPE_WINDOW = 0x1,
    ENGINE_RENDER_TYPE_IMAGE = 0x2,
    ENGINE_RENDER_TYPE_DEPTH = 0x4,
    ENGINE_RENDER_TYPE_CUBEMAP = 0x8,
} EngineRenderType;

typedef enum{
    ENGINE_RENDER_FLAG_ONE_SHOT = 0x1,
    ENGINE_RENDER_FLAG_SHOOTED = 0x2,
    ENGINE_RENDER_FLAG_PERSPECTIVE = 0x4,
    ENGINE_RENDER_FLAG_FRUSTRUM = 0x8,
    ENGINE_RENDER_FLAG_ORTHOGRAPHIC = 0x10,
    ENGINE_RENDER_FLAG_DEPTH = 0x20,
} EngineRenderFlag;

typedef struct{
    void* depth_image;
    void* depth_memory;
    void* depth_view;

    void *image;
    void *image_memory;
    void *sampler;
    void *view;

    void **shadowCubeRenderPass;
    void **shadowCubeMapFaceImageViews;

    void **framebufers;

    uint32_t m_currentLayout;
} RenderFrame;

typedef struct{
    RenderFrame *frames;
    uint32_t num_frames;

    void *render_pass;


    double persp_view_distance;
    double persp_view_near;
    double persp_view_angle;

    double frust_side;
    double frust_near;
    double frust_far;

    double ortg_view_distance;
    double ortg_view_size;

    float cascadeSplit;

    vec3 up_vector;

    mat4 view;

    uint32_t m_currentStage;

    uint32_t currFrame;

    uint32_t width;
    uint32_t height;
    uint32_t mip_levels;
    uint32_t m_format;

    EngineRenderType type;

    uint32_t indx_descriptor;
    uint32_t flags;
} RenderTexture;

void RenderTextureInit(RenderTexture *render, uint32_t type, uint32_t width, uint32_t height, uint32_t flags);

void RenderTextureAddSurface(RenderTexture *render);
void RenderTextureReload(RenderTexture *render, uint32_t indx_surf);

void RenderTextureSetCurrentFrame(RenderTexture *render, uint32_t indx_frame);
void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff);
void RenderTextureRecreate(RenderTexture *render);
void RenderTextureEndRendering(RenderTexture *render, void *cmd_buff);
void RenderTextureDestroy(RenderTexture *render);

#endif // RENDER_TEXTURE_H
