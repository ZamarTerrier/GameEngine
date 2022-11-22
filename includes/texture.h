#ifndef TEXTURE_H
#define TEXTURE_H

#include "engine_includes.h"

#include "e_texture_variables.h"

#include "e_resource.h"
#include "tools.h"

void TextureCreate( void *texture, GameObjectImage *image, bool from_file);

int TextureImageCreate(GameObjectImage *image, Texture2D *texture, bool from_file);

void createTextureImageView() ;

void createImage(uint32_t width, uint32_t height, uint32_t format, uint32_t tiling, uint32_t usage, uint32_t properties, void** image, void** imageMemory);

void* createImageView(void* image, uint32_t format, uint32_t aspectFlags);

void createTextureSampler(Texture2D *texture);

void createDepthResources() ;

bool hasStencilComponent(uint32_t format);

uint32_t findDepthFormat();

uint32_t findSupportedFormat(const uint32_t* candidates, size_t countCandidates, uint32_t tiling, uint32_t features);

void createDepthResources();

void transitionImageLayout(void* image, uint32_t format, uint32_t oldLayout, uint32_t newLayout);

void copyBufferToImage(void* buffer, void* image, uint32_t width, uint32_t height);

void copyImage(void* cmdBuffer, void* srcImageId, void* dstImageId, uint32_t width, uint32_t height);

void ImageAddTexture(localParam *local, GameObjectImage *image);

void changeTexture(localParam *local, int elem, const char* source, int size);

void ImageDestroyTexture(Texture2D* texture);

#endif // TEXTURE_H
