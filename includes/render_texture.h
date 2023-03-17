#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "engine_includes.h"

typedef struct{
    void *image;
    void *sampler;
    void *view;

    void *render_pass;
    void *framebufer;

    uint32_t m_currentLayout;
    uint32_t m_currentStage;

    uint32_t width;
    uint32_t height;
    uint32_t mip_levels;
} RenderTexture;

#endif // RENDER_TEXTURE_H
