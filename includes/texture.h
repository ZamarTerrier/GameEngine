#ifndef TEXTURE_H
#define TEXTURE_H

#include "engine_includes.h"

#include "e_texture_variables.h"

#include "tools.h"

#define EMPTY_IMAGE_WIDTH 100
#define EMPTY_IMAGE_HEIGHT 100


#ifdef __cplusplus
extern "C"
{
#endif

int ImageResize(ImageFileData *data, uint32_t width, uint32_t height);

int ImageLoadFile(ImageFileData *data, uint32_t from_file);

Texture2D *TextureFindTexture(char *image);

void TextureCreateImage(uint32_t width, uint32_t height, uint32_t format, uint32_t tiling, uint32_t usage, uint32_t properties, void** image, void** imageMemory);

int TextureImageCreate(GameObjectImage *image, ShaderBuffer *descriptor, bool from_file);

void* TextureCreateImageView(void* image, uint32_t format, uint32_t aspectFlags);

void TextureCreateSampler(Texture2D *texture);

ShaderBuffer *TextureImageAdd(localParam *local, GameObjectImage *image, uint32_t width, uint32_t height);

void TextureCreate( ShaderBuffer *descriptor, GameObjectImage *image, bool from_file);
void TextureCreateSpecific(ShaderBuffer *descriptor, uint32_t width, uint32_t height);

void TextureUpdate(ShaderBuffer *descriptor, void *in_data, uint32_t size_data, uint32_t offset);

void TextureSetTexture(ShaderBuffer *descriptor, const char* path);

void ImageDestroyTexture(Texture2D* texture);

#ifdef __cplusplus
}
#endif

#endif // TEXTURE_H
