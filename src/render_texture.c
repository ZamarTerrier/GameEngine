#include "render_texture.h"

#include <vulkan/vulkan.h>

#include "buffers.h"
#include "texture.h"

#include "tools.h"

#include "e_resource_engine.h"

void RenderTextureCreateRenderPass(RenderTexture *render)
{
    VkAttachmentDescription depthAttachment;
    depthAttachment.format = render->m_format;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference depthAttachmentRef;
    depthAttachmentRef.attachment = 0;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency depency;
    depency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depency.dstSubpass = 0;
    depency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depency.srcAccessMask = 0;
    depency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo;
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &depthAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &depency;

    if(vkCreateRenderPass(e_device, &renderPassInfo, NULL, &render->render_pass) != VK_SUCCESS)
    {
        printf("Error create render pass for render texture.");
        exit(1);
    }
}

void RenderTextureInit(RenderTexture *render)
{
    memset(render, 0, sizeof(RenderTexture));

    render->m_format = VK_FORMAT_D16_UNORM;

    render->frames = (RenderFrame*) calloc(imagesCount, sizeof(RenderFrame));

    RenderTextureCreateRenderPass(render);


    for (int i=0;i < imagesCount;i++) {

        RenderFrame *frame = &render->frames[i];

        frame->m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        TextureCreateImage(RENDER_TEXTURE_WIDTH, RENDER_TEXTURE_HEIGHT, render->m_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &frame->image, &frame->image_memory);

        TextureCreateSampler(&frame->sampler, render->m_format);
        frame->view = TextureCreateImageView(frame->image, render->m_format, VK_IMAGE_ASPECT_DEPTH_BIT);

        VkImageView attachments[] = { frame->view };

        VkFramebufferCreateInfo framebufferInfo;
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render->render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = RENDER_TEXTURE_WIDTH;
        framebufferInfo.height = RENDER_TEXTURE_HEIGHT;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(e_device, &framebufferInfo, NULL, &frame->framebufer) != VK_SUCCESS)
        {
            printf("Error create framebuffer for render texture.");
            exit(1);
        }
    }

}

void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff)
{

    RenderFrame *frame = &render->frames[imageIndex];

    if(frame->m_currentLayout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {

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

        imgBar.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        imgBar.subresourceRange.baseMipLevel = 0;
        imgBar.subresourceRange.levelCount = 1;
        imgBar.subresourceRange.baseArrayLayer = 0;
        imgBar.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(cmd_buff, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &imgBar);

        frame->m_currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkRenderPassBeginInfo renderBeginInfo;
    memset(&renderBeginInfo, 0, sizeof(VkRenderPassBeginInfo));
    renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo.renderPass = render->render_pass;
    renderBeginInfo.framebuffer = frame->framebufer;
    renderBeginInfo.renderArea.offset.x = 0;
    renderBeginInfo.renderArea.offset.y = 0;
    renderBeginInfo.renderArea.extent.width = RENDER_TEXTURE_WIDTH;
    renderBeginInfo.renderArea.extent.height = RENDER_TEXTURE_HEIGHT;

    VkClearValue clearValue;

    clearValue.depthStencil.depth = 1.0f;
    clearValue.depthStencil.stencil = 0.0;

    renderBeginInfo.clearValueCount = 1;
    renderBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(cmd_buff, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderTextureEndRendering(void *cmd_buff)
{

    vkCmdEndRenderPass(cmd_buff);
}

void RenderTextureDestroy(RenderTexture *render)
{

    for (int i=0;i<imagesCount;i++) {
        vkDestroyImage(e_device, render->frames[i].image, NULL);
        vkFreeMemory(e_device, render->frames[i].image_memory, NULL);
        vkDestroySampler(e_device, render->frames[i].sampler, NULL);
        vkDestroyImageView(e_device, render->frames[i].view, NULL);
        vkDestroyFramebuffer(e_device, render->frames[i].framebufer, NULL);
    }

    free(render->frames);

    vkDestroyRenderPass(e_device, render->render_pass, NULL);

}
