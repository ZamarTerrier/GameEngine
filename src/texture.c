#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

#include <vulkan/vulkan.h>

#include "stb_image.h"

void ImageCreateEmpty(void** image, void** imageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = 100;
    imageInfo.extent.height = 100;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, NULL, image) != VK_SUCCESS) {
        printf("failed to create image!");
        exit(-1);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(device, &allocInfo, NULL, imageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory!");
        exit(-1);
    }

    vkBindImageMemory(device, *image, *imageMemory, 0);
}

Texture2D createTexture(const char* source, int size, bool from_file){

    Texture2D texture;

    int res = TextureImageCreate(source, size, &texture, from_file);

    createTextureImageView(&texture);
    createTextureSampler(&texture);

    return texture;

}

int TextureImageCreate(const char* source, int size, Texture2D *texture, bool from_file) {

    if(source == NULL)
    {
        ImageCreateEmpty(&texture->textureImage, &texture->textureImageMemory);
        transitionImageLayout(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        transitionImageLayout(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        return;
    }

    int texWidth, texHeight, texChannels;

    bool find = false;
    stbi_uc *pixels;

    if(from_file)
    {
        engine_buffered_image *images = e_var_images;

        for(int i=0; i < e_var_num_images;i++)
        {
            if(ToolsCmpStrings(images[i].path, source))
            {
                pixels = images[i].pixels;
                texWidth = images[i].texWidth;
                texHeight = images[i].texHeight;
                texChannels = images[i].texChannels;

                find = true;
            }
        }

        if(!find)
        {
            pixels = stbi_load(source, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

            e_var_num_images++;


            e_var_images = realloc(e_var_images, e_var_num_images * sizeof(engine_buffered_image));

            images = e_var_images;

            int len = strlen(source);
            images[e_var_num_images - 1].path = calloc(len + 1, sizeof(char));
            memcpy(images[e_var_num_images - 1].path, source, len);
            images[e_var_num_images - 1].path[len] = '\0';
            images[e_var_num_images - 1].texWidth = texWidth;
            images[e_var_num_images - 1].texChannels = texChannels;
            images[e_var_num_images - 1].texHeight = texHeight;
            images[e_var_num_images - 1].pixels = pixels;
        }
    }
    else
    {

        engine_buffered_image *images = e_var_images;

        for(int i=0; i < e_var_num_images;i++)
        {
            if(ToolsCmpStrings(images[i].path, source))
            {
                pixels = images[i].pixels;
                texWidth = images[i].texWidth;
                texHeight = images[i].texHeight;
                texChannels = images[i].texChannels;

                find = true;
            }
        }

        if(!find)
        {
            pixels = stbi_load_from_memory(source, size, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

            e_var_num_images++;

            e_var_images = realloc(e_var_images, e_var_num_images * sizeof(engine_buffered_image));

            images = e_var_images;

            int len = strlen(source);
            images[e_var_num_images - 1].path = calloc(len + 1, sizeof(char));
            memcpy(images[e_var_num_images - 1].path, source, len);
            images[e_var_num_images - 1].path[len] = '\0';
            images[e_var_num_images - 1].texWidth = texWidth;
            images[e_var_num_images - 1].texChannels = texChannels;
            images[e_var_num_images - 1].texHeight = texHeight;
            images[e_var_num_images - 1].pixels = pixels;
        }
    }

    VkDeviceSize imageSize = texWidth * texHeight * sizeof(float);

    texture->texHeight = texHeight;
    texture->texWidth = texWidth;

    if (!pixels) {
        printf("failed to load texture image!");
        ImageCreateEmpty(&texture->textureImage, &texture->textureImageMemory);
        transitionImageLayout(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        transitionImageLayout(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        return 0;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BuffersCreate(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createImage( texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->textureImage, &texture->textureImageMemory);

    transitionImageLayout(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, texture->textureImage, texWidth, texHeight);
    transitionImageLayout(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    return 0;
}

void createTextureImageView(Texture2D *texture) {
    texture->textureImageView = createImageView(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void createImage(uint32_t width, uint32_t height, uint32_t format, uint32_t tiling, uint32_t usage, uint32_t properties, void** image, void** imageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, NULL, image) != VK_SUCCESS) {
        printf("failed to create image!");
        exit(-1);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, NULL, imageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory!");
        exit(-1);
    }

    vkBindImageMemory(device, *image, *imageMemory, 0);
}

void* createImageView(void* image, uint32_t format, uint32_t aspectFlags) {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, NULL, &imageView) != VK_SUCCESS) {
        printf("failed to create texture image view!");
        exit(1);
    }

    return imageView;
}

void createTextureSampler(Texture2D *texture) {

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties  ={};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(device, &samplerInfo, NULL, &texture->textureSampler) != VK_SUCCESS) {
        printf("failed to create texture sampler!");
        exit(1);
    }
}

void createDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthImage, &depthImageMemory);
    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

bool hasStencilComponent(uint32_t format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

uint32_t findDepthFormat() {

    VkFormat formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};

    return findSupportedFormat( formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
}

uint32_t findSupportedFormat(const uint32_t* candidates, size_t countCandidates, uint32_t tiling, uint32_t features) {

    for (int i=0;i < countCandidates;i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return candidates[i];
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return candidates[i];
        }
    }

}

void transitionImageLayout(void* image, uint32_t format, uint32_t oldLayout, uint32_t newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        printf("Failed to transition image layout from 0x%X to 0x%X", oldLayout, newLayout);
        exit(1);
    }

    vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, NULL,
                0, NULL,
                1, &barrier
                );

    endSingleTimeCommands(commandBuffer);
}

void copyBufferToImage(void *buffer, void *image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;
    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    vkCmdCopyBufferToImage( commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void copyImage(void *cmdBuffer, void *srcImageId, void * dstImageId, uint32_t width, uint32_t height)
{

    VkImageSubresourceLayers subResource = {};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subResource.baseArrayLayer = 0;
    subResource.layerCount = 1;
    subResource.mipLevel = 0;

    VkImageCopy region = {};
    region.srcOffset.x = 0;
    region.srcOffset.y = 0;
    region.srcOffset.z = 0;
    region.srcSubresource = subResource;
    region.dstOffset.x = 0;
    region.dstOffset.y = 0;
    region.dstOffset.z = 0;
    region.dstSubresource = subResource;
    region.extent.width = width;
    region.extent.height = height;
    region.extent.depth = 1;

    vkCmdCopyImage( cmdBuffer, srcImageId, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImageId, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, &region);

}

void ImageAddTexture(localParam *local, GameObjectImage *image){

    local->descrCount ++;

    local->descriptors = (ShaderBuffer *) realloc(local->descriptors, local->descrCount * sizeof(ShaderBuffer));

    ShaderBuffer *descriptor = &local->descriptors[local->descrCount - 1];

    descriptor->image = image;
    Texture2D temptexture;

    if(image->size > 0)
        temptexture = createTexture(descriptor->image->buffer, descriptor->image->size, 0);
    else
        temptexture = createTexture(descriptor->image->path, 0, 1);

    descriptor->texture = calloc(1, sizeof(Texture2D));
    memcpy(descriptor->texture, &temptexture, sizeof(Texture2D));

    image->imgHeight = temptexture.texHeight;
    image->imgWidth = temptexture.texWidth;

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;

}



void changeTexture(localParam *local, int elem, const char* source, int size){

    /*destroyTexture(local->descriptors[elem].texture);

    if(size > 0)
        local->descriptors[elem].texture = createTexture(source, size, 0);
    else
        local->descriptors[elem].texture = createTexture(source, 0, 1);*/

}

void destroyTexture(Texture2D* texture){

    vkDestroySampler(device, texture->textureSampler, NULL);
    vkDestroyImageView(device, texture->textureImageView, NULL);

    vkDestroyImage(device, texture->textureImage, NULL);
    vkFreeMemory(device, texture->textureImageMemory, NULL);

}
