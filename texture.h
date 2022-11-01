#ifndef TEXTURE_H
#define TEXTURE_H

#include "stdinclude.h"

#include "e_resource.h"
#include "tools.h"

typedef struct{
    int texWidth;
    int texHeight;
    int texChannels;
    bool link;
    void* pixels;
} STBIStruct;

Texture2D createTexture(const char* file, void* stbi_info);

STBIStruct *TextureImageCreate(const char* file, Texture2D *texture, STBIStruct *stbi_info);

void createTextureImageView() ;

void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

void createTextureSampler();

void createDepthResources() ;

bool hasStencilComponent(VkFormat format);

VkFormat findDepthFormat();

VkFormat findSupportedFormat(const VkFormat* candidates, size_t countCandidates, VkImageTiling tiling, VkFormatFeatureFlags features);

void createDepthResources();

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

void copyImage(VkCommandBuffer cmdBuffer, VkImage srcImageId, VkImage dstImageId, uint32_t width, uint32_t height);

void* ImageAddTexture(localParam *local, const char* file, void *stbi_info);

void changeTexture(localParam *local, int elem, const char* file);

void destroyTexture(Texture2D* texture);

#endif // TEXTURE_H
