#include "render_texture.h"

#include <vulkan/vulkan.h>

#include "buffers.h"
#include "texture.h"

#include "e_math.h"

#include "tools.h"

#include "e_resource_engine.h"

void RenderTextureCreateRenderPass(RenderTexture *render)
{
    VkAttachmentDescription *colorAttachment = calloc(1, sizeof(VkAttachmentDescription));
    colorAttachment->format = render->m_format;
    colorAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment->initialLayout = render->type == ENGINE_RENDER_TYPE_IMAGE ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment->finalLayout = render->type == ENGINE_RENDER_TYPE_IMAGE ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription *depthAttachment = calloc(1, sizeof(VkAttachmentDescription));
    depthAttachment->format = findDepthFormat();
    depthAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment->initialLayout = render->type == ENGINE_RENDER_TYPE_DEPTH ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment->finalLayout = render->type == ENGINE_RENDER_TYPE_DEPTH ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkAttachmentReference *colorAttachmentRef = calloc(1, sizeof(VkAttachmentReference));
    colorAttachmentRef->attachment = 0;
    colorAttachmentRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference *depthAttachmentRef = calloc(1, sizeof(VkAttachmentReference));
    depthAttachmentRef->attachment = render->type  == ENGINE_RENDER_TYPE_DEPTH ? 0 : 1;
    depthAttachmentRef->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    memset(&subpass, 0, sizeof(VkSubpassDescription));
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    if(render->type != ENGINE_RENDER_TYPE_IMAGE)
        subpass.pDepthStencilAttachment = depthAttachmentRef;

    if(render->type != ENGINE_RENDER_TYPE_DEPTH){
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = colorAttachmentRef;
    }

    VkRenderPassCreateInfo renderPassInfo;
    memset(&renderPassInfo, 0, sizeof(VkRenderPassCreateInfo));


    VkSubpassDependency *depency = calloc( render->type != ENGINE_RENDER_TYPE_DEPTH ? 2 : 1, sizeof(VkSubpassDependency));

    if(render->type != ENGINE_RENDER_TYPE_DEPTH){


        depency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        depency[0].dstSubpass = 0;
        depency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        depency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        depency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT; //0;
        depency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        depency[1].srcSubpass = 0;
        depency[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        depency[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        depency[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        depency[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        if(render->type == ENGINE_RENDER_TYPE_WINDOW)
        {
            VkAttachmentDescription attachments[] = {*colorAttachment, *depthAttachment};

            renderPassInfo.attachmentCount = 2;
            renderPassInfo.pDependencies = depency;
            renderPassInfo.pAttachments = attachments;
        }else{
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pDependencies = depency;
            renderPassInfo.pAttachments = colorAttachment;
        }

    }else{

        depency->srcSubpass = VK_SUBPASS_EXTERNAL;
        depency->dstSubpass = 0;
        depency->srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        depency->srcAccessMask = 0;
        depency->dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        depency->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pDependencies = depency;
        renderPassInfo.pAttachments = depthAttachment;
    }


    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;

    if(vkCreateRenderPass(e_device, &renderPassInfo, NULL, &render->render_pass) != VK_SUCCESS)
    {
        printf("Error create render pass for render texture.");
        exit(1);
    }

    free(colorAttachment);
    free(depthAttachment);
    free(colorAttachmentRef);
    free(depthAttachmentRef);
    free(depency);
}

void RenderTextureInit(RenderTexture *render, uint32_t type, uint32_t width, uint32_t height)
{
    memset(render, 0, sizeof(RenderTexture));

    render->m_format = type == ENGINE_RENDER_TYPE_DEPTH ? findDepthFormat() : swapChainImageFormat;

    render->frames = (RenderFrame*) calloc(imagesCount, sizeof(RenderFrame));

    render->type = type;
    render->mip_levels = floor(log2(e_max(width, height))) + 1;;

    RenderTextureCreateRenderPass(render);

    if(type != ENGINE_RENDER_TYPE_WINDOW )
    {
        render->height = height;
        render->width = width;
    }else{
        render->height = swapChainExtent.height;
        render->width = swapChainExtent.width;
    }


    for (int i=0;i < imagesCount;i++) {

        RenderFrame *frame = &render->frames[i];

        frame->m_currentLayout = type == ENGINE_RENDER_TYPE_WINDOW ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;

        if(type != ENGINE_RENDER_TYPE_WINDOW)
        {
            uint32_t usage = type == ENGINE_RENDER_TYPE_DEPTH ?  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT :  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

            TextureCreateImage(render->width, render->height, render->m_format, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &frame->image, &frame->image_memory);

            TextureCreateSampler(&frame->sampler, render->m_format);
            frame->view = TextureCreateImageView(frame->image, render->m_format, type == ENGINE_RENDER_TYPE_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

            VkImageMemoryBarrier imgBar;

            imgBar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imgBar.pNext = NULL;
            imgBar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imgBar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imgBar.srcAccessMask = 0;
            imgBar.dstAccessMask = 0;
            imgBar.oldLayout = frame->m_currentLayout;
            imgBar.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imgBar.image = frame->image;

            imgBar.subresourceRange.aspectMask = type == ENGINE_RENDER_TYPE_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
            imgBar.subresourceRange.baseMipLevel = 0;
            imgBar.subresourceRange.levelCount = 1;
            imgBar.subresourceRange.baseArrayLayer = 0;
            imgBar.subresourceRange.layerCount = 1;

            void *cmd_buff = beginSingleTimeCommands();

            vkCmdPipelineBarrier(cmd_buff, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &imgBar);

            endSingleTimeCommands(cmd_buff);

        }else
            frame->view = swapChainImageViews[i];


        VkFramebufferCreateInfo *framebufferInfo = calloc(1, sizeof(VkFramebufferCreateInfo));
        framebufferInfo->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo->renderPass = render->render_pass;
        framebufferInfo->width = render->width;
        framebufferInfo->height = render->height;
        framebufferInfo->layers = 1;

        if(type == ENGINE_RENDER_TYPE_WINDOW)
        {
            VkImageView attachments[2];
            attachments[0] = frame->view;
            attachments[1] = depthImageView;

            framebufferInfo->attachmentCount = 2;
            framebufferInfo->pAttachments = attachments;
        }else if(type == ENGINE_RENDER_TYPE_IMAGE)
        {
            VkImageView attachment[] = { frame->view };

            framebufferInfo->attachmentCount = 1;
            framebufferInfo->pAttachments = attachment;
        }else{
            VkImageView attachment[] = { frame->view };

            framebufferInfo->attachmentCount = 1;
            framebufferInfo->pAttachments = attachment;
        }

        if(vkCreateFramebuffer(e_device, framebufferInfo, NULL, &frame->framebufer) != VK_SUCCESS)
        {
            printf("Error create framebuffer for render texture.");
            exit(1);
        }

        free(framebufferInfo);
    }

}

void RenderTextureRecreate(RenderTexture *render)
{
    RenderTextureDestroy(render);

    RenderTextureInit(render, render->type, render->width, render->height);
}

void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff)
{

    RenderFrame *frame = &render->frames[imageIndex];

    VkRenderPassBeginInfo renderBeginInfo;
    memset(&renderBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.renderPass = render->render_pass;
    renderBeginInfo.framebuffer = frame->framebufer;
    renderBeginInfo.renderArea.offset.x = 0;
    renderBeginInfo.renderArea.offset.y = 0;
    renderBeginInfo.renderArea.extent.width = render->width;
    renderBeginInfo.renderArea.extent.height = render->height;

    if(render->type == ENGINE_RENDER_TYPE_DEPTH)
    {
        VkClearValue clearValue;

        clearValue.depthStencil.depth = 1.0f;
        clearValue.depthStencil.stencil = 0.0;

        renderBeginInfo.clearValueCount = 1;
        renderBeginInfo.pClearValues = &clearValue;
    }else{
        VkClearValue clearValues[2];

        clearValues[0].color.float32[0] = 0.8f;
        clearValues[0].color.float32[1] = 0.1f;
        clearValues[0].color.float32[2] = 0.1f;
        clearValues[0].color.float32[3] = 1.0f;
        clearValues[1].depthStencil.depth = 1.0f;
        clearValues[1].depthStencil.stencil = 0;

        renderBeginInfo.clearValueCount = 2;
        renderBeginInfo.pClearValues = clearValues;
    }

    vkCmdBeginRenderPass(cmd_buff, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderTextureEndRendering(void *cmd_buff)
{

    vkCmdEndRenderPass(cmd_buff);
}

void RenderTextureDestroy(RenderTexture *render)
{


    for (int i=0;i<imagesCount;i++) {

        if(render->type != ENGINE_RENDER_TYPE_WINDOW)
        {
            vkDestroyImage(e_device, render->frames[i].image, NULL);
            vkFreeMemory(e_device, render->frames[i].image_memory, NULL);
            vkDestroySampler(e_device, render->frames[i].sampler, NULL);
            vkDestroyImageView(e_device, render->frames[i].view, NULL);
        }

        vkDestroyFramebuffer(e_device, render->frames[i].framebufer, NULL);
    }

    free(render->frames);

    vkDestroyRenderPass(e_device, render->render_pass, NULL);

}
