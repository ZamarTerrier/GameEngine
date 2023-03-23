#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "engine_includes.h"

#define RENDER_TEXTURE_WIDTH 2048
#define RENDER_TEXTURE_HEIGHT 2048

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
} RenderTexture;

void RenderTextureInit(RenderTexture *render);
void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff);
void RenderTextureEndRendering(void *cmd_buff);
void RenderTextureDestroy(RenderTexture *render);

#endif // RENDER_TEXTURE_H
