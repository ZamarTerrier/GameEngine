#ifndef E_TEXTURE_VARIABLES_H
#define E_TEXTURE_VARIABLES_H

typedef struct{
    void *textureImage;
    void *textureImageMemory;
    void *textureImageView;
    void *textureSampler;
    int texWidth;
    int texHeight;
} Texture2D;

typedef struct{
    char *path;
    int texWidth;
    int texHeight;
    int texChannels;
    void *pixels;
    Texture2D texture;
} engine_buffered_image;

#endif // E_TEXTURE_VARIABLES_H
