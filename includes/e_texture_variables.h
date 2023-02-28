#ifndef E_TEXTURE_VARIABLES_H
#define E_TEXTURE_VARIABLES_H

#include "engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    char *path;
    char *buffer;
    char *data;
    uint32_t buff_size;
    uint32_t texWidth;
    uint32_t texHeight;
    uint32_t texChannels;
} ImageFileData;

typedef struct{
    void *textureImage;
    void *textureImageMemory;
    void *textureImageView;
    void *textureSampler;
    ImageFileData image_data;
    bool generated;
} Texture2D;

typedef struct{
    char path[2048];
    Texture2D texture;
} engine_buffered_image;

#ifdef __cplusplus
}
#endif

#endif // E_TEXTURE_VARIABLES_H
