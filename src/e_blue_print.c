#include "e_blue_print.h"

#include <vulkan/vulkan.h>

#include "graphicsObject.h"

#include "render_texture.h"

#include "e_buffer.h"

#include "e_texture.h"

#include "e_resource_engine.h"

void BluePrintInit()
{

}

void BluePrintAddPushConstant(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t stage, uint32_t offset){

    BluePrintPushConstant *push_constant = &blueprints->blue_print_packs[indx_pack].push_constants[blueprints->blue_print_packs[indx_pack].num_push_constants];

    push_constant->offset = offset;
    push_constant->size = size;
    push_constant->stageFlags = stage;

    blueprints->blue_print_packs[indx_pack].num_push_constants ++;
}

void BluePrintAddUniformObject(Blueprints *blueprints, uint32_t indx_pack, uint64_t size, uint32_t flags, void *update_func, uint32_t layer_indx){

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
    descriptor->update = update_func;
    descriptor->indx_layer = layer_indx;

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
    descriptor->flags = 0;

    BuffersCreateUniform(&descriptor->uniform);
}

void BluePrintAddRenderImageArray(Blueprints *blueprints, uint32_t indx_pack, void *obj, uint32_t size)
{
    RenderTexture **renders = obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->textures = calloc(imagesCount, sizeof(Texture2D*));
    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = size;
    descriptor->size = size;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = 0;

    Texture2D **textures = descriptor->textures;

    for(int i=0;i < imagesCount;i++)
    {
        textures[i] = calloc(size, sizeof(Texture2D));

        for(int j=0;j < size; j++)
        {
            if(renders[j]->flags & ENGINE_RENDER_FLAG_ONE_SHOT)
            {
                textures[i][j].textureImageView = renders[j]->frames[0].view;
                textures[i][j].textureSampler = renders[j]->frames[0].sampler;
            }else{
                textures[i][j].textureImageView = renders[j]->frames[i].view;
                textures[i][j].textureSampler = renders[j]->frames[i].sampler;
            }
        }
    }

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;
}

void BluePrintAddRenderImageVector(Blueprints *blueprints, uint32_t indx_pack, void *obj, uint32_t size)
{
    RenderTexture **renders = obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->textures = calloc(imagesCount, sizeof(Texture2D*));
    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = size;
    descriptor->size = size;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = 0;

    Texture2D **textures = descriptor->textures;

    for(int i=0;i < imagesCount;i++)
    {
        textures[i] = calloc(size, sizeof(Texture2D));

        for(int j=0;j < size; j++)
        {
            if(renders[j]->flags & ENGINE_RENDER_FLAG_ONE_SHOT)
            {
                textures[i][j].textureImageView = TextureCreateImageView(renders[j]->frames[0].image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, renders[j]->type == ENGINE_RENDER_TYPE_DEPTH ? findDepthFormat() : swapChainImageFormat, renders[j]->type == ENGINE_RENDER_TYPE_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
                textures[i][j].textureSampler = renders[j]->frames[0].sampler;
            }else{
                textures[i][j].textureImageView = TextureCreateImageView(renders[j]->frames[i].image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, renders[j]->type == ENGINE_RENDER_TYPE_DEPTH ? findDepthFormat() : swapChainImageFormat, renders[j]->type == ENGINE_RENDER_TYPE_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
                textures[i][j].textureSampler = renders[j]->frames[i].sampler;
            }
            textures[i][j].flags |= ENGINE_TEXTURE2D_FLAG_VIEW;
        }
    }

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;
}

void BluePrintAddRenderImageCube(Blueprints *blueprints, uint32_t indx_pack, uint32_t indx_cube, void *obj)
{
    RenderTexture *render = obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->textures = calloc(imagesCount, sizeof(Texture2D*));
    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = 0;

    Texture2D **textures = descriptor->textures;

    for(int i=0;i < imagesCount;i++)
    {
        textures[i] = calloc(1, sizeof(Texture2D));

        if(render->flags & ENGINE_RENDER_FLAG_ONE_SHOT)
        {
            textures[i][0].textureImageView = render->frames[0].shadowCubeMapFaceImageViews[indx_cube];
            textures[i][0].textureSampler = render->frames[0].sampler;
        }else{
            textures[i][0].textureImageView = render->frames[i].shadowCubeMapFaceImageViews[indx_cube];
            textures[i][0].textureSampler = render->frames[i].sampler;
        }
        textures[i][0].flags = 0;

    }

    blueprints->blue_print_packs[indx_pack].num_descriptors ++;
}

void BluePrintAddRenderImage(Blueprints *blueprints, uint32_t indx_pack, void *obj)
{
    RenderTexture *render = obj;

    uint32_t nums = blueprints->blue_print_packs[indx_pack].num_descriptors;
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[nums];

    descriptor->textures = calloc(imagesCount, sizeof(Texture2D*));
    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = 0;

    Texture2D **textures = descriptor->textures;

    for(int i=0;i < imagesCount;i++)
    {
        textures[i] = calloc(1, sizeof(Texture2D));

        if(render->flags & ENGINE_RENDER_FLAG_ONE_SHOT)
        {
            textures[i][0].textureImageView = render->frames[0].view;
            textures[i][0].textureSampler = render->frames[0].sampler;
        }else{
            textures[i][0].textureImageView = render->frames[i].view;
            textures[i][0].textureSampler = render->frames[i].sampler;
        }
        textures[i][0].flags = 0;

    }

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
    descriptor->size = 0;
    descriptor->textures = calloc(1, sizeof(Texture2D *));

    if(!(image->flags & ENGINE_TEXTURE_FLAG_SPECIFIC))
    {
        if(descriptor->image->size > 0)
            TextureCreate(descriptor, VK_IMAGE_VIEW_TYPE_2D, descriptor->image, 0);
        else
            TextureCreate(descriptor, VK_IMAGE_VIEW_TYPE_2D, descriptor->image, 1);

    }else
    {
        if(image->flags & ENGINE_TEXTURE_FLAG_URGB)
            TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_UINT, image->imgWidth, image->imgHeight);
        else
            TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_SINT, image->imgWidth, image->imgHeight);
    }


    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE;

    pack->num_descriptors ++;

    return &pack->descriptors[pack->num_descriptors - 1];
}

void BluePrintAddTextureImageArray(Blueprints *blueprints, uint32_t indx_pack, GameObjectImage *images, uint32_t size)
{
    BluePrintPack *pack = &blueprints->blue_print_packs[indx_pack];

    if(pack->num_descriptors + 1 > MAX_UNIFORMS)
    {
        printf("Слишком много декрипторов!\n");
        return;
    }

    BluePrintDescriptor *descriptor = &pack->descriptors[pack->num_descriptors];

    descriptor->image = images;
    descriptor->size = 0;
    descriptor->textures = calloc(size, sizeof(Texture2D *));

    for(int i=0;i < size;i++)
    {
        if(!(images[i].flags & ENGINE_TEXTURE_FLAG_SPECIFIC))
        {
            if(descriptor->image->size > 0)
                TextureCreate(descriptor, VK_IMAGE_VIEW_TYPE_2D, &images[i], 0);
            else
                TextureCreate(descriptor, VK_IMAGE_VIEW_TYPE_2D, &images[i], 1);

        }else
        {
            if(images[i].flags & ENGINE_TEXTURE_FLAG_URGB)
                TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_UINT, images[i].imgWidth, images[i].imgHeight);
            else
                TextureCreateSpecific(descriptor, VK_FORMAT_R8G8B8A8_SINT, images[i].imgWidth, images[i].imgHeight);
        }
    }

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = descriptor->size;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->flags = ENGINE_BLUE_PRINT_FLAG_ARRAY_IMAGE | ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE;

    pack->num_descriptors ++;
}
