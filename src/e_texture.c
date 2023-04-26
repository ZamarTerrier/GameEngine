#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "e_texture.h"

#include <vulkan/vulkan.h>

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

#include "render_texture.h"

#include "e_buffer.h"

#include "e_blue_print.h"

#include "e_math.h"
#include "e_tools.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"

//Не корректно
int ImageWriteFile(uint32_t indx)
{

    /*
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    RenderTexture *render = render_texture;

    uint32_t width = 2048, height = 2048;

    VkDeviceSize bufferSize = width * height * 4;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    ToolsTransitionImageLayout(render->frames[indx].image, render->m_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    ToolsCopyBufferToImage(stagingBuffer, render->frames[indx].image, width, height);
    ToolsTransitionImageLayout(render->frames[indx].image, render->m_format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    char *data;

    if(vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS){
         stbi_write_png("/home/ilia/temp.png", width, height, 4, data, width * 4);
         vkUnmapMemory(e_device, stagingBufferMemory);
    }

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);

    return 0;*/
}

int ImageLoadFile(ImageFileData *data, uint32_t from_file)
{
    char *pixels;

    if(from_file)
        pixels = stbi_load(data->path, &data->texWidth, &data->texHeight, &data->texChannels, STBI_rgb_alpha);
    else
        pixels = stbi_load_from_memory(data->buffer, data->buff_size, &data->texWidth, &data->texHeight, &data->texChannels, STBI_rgb_alpha);

    if(!pixels)
        return 1;

    data->data = pixels;

    return 0;
}

int ImageSetTile(const char *path, char *data, uint32_t width, uint32_t height, uint32_t tile_x, uint32_t tile_y, uint32_t tile_size)
{
    ImageFileData f_data;

    f_data.path = path;

    int res = ImageLoadFile(&f_data, 1);
    if(res)
        printf("Ошибка!\n");

    if(f_data.texChannels < 4)
    {
        free(f_data.data);
        return;
    }

    res = ImageResize(&f_data, tile_size, tile_size);
    if(res)
        printf("Ошибка!\n");

    uint32_t iter_x = 0, iter_y = 0;

    uint32_t *t_point = f_data.data;
    uint32_t *d_point = data;
    while(iter_y < f_data.texHeight)
    {
        d_point[((tile_y * tile_size * width) + (tile_x * tile_size)) + ((iter_y * width) + iter_x)] = (t_point[iter_y * f_data.texWidth + iter_x] | 0xFF000000);

        iter_x ++;

        if(iter_x >= f_data.texWidth)
        {
            iter_x = 0;
            iter_y ++;
        }
    }

    free(f_data.data);
}

int ImageResize(ImageFileData *data, uint32_t width, uint32_t height)
{
    char *pixels = calloc(width * height, sizeof(char) * data->texChannels);

    int result = stbir_resize_uint8(data->data, data->texWidth, data->texHeight, 0, pixels, width, height, 0, data->texChannels);

    if(!result)
        return 1;

    free(data->data);

    data->data = pixels;
    data->texWidth = width;
    data->texHeight = height;

    return 0;
}

void ImageCreateEmpty(Texture2D *texture) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = texture->image_data.texWidth;
    imageInfo.extent.height = texture->image_data.texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = texture->textureType;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(e_device, &imageInfo, NULL, &texture->textureImage) != VK_SUCCESS) {
        printf("failed to create image!");
        exit(-1);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(e_device, texture->textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(e_device, &allocInfo, NULL, &texture->textureImageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory!");
        exit(-1);
    }

    vkBindImageMemory(e_device, texture->textureImage, texture->textureImageMemory, 0);
}

void TextureCreateEmptyDefault(Texture2D *texture)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    texture->textureType = VK_FORMAT_R8G8B8A8_SRGB;
    texture->image_data.texWidth = EMPTY_IMAGE_WIDTH;
    texture->image_data.texHeight = EMPTY_IMAGE_HEIGHT;

    VkDeviceSize bufferSize = EMPTY_IMAGE_HEIGHT * EMPTY_IMAGE_WIDTH * 4;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    char some_data[bufferSize];
    memset(some_data, 0, bufferSize);

    uint32_t * data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, some_data, bufferSize);
    vkUnmapMemory(e_device, stagingBufferMemory);

    ImageCreateEmpty(texture);

    ToolsTransitionImageLayout(texture->textureImage, texture->textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    ToolsCopyBufferToImage(stagingBuffer, texture->textureImage, EMPTY_IMAGE_WIDTH, EMPTY_IMAGE_HEIGHT);
    ToolsTransitionImageLayout(texture->textureImage, texture->textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);

    texture->image_data.texWidth = EMPTY_IMAGE_WIDTH;
    texture->image_data.texHeight = EMPTY_IMAGE_HEIGHT;
}

void TextureCreateEmpty(Texture2D *texture)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize bufferSize = texture->image_data.texWidth * texture->image_data.texHeight * 4;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    uint32_t *data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memset(data, 0, bufferSize);
    vkUnmapMemory(e_device, stagingBufferMemory);

    ImageCreateEmpty(texture);

    ToolsTransitionImageLayout(texture->textureImage, texture->textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    ToolsCopyBufferToImage(stagingBuffer, texture->textureImage, texture->image_data.texWidth, texture->image_data.texHeight);
    ToolsTransitionImageLayout(texture->textureImage, texture->textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);
}

Texture2D *TextureFindTexture(char *path)
{
    if(path == NULL)
        return NULL;

    engine_buffered_image *buff_images = e_var_images;

    char *temp;
    for(int i=0;i < e_var_num_images;i++)
    {
        temp = buff_images[i].path;
        if(strstr(temp, path))
            return &buff_images[i].texture;
    }

    return NULL;
}

int TextureImageCreate(GameObjectImage *image, BluePrintDescriptor *descriptor, bool from_file) {

    Texture2D *temp_tex;

    engine_buffered_image *images = e_var_images;

    if(image == NULL)
    {
        descriptor->textures[descriptor->size] = &images[0].texture;
        return 0;
    }

    if(image->path == NULL && image->buffer == NULL)
    {
        descriptor->textures[descriptor->size] = &images[0].texture;
        return 0;
    }

    ImageFileData fileData;

    fileData.path = image->path;
    fileData.buffer = image->buffer;
    fileData.buff_size = image->size;

    temp_tex = TextureFindTexture(image->path);

    if(temp_tex != NULL)
    {
        descriptor->textures[descriptor->size] = TextureFindTexture(image->path);
        return 0;
    }

    ImageLoadFile(&fileData, from_file);

    int len = strlen(image->path);
    memset(images[e_var_num_images].path, 0, 2048);
    memcpy(images[e_var_num_images].path, image->path, len);
    images[e_var_num_images].texture.image_data = fileData;
    images[e_var_num_images].texture.textureType = VK_FORMAT_R8G8B8A8_SRGB;

    VkDeviceSize imageSize = fileData.texWidth * fileData.texHeight * sizeof(float);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    void* data;

    if (!fileData.data) {
        printf("failed to load texture image!");

        descriptor->textures[descriptor->size] = &images[0].texture;

        return 0;
    }

    BuffersCreate(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(e_device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, fileData.data, imageSize);
    vkUnmapMemory(e_device, stagingBufferMemory);

    TextureCreateImage( fileData.texWidth, fileData.texHeight, images[e_var_num_images].texture.textureType, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0, &images[e_var_num_images].texture.textureImage, &images[e_var_num_images].texture.textureImageMemory);

    ToolsTransitionImageLayout(images[e_var_num_images].texture.textureImage, images[e_var_num_images].texture.textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    ToolsCopyBufferToImage(stagingBuffer, images[e_var_num_images].texture.textureImage, fileData.texWidth, fileData.texHeight);
    ToolsTransitionImageLayout(images[e_var_num_images].texture.textureImage, images[e_var_num_images].texture.textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);

    return 1;
}

void TextureCreateTextureImageView(Texture2D *texture, uint32_t type) {
    texture->textureImageView = TextureCreateImageView(texture->textureImage, type, texture->textureType, VK_IMAGE_ASPECT_COLOR_BIT);
}

void TextureCreateImage(uint32_t width, uint32_t height, uint32_t format, uint32_t tiling, uint32_t usage, uint32_t properties, uint32_t flags, void** image, void** imageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;

    if(flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
        imageInfo.arrayLayers = 6;
    else
        imageInfo.arrayLayers = 1;

    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = flags;

    if (vkCreateImage(e_device, &imageInfo, NULL, image) != VK_SUCCESS) {
        printf("failed to create image!");
        exit(-1);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(e_device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(e_device, &allocInfo, NULL, imageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory!");
        exit(-1);
    }

    vkBindImageMemory(e_device, *image, *imageMemory, 0);
}

void* TextureCreateImageView(void* image, uint32_t type, uint32_t format, uint32_t aspectFlags) {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = type; //VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format; //VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(e_device, &viewInfo, NULL, &imageView) != VK_SUCCESS) {
        printf("failed to create texture image view!");
        exit(1);
    }

    return imageView;
}

void* TextureCreateImageViewCube(void* image, void **shadowCubeMapFaceImageViews, uint32_t format, uint32_t aspect_mask) {

    VkImageView *some_views = shadowCubeMapFaceImageViews;

    VkImageViewCreateInfo *viewInfo = calloc(1, sizeof(VkImageViewCreateInfo));
    viewInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo->image = image;
    viewInfo->viewType = VK_IMAGE_VIEW_TYPE_CUBE; //VK_IMAGE_VIEW_TYPE_2D;
    viewInfo->format = format; //VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo->subresourceRange.aspectMask = aspect_mask;
    viewInfo->subresourceRange.baseMipLevel = 0;
    viewInfo->subresourceRange.levelCount = 1;
    viewInfo->subresourceRange.baseArrayLayer = 0;
    viewInfo->subresourceRange.layerCount = 6;

    VkImageView imageView;
    if (vkCreateImageView(e_device, viewInfo, NULL, &imageView) != VK_SUCCESS) {
        printf("failed to create texture image view!");
        exit(1);
    }

    viewInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo->subresourceRange.layerCount = 1;
    viewInfo->image = image;

    for (uint32_t i = 0; i < 6; i++)
    {
        viewInfo->subresourceRange.baseArrayLayer = i;
        vkCreateImageView(e_device, viewInfo, NULL, &some_views[i]);
    }

    free(viewInfo);

    return imageView;
}

void TextureCreateSampler(void *sampler, uint32_t texture_type) {

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    if(texture_type == VK_FORMAT_R8G8B8A8_UINT)
    {
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
    }else{
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
    }

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties  ={};
    vkGetPhysicalDeviceProperties(e_physicalDevice, &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1.0f;

    if (vkCreateSampler(e_device, &samplerInfo, NULL, sampler) != VK_SUCCESS) {
        printf("failed to create texture sampler!");
        exit(1);
    }
}

void TextureArrayInit(Blueprints *blueprints, uint32_t size)
{
    /*
    if(blueprints->count + 1 > MAX_UNIFORMS)
    {
        printf("Слишком много декрипторов!\n");
        return;
    }

    BluePrintDescriptor *descriptor = &blueprints->descriptors[blueprints->count];

    descriptor->descrType = ENGINE_DESCRIPTOR_TYPE_IMAGE_SAMPLER;
    descriptor->descrCount = 0;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;*/
}

void TextureCreate(BluePrintDescriptor *descriptor, uint32_t type, GameObjectImage *image, bool from_file){

    int res = TextureImageCreate(image, descriptor, from_file);


    if(res)
    {
        engine_buffered_image *images = e_var_images;

        Texture2D *texture = &images[e_var_num_images].texture;

        texture->flags = 0;

        TextureCreateTextureImageView(texture, type);
        TextureCreateSampler(&texture->textureSampler, texture->textureType);

        descriptor->textures[descriptor->size] = texture;
        descriptor->size ++;

        image->imgHeight = texture->image_data.texHeight;
        image->imgWidth = texture->image_data.texWidth;

        e_var_num_images ++;
    }else{

        Texture2D *texture = descriptor->textures[descriptor->size];

        texture->flags = texture->flags;

        descriptor->size ++;

        return;
    }
}

void TextureCreateSpecific(BluePrintDescriptor *descriptor, uint32_t format, uint32_t width, uint32_t height)
{
    descriptor->textures[descriptor->size] = calloc(1, sizeof(Texture2D));

    Texture2D *texture = descriptor->textures[descriptor->size];

    texture->flags = ENGINE_TEXTURE2D_FLAG_GENERATED;
    texture->image_data.texWidth = width;
    texture->image_data.texHeight = height;
    texture->textureType = format;

    TextureCreateEmpty(texture);
    TextureCreateTextureImageView(texture, VK_IMAGE_VIEW_TYPE_2D);
    TextureCreateSampler(&texture->textureSampler, texture->textureType);

}

void TextureUpdate(BluePrintDescriptor *descriptor, void *in_data, uint32_t size_data, uint32_t offset)
{
    Texture2D *texture = descriptor->textures[0];

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize bufferSize = texture->image_data.texWidth * texture->image_data.texHeight * 4;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    uint32_t *data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memset(data, 0, bufferSize);
    memcpy(data + offset, in_data, size_data);
    vkUnmapMemory(e_device, stagingBufferMemory);

    ToolsTransitionImageLayout(texture->textureImage, texture->textureType, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    ToolsCopyBufferToImage(stagingBuffer, texture->textureImage, texture->image_data.texWidth, texture->image_data.texHeight);
    ToolsTransitionImageLayout(texture->textureImage, texture->textureType, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);
}

//Не корректно
void TextureSetTexture(BluePrintDescriptor *descriptor, const char* path){

    /*ImageDestroyTexture(descriptor->textures);

    GameObjectImage g_image;
    memset(&g_image, 0, sizeof(GameObjectImage));

    if(path != NULL)
        TextureCreate(descriptor, &g_image, 1);
    else
        TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_SINT, 100, 100);*/

}

void ImageDestroyTexture(Texture2D* texture){

    vkDestroyImage(e_device, texture->textureImage, NULL);
    vkFreeMemory(e_device, texture->textureImageMemory, NULL);
    vkDestroySampler(e_device, texture->textureSampler, NULL);
    vkDestroyImageView(e_device, texture->textureImageView, NULL);

    free(texture->image_data.data);
}
