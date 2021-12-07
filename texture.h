#ifndef TEXTURE_H
#define TEXTURE_H

#include "stdinclude.h"

#include "resource.h"
#include "tools.h"

Texture2D createTexture(const char* file);

void createTextureImage();

void createTextureImageView() ;

void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);

VkImageView createImageView(VkImage image, VkFormat format);

void createTextureSampler();

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

void copyImage(VkCommandBuffer cmdBuffer, VkImage srcImageId, VkImage dstImageId, uint32_t width, uint32_t height);

void destroyTexture(Texture2D* texture);

#endif // TEXTURE_H
