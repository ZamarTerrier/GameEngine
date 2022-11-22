#ifndef E_TEXTURE_VARIABLES_H
#define E_TEXTURE_VARIABLES_H

#include "engine_includes.h"

typedef struct{
    void *textureImage;
    void *textureImageMemory;
    void *textureImageView;
    void *textureSampler;
    int texWidth;
    int texHeight;
    bool linked;
} Texture2D;

typedef struct{
    char *path;
    int texWidth;
    int texHeight;
    int texChannels;
    void *pixels;
    Texture2D *texture;
} engine_buffered_image;

#endif // E_TEXTURE_VARIABLES_H
