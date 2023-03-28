#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "engine_includes.h"

typedef enum{
    ENGINE_RENDER_TYPE_WINDOW,
    ENGINE_RENDER_TYPE_IMAGE,
    ENGINE_RENDER_TYPE_DEPTH
} EngineRenderType;

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

    void *render_pass;

    uint32_t m_currentStage;

    uint32_t width;
    uint32_t height;
    uint32_t mip_levels;
    uint32_t m_format;

    uint32_t type;

    uint32_t indx_descriptor;
} RenderTexture;

void RenderTextureInit(RenderTexture *render, uint32_t type, uint32_t width, uint32_t height);
void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff);
void RenderTextureRecreate(RenderTexture *render);
void RenderTextureEndRendering(void *cmd_buff);
void RenderTextureDestroy(RenderTexture *render);

#endif // RENDER_TEXTURE_H
