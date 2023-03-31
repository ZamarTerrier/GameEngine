#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "engine_includes.h"

#include "gameObject.h"

typedef enum{
    ENGINE_RENDER_TYPE_WINDOW,
    ENGINE_RENDER_TYPE_IMAGE,
    ENGINE_RENDER_TYPE_DEPTH
} EngineRenderType;

typedef enum{
    ENGINE_RENDER_FLAG_ONE_SHOT = 0x1,
    ENGINE_RENDER_FLAG_SHOOTED = 0x2
} EngineRenderFlag;

typedef struct{
    void *image;
    void *image_memory;
    void *sampler;
    void *view;

    void *framebufer;


    uint32_t m_currentLayout;
} RenderFrame;

typedef struct{
    RenderFrame *frames;
    uint32_t num_frames;

    void *render_pass;

    uint32_t m_currentStage;

    uint32_t width;
    uint32_t height;
    uint32_t mip_levels;
    uint32_t m_format;

    uint32_t type;

    uint32_t indx_descriptor;

    uint32_t flags;
} RenderTexture;

void RenderTextureInit(RenderTexture *render, uint32_t type, uint32_t width, uint32_t height);

void RenderTextureReload(RenderTexture *render);

void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff);
void RenderTextureRecreate(RenderTexture *render);
void RenderTextureEndRendering(void *cmd_buff);
void RenderTextureDestroy(RenderTexture *render);

#endif // RENDER_TEXTURE_H
