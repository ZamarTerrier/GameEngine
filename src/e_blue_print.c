#include "e_blue_print.h"

#include <vulkan/vulkan.h>

#include "graphicsObject.h"

#include "buffers.h"

#include "texture.h"

void BluePrintInit()
{

}

void BluePrintAddUniformObject(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags){

    if(blueprints->blue_print_packs[indx_pack].num_descriptors + 1 >= MAX_UNIFORMS)
    {
        printf("Слишком много декрипторов!\n");
        return;
    }

    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[blueprints->blue_print_packs[indx_pack].num_descriptors];

    descriptor->uniform.size = size;
    descriptor->descrType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = flags;
    descriptor->buffsize = size;
    descriptor->image = NULL;

    BuffersCreateUniform(&descriptor->uniform);

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;
}

void BluePrintAddUniformShadow(BluePrintDescriptor *descriptor, VkDeviceSize size, VkShaderStageFlags flags){

    descriptor->uniform.size = size;
    descriptor->descrType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = flags;
    descriptor->buffsize = size;
    descriptor->image = NULL;

    BuffersCreateUniform(&descriptor->uniform);
}

void BluePrintAddRenderImage(Blueprints *blueprints, uint32_t indx_pack, void *render)
{
    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->texture = calloc(1, sizeof(Texture2D));
    descriptor->descrType = 0x20;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->render_image = render;

    Texture2D *texture = descriptor->texture;
    texture->generated = false;

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;
}

BluePrintDescriptor *BluePrintAddTextureImage(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *image)
{
    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    if(pack->num_descriptors + 1 > MAX_UNIFORMS)
    {
        printf("Слишком много декрипторов!\n");
        return;
    }

    BluePrintDescriptor *descriptor = &pack->descriptors[pack->num_descriptors];

    descriptor->image = image;

    if(!(image->flags & ENGINE_TEXTURE_FLAG_SPECIFIC))
    {
        if(descriptor->image->size > 0)
            TextureCreate(descriptor, descriptor->image, 0);
        else
            TextureCreate(descriptor, descriptor->image, 1);

        Texture2D *texture = descriptor->texture;
        image->imgHeight = texture->image_data.texHeight;
        image->imgWidth = texture->image_data.texWidth;
    }else
    {
        if(image->flags & ENGINE_TEXTURE_FLAG_URGB)
            TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_UINT, image->imgWidth, image->imgHeight);
        else
            TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_SINT, image->imgWidth, image->imgHeight);
    }


    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;

    pack->num_descriptors ++;

    return &pack->descriptors[pack->num_descriptors - 1];
}
