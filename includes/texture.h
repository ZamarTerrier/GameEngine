#ifndef TEXTURE_H
#define TEXTURE_H

#include "engine_includes.h"

#include "e_blue_print.h"

#include "e_texture_variables.h"

#include "tools.h"

#define EMPTY_IMAGE_WIDTH 100
#define EMPTY_IMAGE_HEIGHT 100


#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    ENGINE_TEXTURE_FLAG_SPECIFIC = 0x1,
    ENGINE_TEXTURE_FLAG_URGB = 0x2,
} EngineTextureFlags;

int ImageWriteFile(uint32_t indx);
int ImageLoadFile(ImageFileData *data, uint32_t from_file);
int ImageSetTile(const char *path, char *data, uint32_t width, uint32_t height, uint32_t tile_x, uint32_t tile_y, uint32_t tile_size);
int ImageResize(ImageFileData *data, uint32_t width, uint32_t height);

Texture2D *TextureFindTexture(char *image);

void TextureCreateImage(uint32_t width, uint32_t height, uint32_t format, uint32_t tiling, uint32_t usage, uint32_t properties, void** image, void** imageMemory);

int TextureImageCreate(GameObjectImage *image, BluePrintDescriptor *descriptor, bool from_file);

void TextureCreateEmptyDefault(Texture2D *texture);

void TextureCreateTextureImageView(Texture2D *texture);
void* TextureCreateImageView(void* image, uint32_t format, uint32_t aspectFlags);

void TextureCreateSampler(void *sampler, uint32_t texture_type);

void TextureCreate( BluePrintDescriptor *descriptor, GameObjectImage *image, bool from_file);
void TextureCreateSpecific(BluePrintDescriptor *descriptor, uint32_t format, uint32_t width, uint32_t height);

void TextureUpdate(BluePrintDescriptor *descriptor, void *in_data, uint32_t size_data, uint32_t offset);

void TextureSetTexture(BluePrintDescriptor *descriptor, const char* path);

void ImageDestroyTexture(Texture2D* texture);

#ifdef __cplusplus
}
#endif

#endif // TEXTURE_H
