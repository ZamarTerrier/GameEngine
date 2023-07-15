#include "render_texture.h"

#include "engine.h"

#include <vulkan/vulkan.h>

#include "lightObject.h"

#include "e_buffer.h"
#include "e_texture.h"

#include "e_math.h"

#include "e_tools.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"

void RenderTextureCreateDepthResource(RenderTexture *render, RenderFrame *frame)
{

    VkFormat depthFormat = findDepthFormat();

    TextureCreateImage(render->width, render->height, 1,depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0, &frame->depth_image, &frame->depth_memory);
    frame->depth_view = TextureCreateImageView(frame->depth_image, VK_IMAGE_VIEW_TYPE_2D, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

    ToolsTransitionImageLayout(frame->depth_image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

}

void RenderTextureCreateRenderPass(RenderTexture *render, void **render_pass)
{

    if(render->m_format == 0)
    {
        VkSubpassDescription subpassDescription;
        memset(&subpassDescription, 0, sizeof(VkSubpassDescription));

        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        // Geometry render pass doesn't need any output attachment.
        VkRenderPassCreateInfo renderPassInfo;
        memset(&renderPassInfo, 0, sizeof(VkRenderPassCreateInfo));

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 0;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescription;

        if(vkCreateRenderPass(e_device, &renderPassInfo, NULL, render_pass) != VK_SUCCESS)
        {
            printf("Error create render pass for render texture.");
            exit(1);
        }

        return;
    }

    VkAttachmentDescription *colorAttachment = calloc(1, sizeof(VkAttachmentDescription));
    colorAttachment->format = render->m_format;
    colorAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment->initialLayout = render->type == ENGINE_RENDER_TYPE_IMAGE || render->type == ENGINE_RENDER_TYPE_CUBEMAP ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment->finalLayout = render->type == ENGINE_RENDER_TYPE_IMAGE || render->type == ENGINE_RENDER_TYPE_CUBEMAP ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription *depthAttachment = calloc(1, sizeof(VkAttachmentDescription));
    depthAttachment->format = findDepthFormat();
    depthAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    if(render->type == ENGINE_RENDER_TYPE_DEPTH || (render->flags & ENGINE_RENDER_FLAG_DEPTH))
    {
        depthAttachment->initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        depthAttachment->finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }else{
        depthAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    }

    VkAttachmentReference *colorAttachmentRef = calloc(1, sizeof(VkAttachmentReference));
    colorAttachmentRef->attachment = 0;//Номер атачмента
    colorAttachmentRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference *depthAttachmentRef = calloc(1, sizeof(VkAttachmentReference));
    depthAttachmentRef->attachment = render->type  == ENGINE_RENDER_TYPE_DEPTH || (render->flags & ENGINE_RENDER_FLAG_DEPTH) ? 0 : 1;//Номер атачмента
    depthAttachmentRef->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    memset(&subpass, 0, sizeof(VkSubpassDescription));
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    if(render->type != ENGINE_RENDER_TYPE_IMAGE)
        subpass.pDepthStencilAttachment = depthAttachmentRef;

    if(render->type != ENGINE_RENDER_TYPE_DEPTH && !(render->flags & ENGINE_RENDER_FLAG_DEPTH)){
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = colorAttachmentRef;
    }

    VkRenderPassCreateInfo renderPassInfo;
    memset(&renderPassInfo, 0, sizeof(VkRenderPassCreateInfo));

    VkSubpassDependency *depency = calloc( render->type != ENGINE_RENDER_TYPE_DEPTH ? 2 : 1, sizeof(VkSubpassDependency));

    if(render->type == ENGINE_RENDER_TYPE_CUBEMAP)
    {
        if(render->flags & ENGINE_RENDER_FLAG_DEPTH)
        {
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments = depthAttachment;
        }else{
            VkAttachmentDescription attachments[] = {*colorAttachment, *depthAttachment};

            renderPassInfo.attachmentCount = 2;
            renderPassInfo.pAttachments = attachments;
        }


    }else if(render->type != ENGINE_RENDER_TYPE_DEPTH){


        depency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        depency[0].dstSubpass = 0;
        depency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        depency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        depency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
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
    renderPassInfo.dependencyCount = render->type == ENGINE_RENDER_TYPE_CUBEMAP ? 0 : 1;

    if(vkCreateRenderPass(e_device, &renderPassInfo, NULL, render_pass) != VK_SUCCESS)
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

void RenderTextureTransitionLayer(RenderFrame *frame, uint32_t render_type, uint32_t type_layout, uint32_t aspect_mask)
{
    VkImageMemoryBarrier imgBar;

    imgBar.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imgBar.pNext = NULL;
    imgBar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imgBar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imgBar.srcAccessMask = 0;
    imgBar.dstAccessMask = 0;
    imgBar.oldLayout = frame->m_currentLayout;
    imgBar.newLayout = type_layout;
    imgBar.image = frame->image;

    imgBar.subresourceRange.aspectMask = aspect_mask;//render_type == ENGINE_RENDER_TYPE_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imgBar.subresourceRange.baseMipLevel = 0;
    imgBar.subresourceRange.levelCount = 1;
    imgBar.subresourceRange.baseArrayLayer = 0;

    imgBar.subresourceRange.layerCount = render_type == ENGINE_RENDER_TYPE_CUBEMAP ? 6 : 1;

    void *cmd_buff = beginSingleTimeCommands();

    vkCmdPipelineBarrier(cmd_buff, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &imgBar);

    endSingleTimeCommands(cmd_buff);
}

void RenderTextureCreateFrames(RenderTexture *render, uint32_t flags)
{
    for (int i=0;i < render->num_frames;i++) {

        RenderFrame *frame = &render->frames[i];

        if(render->m_format != 0)
        {
            frame->m_currentLayout = render->type == ENGINE_RENDER_TYPE_WINDOW ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;

            if(render->type != ENGINE_RENDER_TYPE_WINDOW)
            {
                uint32_t usage = render->type == ENGINE_RENDER_TYPE_DEPTH || (render->flags & ENGINE_RENDER_FLAG_DEPTH) ?  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT :  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

                TextureCreateImage(render->width, render->height, 1,render->m_format, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, flags, &frame->image, &frame->image_memory);

                TextureCreateSampler(&frame->sampler, render->m_format, 1);

                if(render->type & ENGINE_RENDER_TYPE_CUBEMAP)
                {
                    frame->shadowCubeMapFaceImageViews = calloc(6, sizeof(VkImageView));
                    frame->view = TextureCreateImageViewCube(frame->image, frame->shadowCubeMapFaceImageViews, render->m_format, render->flags & ENGINE_RENDER_FLAG_DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
                    frame->framebufers = calloc(6, sizeof(VkFramebuffer));
                }else{
                    frame->view = TextureCreateImageView(frame->image, VK_IMAGE_VIEW_TYPE_2D, render->m_format, render->type == ENGINE_RENDER_TYPE_DEPTH || (render->flags & ENGINE_RENDER_FLAG_DEPTH) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT, 1);
                    frame->framebufers = calloc(1, sizeof(VkFramebuffer));
                }

                RenderTextureCreateDepthResource(render, frame);

                RenderTextureTransitionLayer(frame, render->type, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, render->type == ENGINE_RENDER_TYPE_DEPTH || (render->flags & ENGINE_RENDER_FLAG_DEPTH) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

            }else{
                frame->view = swapChainImageViews[i];
                frame->framebufers = calloc(1, sizeof(VkFramebuffer));
            }
        }else{
            frame->m_currentLayout = VK_IMAGE_LAYOUT_GENERAL;

            frame->framebufers = calloc(1, sizeof(VkFramebuffer));
        }

        VkFramebufferCreateInfo *framebufferInfo = calloc(1, sizeof(VkFramebufferCreateInfo));
        framebufferInfo->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo->renderPass = render->render_pass;
        framebufferInfo->width = render->width;
        framebufferInfo->height = render->height;
        framebufferInfo->layers = 1;

        if(render->type == ENGINE_RENDER_TYPE_GEOMETRY)
        {
            if(vkCreateFramebuffer(e_device, framebufferInfo, NULL, &frame->framebufers[0]) != VK_SUCCESS)
            {
                printf("Error create framebuffer for render texture.");
                exit(1);
            }
        }else if(render->type == ENGINE_RENDER_TYPE_WINDOW)
        {
            VkImageView attachments[2];
            attachments[0] = frame->view;
            attachments[1] = depthImageView;

            framebufferInfo->attachmentCount = 2;
            framebufferInfo->pAttachments = attachments;

            if(vkCreateFramebuffer(e_device, framebufferInfo, NULL, &frame->framebufers[0]) != VK_SUCCESS)
            {
                printf("Error create framebuffer for render texture.");
                exit(1);
            }

        }else if(render->type == ENGINE_RENDER_TYPE_CUBEMAP){


            if(render->flags & ENGINE_RENDER_FLAG_DEPTH)
            {
                for(int j=0;j < 6;j++)
                {
                    VkImageView attachments[] = { frame->shadowCubeMapFaceImageViews[j] };

                    framebufferInfo->attachmentCount = 1;
                    framebufferInfo->pAttachments = attachments;

                    if(vkCreateFramebuffer(e_device, framebufferInfo, NULL, &frame->framebufers[j]) != VK_SUCCESS)
                    {
                        printf("Error create framebuffer for render texture.");
                        exit(1);
                    }
                }
            }else{
                VkImageView attachments[2];
                attachments[1] = frame->depth_view;

                for(int j=0;j < 6;j++)
                {
                    attachments[0] = frame->shadowCubeMapFaceImageViews[j];

                    framebufferInfo->attachmentCount = 2;
                    framebufferInfo->pAttachments = attachments;

                    if(vkCreateFramebuffer(e_device, framebufferInfo, NULL, &frame->framebufers[j]) != VK_SUCCESS)
                    {
                        printf("Error create framebuffer for render texture.");
                        exit(1);
                    }
                }
            }

        }else{
            VkImageView attachment[] = { frame->view };

            framebufferInfo->attachmentCount = 1;
            framebufferInfo->pAttachments = attachment;

            if(vkCreateFramebuffer(e_device, framebufferInfo, NULL, &frame->framebufers[0]) != VK_SUCCESS)
            {
                printf("Error create framebuffer for render texture.");
                exit(1);
            }
        }


        free(framebufferInfo);
    }
}

void RenderTextureInit(RenderTexture *render, uint32_t type, uint32_t width, uint32_t height, uint32_t flags)
{
    memset(render, 0, sizeof(RenderTexture));

    switch(type)
    {
        case ENGINE_RENDER_TYPE_DEPTH:
            render->m_format = findDepthFormat();
            break;
        case ENGINE_RENDER_TYPE_CUBEMAP:
            render->m_format = VK_FORMAT_R32_SFLOAT;
            break;
        case ENGINE_RENDER_TYPE_WINDOW:
            render->m_format = swapChainImageFormat;
            break;
        case ENGINE_RENDER_TYPE_IMAGE:
            render->m_format = swapChainImageFormat;
            break;
        default:
            render->m_format = 0;
            break;
    }

    render->type = type;
    render->mip_levels = floor(log2(e_max(width, height))) + 1;

    if(flags & ENGINE_RENDER_FLAG_DEPTH)
        render->m_format = findDepthFormat();

    render->flags = flags;

    render->currFrame = 0;

    if(type != ENGINE_RENDER_TYPE_WINDOW && type != ENGINE_RENDER_TYPE_GEOMETRY)
    {
        render->height = height;
        render->width = width;
    }else{
        render->height = swapChainExtent.height;
        render->width = swapChainExtent.width;
    }

    render->persp_view_distance = 500;
    render->persp_view_near = 0.01;
    render->persp_view_angle = 45;

    render->ortg_view_distance = 100;
    render->ortg_view_size = 0.01;

    render->frust_far = 100;
    render->frust_near = 0.1;
    render->frust_side = 0.1;

    render->cascadeSplit = 0;

    render->clear_color = vec3_f( 0.3, 0.01f, 0.01f);
    render->up_vector = vec3_f( 0, 1, 0);

    if(render->type == ENGINE_RENDER_TYPE_IMAGE)
    {
        render->flags |= ENGINE_RENDER_FLAG_ONE_SHOT;
        render->frames = (RenderFrame*) calloc(1, sizeof(RenderFrame));
        render->num_frames = 1;
    }else{
        render->frames = (RenderFrame*) calloc(imagesCount, sizeof(RenderFrame));
        render->num_frames = imagesCount;
    }

    RenderTextureCreateRenderPass(render, &render->render_pass);

    if(type & ENGINE_RENDER_TYPE_CUBEMAP)
        RenderTextureCreateFrames(render, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
    else
        RenderTextureCreateFrames(render, 0);

    return;
}

void RenderTextureReload(RenderTexture *render, uint32_t indx_surf)
{
    if(render->flags & ENGINE_RENDER_FLAG_ONE_SHOT)
        render->flags &= ~(ENGINE_RENDER_FLAG_SHOOTED);
}

void RenderTextureRecreate(RenderTexture *render)
{
    RenderTextureDestroy(render);

    if(render->type == ENGINE_RENDER_TYPE_WINDOW)
        RenderTextureInit(render, render->type, WIDTH, HEIGHT, render->flags);
    else
        RenderTextureInit(render, render->type, render->width, render->height, render->flags);
}

void RenderTextureBeginRendering(RenderTexture *render, void *cmd_buff)
{
    RenderFrame *frame;

    if(render->flags & ENGINE_RENDER_FLAG_ONE_SHOT)
        frame = &render->frames[0];
    else
        frame = &render->frames[imageIndex];

    VkRenderPassBeginInfo *renderBeginInfo = calloc(1, sizeof(VkRenderPassBeginInfo));
    renderBeginInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBeginInfo->renderPass = render->render_pass;

    if(render->type & ENGINE_RENDER_TYPE_CUBEMAP)
        renderBeginInfo->framebuffer = frame->framebufers[render->currFrame];
    else{
        renderBeginInfo->framebuffer = frame->framebufers[0];
    }
    renderBeginInfo->renderArea.offset.x = 0;
    renderBeginInfo->renderArea.offset.y = 0;
    renderBeginInfo->renderArea.extent.width = render->width;
    renderBeginInfo->renderArea.extent.height = render->height;

    if(render->m_format != 0)
    {
        if(render->type == ENGINE_RENDER_TYPE_DEPTH || (render->flags & ENGINE_RENDER_FLAG_DEPTH))
        {
            VkClearValue clearValue;

            clearValue.depthStencil.depth = 1.0f;
            clearValue.depthStencil.stencil = 0.0;

            renderBeginInfo->clearValueCount = 1;
            renderBeginInfo->pClearValues = &clearValue;
        }else{
            VkClearValue clearValues[2];

            clearValues[0].color.float32[0] = render->clear_color.x;
            clearValues[0].color.float32[1] = render->clear_color.y;
            clearValues[0].color.float32[2] = render->clear_color.z;
            clearValues[0].color.float32[3] = 1.0f;
            clearValues[1].depthStencil.depth = 1.0f;
            clearValues[1].depthStencil.stencil = 0;

            renderBeginInfo->clearValueCount = 2;
            renderBeginInfo->pClearValues = clearValues;
        }
    }

    vkCmdBeginRenderPass(cmd_buff, renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    free(renderBeginInfo);
}

void RenderTextureSetCurrentFrame(RenderTexture *render, uint32_t indx_frame)
{
    render->currFrame = indx_frame;
}

void RenderTextureEndRendering(RenderTexture *render, void *cmd_buff)
{
    vkCmdEndRenderPass(cmd_buff);
}

void RenderTextureDestroy(RenderTexture *render)
{
    for (int j=0;j< render->num_frames;j++) {

        if(render->type != ENGINE_RENDER_TYPE_WINDOW)
        {
            vkDestroyImage(e_device, render->frames[j].image, NULL);
            vkFreeMemory(e_device, render->frames[j].image_memory, NULL);
            vkDestroySampler(e_device, render->frames[j].sampler, NULL);
            vkDestroyImageView(e_device, render->frames[j].view, NULL);

            vkDestroyImage(e_device, render->frames[j].depth_image, NULL);
            vkFreeMemory(e_device, render->frames[j].depth_memory, NULL);
            vkDestroyImageView(e_device, render->frames[j].depth_view, NULL);
        }

        if(render->type & ENGINE_RENDER_TYPE_CUBEMAP)
        {
            for(int i=0;i < 6;i++){
                vkDestroyFramebuffer(e_device, render->frames[j].framebufers[i], NULL);
                vkDestroyImageView(e_device, render->frames[j].shadowCubeMapFaceImageViews[i], NULL);
            }
        }else
            vkDestroyFramebuffer(e_device, render->frames[j].framebufers[0], NULL);
    }

    free(render->frames);

    vkDestroyRenderPass(e_device, render->render_pass, NULL);
}
