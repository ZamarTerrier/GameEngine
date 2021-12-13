#include "textObject.h"

#include "transform.h"

#include "bindDesciption.h"
#include "texture.h"
#include "buffers.h"

#include "camera.h"

#include "resource.h"

#define TEXTOVERLAY_MAX_CHAR_COUNT 2048


void initTextObject(TextObject* to)
{

    initTransform(&to->transform);

    to->graphObj.local.uniformCount = 3;
    to->graphObj.local.uniformSizes = (VkDeviceSize *) calloc(to->graphObj.local.uniformCount, sizeof(VkDeviceSize));
    to->graphObj.local.uniformSizes[0] = sizeof(ViewUniformObject);
    to->graphObj.local.uniformSizes[1] = sizeof(UniformBufferObject);
    to->graphObj.local.uniformSizes[2] = sizeof(ImgUniformParam);

    to->font.fontWidth = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
    to->font.fontHeight = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;

    unsigned char font24pixels[to->font.fontWidth][to->font.fontHeight];

    stb_font_consolas_24_latin1(to->font.stbFontData, font24pixels, to->font.fontHeight);

    VkDeviceSize bufferSize = TEXTOVERLAY_MAX_CHAR_COUNT *  sizeof(Vertex);

    createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &to->graphObj.shape.vertex.vertexBuffer, &to->graphObj.shape.vertex.vertexBufferMemory);

    to->graphObj.aShader.bindingDescription = getBindingDescription();

    to->graphObj.local.textures = NULL;
    to->graphObj.local.textures = (Texture2D *) calloc(1, sizeof(Texture2D));

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, &font24pixels[0][0], to->font.fontWidth * to->font.fontHeight);
    vkUnmapMemory(device, stagingBufferMemory);

    createImage(to->font.fontWidth, to->font.fontHeight, VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &to->graphObj.local.textures[0].textureImage, &to->graphObj.local.textures[0].textureImageMemory);

    transitionImageLayout(to->graphObj.local.textures[0].textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer, to->graphObj.local.textures[0].textureImage, to->font.fontWidth, to->font.fontHeight);
    transitionImageLayout(to->graphObj.local.textures[0].textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);


    to->graphObj.local.textures[0].textureImageView = createImageView(to->graphObj.local.textures[0].textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    createTextureSampler(&to->graphObj.local.textures[0]);

    to->graphObj.local.texturesCount = 1;

    createUniformBuffers(&to->graphObj.local);

    uint32_t unionSize = to->graphObj.local.texturesCount + to->graphObj.local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(unionSize,sizeof(VkDescriptorType)) ;

    for(i=0;i < to->graphObj.local.uniformCount;i++)
    {
        types[i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    for(i=0;i < to->graphObj.local.texturesCount;i++)
    {
        types[i + to->graphObj.local.uniformCount] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }


    createDescriptorSetLayout(&to->graphObj.gItems, types, unionSize);
    createDescriptorPool(&to->graphObj.gItems, types, unionSize);
    createDescriptorSets(&to->graphObj.gItems, &to->graphObj.local);
    preparePipeline(to);

}

void preparePipeline(TextObject* to)
{
    shader vertShaderCode = readFile(to->graphObj.aShader.vertShader);
    shader fragShaderCode = readFile(to->graphObj.aShader.fragShader);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // Enable blending, using alpha from red channel of the font texture (see text.frag)

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    //Растеризатор
    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.lineWidth = 1.0f;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    //---------------
    //Колор блендинг
    VkPipelineColorBlendAttachmentState blendAttachmentState = {};
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &blendAttachmentState;
    //---------------
    //Тест глубины
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.minDepthBounds = 0.0f; // Optional
    depthStencilState.maxDepthBounds = 1.0f; // Optional
    depthStencilState.stencilTestEnable = VK_FALSE;
    //--------------
    //Вьюпорт
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    //-----------

    //Мультисэмплинг
    VkPipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //------------

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = 1;
    vertexInputState.pVertexBindingDescriptions = &to->graphObj.aShader.bindingDescription;
    vertexInputState.vertexAttributeDescriptionCount = 3;
    vertexInputState.pVertexAttributeDescriptions = attributeDescription;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &to->graphObj.gItems.descriptorSetLayout; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &to->graphObj.gItems.pipelineLayout) != VK_SUCCESS) {
        printf("failed to create pipeline layout!");
        exit(1);
    }

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.layout = to->graphObj.gItems.pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;

    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &to->graphObj.gItems.graphicsPipeline);


    vkDestroyShaderModule(device, fragShaderModule, NULL);
    vkDestroyShaderModule(device, vertShaderModule, NULL);
}

void updateTextUniformBuffer(TextObject* to) {

    Camera* cam = (Camera*) camObj;

    ViewUniformObject vuo = {};
    vuo.pos = cam->pos;
    vuo.scale = cam->scale;

    void* data;

    vkMapMemory(device, to->graphObj.local.uniformBuffersMemory[0][imageIndex], 0, sizeof(vuo), 0, &data);
    memcpy(data, &vuo, sizeof(vuo));
    vkUnmapMemory(device, to->graphObj.local.uniformBuffersMemory[0][imageIndex]);

    UniformBufferObject ubo = {};
    ubo.pos = to->transform.pos;
    ubo.scale = to->transform.scale;

    vkMapMemory(device, to->graphObj.local.uniformBuffersMemory[1][imageIndex], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, to->graphObj.local.uniformBuffersMemory[1][imageIndex]);

    ImgUniformParam iup = {};
    iup.imgOffset = to->transform.img.offset;
    iup.imgScale = to->transform.img.scale;

    vkMapMemory(device, to->graphObj.local.uniformBuffersMemory[2][imageIndex], 0, sizeof(iup), 0, &data);
    memcpy(data, &iup, sizeof(iup));
    vkUnmapMemory(device, to->graphObj.local.uniformBuffersMemory[2][imageIndex]);

}

void drawTextObject(TextObject* to)
{
    vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, to->graphObj.gItems.graphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, to->graphObj.gItems.pipelineLayout, 0, 1, &to->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

    VkDeviceSize offsets = 0;
    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, &to->graphObj.shape.vertex.vertexBuffer, &offsets);
    for (uint32_t j = 0; j < to->font.numLetters; j++)
    {
        vkCmdDraw(commandBuffers[imageIndex], 4, 1, j * 4, 0);
    }
}

void addText(const char* text, float x, float y, TextObject* to)
{

    Vertex* mapped = NULL;

    vkMapMemory(device, to->graphObj.shape.vertex.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);
    to->font.numLetters = 0;

    const uint32_t firstChar = STB_FONT_consolas_24_latin1_FIRST_CHAR;

    const float charW = 1.5f / WIDTH;
    const float charH = 1.5f / HEIGHT;

    float fbW = (float)WIDTH;
    float fbH = (float)HEIGHT;
    x = (x / fbW * 2.0f) - 1.0f;
    y = (y / fbH * 2.0f) - 1.0f;

    // Calculate text width
    float textWidth = 0;
    int len = strlen(text);
    for (i=0;i < len;i++)
    {
        stb_fontchar *charData = &to->font.stbFontData[(uint32_t)text[i] - firstChar];
        textWidth += charData->advance * charW;
    }

    // Generate a uv mapped quad per char in the new text
    for (i=0;i < len;i++)
    {
        stb_fontchar *charData = &to->font.stbFontData[(uint32_t)text[i] - firstChar];

        mapped->pos.x = (x + (float)charData->x0 * charW);
        mapped->pos.y = (y + (float)charData->y0 * charH);
        mapped->texCoord.x = charData->s0;
        mapped->texCoord.y = charData->t0;
        mapped++;

        mapped->pos.x = (x + (float)charData->x1 * charW);
        mapped->pos.y = (y + (float)charData->y0 * charH);
        mapped->texCoord.x = charData->s1;
        mapped->texCoord.y = charData->t0;
        mapped++;

        mapped->pos.x = (x + (float)charData->x0 * charW);
        mapped->pos.y = (y + (float)charData->y1 * charH);
        mapped->texCoord.x = charData->s0;
        mapped->texCoord.y = charData->t1;
        mapped++;

        mapped->pos.x = (x + (float)charData->x1 * charW);
        mapped->pos.y = (y + (float)charData->y1 * charH);
        mapped->texCoord.x = charData->s1;
        mapped->texCoord.y = charData->t1;
        mapped++;

        x += charData->advance * charW;

        to->font.numLetters++;
    }

    vkUnmapMemory(device, to->graphObj.shape.vertex.vertexBufferMemory);
    mapped = NULL;
}

void destroyTextObject(TextObject* to){
    destroyGraphicsObject(&to->graphObj);
}

