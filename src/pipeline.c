#include "pipeline.h"

#include <vulkan/vulkan.h>

#include "render_texture.h"

#include "e_pipeline_variables.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_descriptors.h"
#include "e_resource_export.h"

void PipelineAcceptStack(void *pipeline, void *pipeline_layout)
{
    PipelineStack *stack;

    if(alloc_pipeline_head->node == NULL){
        alloc_pipeline_head->next = calloc(1, sizeof(ChildStack));
        alloc_pipeline_head->node = calloc(1, sizeof(PipelineStack));

        alloc_pipeline_head->next->before = alloc_pipeline_head;

        stack = alloc_pipeline_head->node;
        stack->GraphicsPipeline = pipeline;
        stack->GraphicsPipelineLayout = pipeline_layout;
    }
    else{

        ChildStack *child = alloc_pipeline_head->next;

        while(child->next != NULL)
        {
            child = child->next;
        }

        child->next = calloc(1, sizeof(ChildStack));
        child->next->before = child;
        child->node = calloc(1, sizeof(PipelineStack));

        stack = child->node;
        stack->GraphicsPipeline = pipeline;
        stack->GraphicsPipelineLayout = pipeline_layout;
    }

}

void PipelineClearAll()
{
    ChildStack *child = alloc_pipeline_head;

    PipelineStack *stack = NULL;

    uint32_t counter = 0;

    while(child->next != NULL)
    {
        stack = child->node;

        PipelineDestroyStack(stack->GraphicsPipeline);

        free(child->node);
        child->node = NULL;

        child = child->next;

        free(child->before);

        counter ++;
    }

    if(child->node != NULL){
        stack = child->node;

        PipelineDestroyStack(stack->GraphicsPipeline);

        free(child->node);
        child->node = NULL;

        counter++;
    }

    free(alloc_pipeline_head);

    if(counter > 0)
        printf("Количество не очищенных пайплайнов : %i\n", counter);
}

void PipelineDestroyStack(void *pipeline)
{
    PipelineStack *stack = NULL;

    ChildStack *child = alloc_pipeline_head;

    while(child->next != NULL)
    {
        stack = child->node;

        if(stack->GraphicsPipeline == pipeline)
            break;

        child = child->next;
    }

    stack = child->node;

    if(stack == NULL){
        perror("Такой области памяти нет!\n");
        return;
    }

    if(child->next != NULL && child->before != NULL)
    {
        ChildStack *next = child->next;
        ChildStack *before = child->before;

        vkDestroyPipeline(e_device, stack->GraphicsPipeline, NULL);
        vkDestroyPipelineLayout(e_device, stack->GraphicsPipelineLayout, NULL);
        free(child->node);
        child->node = NULL;

        free(child);
        next->before = before;
        before->next = next;

    }else if(child->next != NULL){
        vkDestroyPipeline(e_device, stack->GraphicsPipeline, NULL);
        vkDestroyPipelineLayout(e_device, stack->GraphicsPipelineLayout, NULL);
        free(child->node);
        child->node = NULL;

        child->next->before = NULL;
        alloc_pipeline_head = child->next;
        free(child);

    }else if(child->before != NULL){
        vkDestroyPipeline(e_device, stack->GraphicsPipeline, NULL);
        vkDestroyPipelineLayout(e_device, stack->GraphicsPipelineLayout, NULL);
        free(child->node);
        child->node = NULL;

        child->before->next = NULL;

        free(child);

    }
}

void PipelineSettingSetDefault(GraphicsObject* graphObj, void *arg){

    PipelineSetting *setting = arg;

    memset(setting, 0, sizeof(PipelineSetting));

    setting->poligonMode = VK_POLYGON_MODE_FILL;
    setting->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    setting->fromFile = 1;
    setting->scissor.offset.x = 0;
    setting->scissor.offset.y = 0;
    setting->scissor.extent = *(EIExtent2D*)&swapChainExtent;
    setting->viewport.x = 0.0f;
    setting->viewport.y = 0.0f;
    setting->viewport.width = (float) swapChainExtent.width;
    setting->viewport.height = (float) swapChainExtent.height;
    setting->viewport.minDepth = 0.0f;
    setting->viewport.maxDepth = 1.0f;
    setting->flags = ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW | ENGINE_PIPELINE_FLAG_DRAW_INDEXED | ENGINE_PIPELINE_FLAG_BIAS |\
                     ENGINE_PIPELINE_FLAG_ALPHA | ENGINE_PIPELINE_FLAG_FRAGMENT_SHADER | ENGINE_PIPELINE_FLAG_VERTEX_SHADER;
    setting->fromFile = 0;
    setting->cull_mode = VK_CULL_MODE_BACK_BIT;
}

void PipelineSettingSetShader(PipelineSetting *setting, char *shader, size_t size, uint32_t type)
{
    uint32_t num = setting->num_stages;
    setting->stages[num].some_shader = shader;
    setting->stages[num].size_some_shader = size;
    setting->stages[num].type_some_shader = type;
    setting->num_stages ++;
}

void PipelineMakePipeline(GraphicsObject *graphObj, uint32_t indx_pack, uint32_t indx_desc)
{
    BluePrintPack *pack = &graphObj->blueprints.blue_print_packs[indx_pack];
    PipelineSetting *setting = &graphObj->blueprints.blue_print_packs[indx_pack].settings[indx_desc];
    ShaderDescriptor *descriptor = &graphObj->gItems.shader_packs[indx_pack].descriptor;
    PipelineStruct *pipeline = &graphObj->gItems.shader_packs[indx_pack].pipelines[graphObj->gItems.shader_packs[indx_pack].num_pipelines];
    RenderTexture *render = graphObj->blueprints.blue_print_packs[indx_pack].render_point;

    //Шейдеры
    VkPipelineShaderStageCreateInfo shaderStages[6];
    memset(shaderStages, 0, sizeof(VkPipelineShaderStageCreateInfo) * 6);

    uint32_t temp = 0x80;
    uint32_t count_stages = 0;
    while(temp < 0x1000){

        if(setting->flags & temp)
        {
            shaderStages[count_stages].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[count_stages].stage = setting->stages[count_stages].type_some_shader;
            shaderStages[count_stages].pName = "main";

            shader some_shader_code;

            if(setting->fromFile)
                some_shader_code = readFile(setting->stages[count_stages].some_shader);
            else{
                some_shader_code.code = setting->stages[count_stages].some_shader;
                some_shader_code.size = setting->stages[count_stages].size_some_shader;
            }

            shaderStages[count_stages].module = createShaderModule(some_shader_code);

            count_stages ++;

        }

        temp *=2;
    }

    //-----------------
    //Информация для щейдеров

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = setting->topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    //-----------
    //Растеризатор
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = setting->poligonMode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = setting->cull_mode;
    rasterizer.frontFace = setting->flags & ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = setting->flags & ENGINE_PIPELINE_FLAG_BIAS ? VK_TRUE : VK_FALSE;
    //-----------------
    //Колор блендинг
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    if((render->type != ENGINE_RENDER_TYPE_DEPTH && !(render->flags & ENGINE_RENDER_FLAG_DEPTH)) || (setting->flags & ENGINE_PIPELINE_FLAG_ALPHA))
    {
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }else
        colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    if(render->type == ENGINE_RENDER_TYPE_DEPTH || (render->flags & ENGINE_RENDER_FLAG_DEPTH))
    {
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
    }

    //---------------
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = graphObj->gItems.perspective;
    depthStencil.depthWriteEnable = graphObj->gItems.perspective;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional

    //---------------
    //Мультисэмплинг
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //------------
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = graphObj->shapes[setting->vert_indx].countBind;
    vertexInputInfo.vertexAttributeDescriptionCount = graphObj->shapes[setting->vert_indx].countAttr;
    vertexInputInfo.pVertexBindingDescriptions = graphObj->shapes[setting->vert_indx].bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = graphObj->shapes[setting->vert_indx].attr;


    VkPipelineDynamicStateCreateInfo dynamicState;
    memset(&dynamicState, 0, sizeof(VkPipelineDynamicStateCreateInfo));

    VkPipelineViewportStateCreateInfo viewportState;
    memset(&viewportState, 0, sizeof(VkPipelineViewportStateCreateInfo));
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    if(setting->flags & ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW)
    {
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

    }else{

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = render->width;
        viewport.height = render->height;
        viewport.minDepth = 0;
        viewport.maxDepth = 1.0;

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = render->width;
        scissor.extent.height = render->height;

        viewportState.pViewports = &viewport;
        viewportState.pScissors = &scissor;
    }

    VkPushConstantRange *push_ranges = calloc(pack->num_push_constants, sizeof(VkPushConstantRange));

    for(int l=0 ;l < pack->num_push_constants;l++)
    {
        push_ranges[l].offset = pack->push_constants[l].offset;
        push_ranges[l].size = pack->push_constants[l].size;
        push_ranges[l].stageFlags = pack->push_constants[l].stageFlags;
    }


    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Кол-во Дескирпторов для Юниформ баферов
    pipelineLayoutInfo.pSetLayouts = &descriptor->descr_set_layout; // Дескирпторы для Юниформ баферов
    pipelineLayoutInfo.pushConstantRangeCount = pack->num_push_constants; // Optional
    pipelineLayoutInfo.pPushConstantRanges = push_ranges;

    if (vkCreatePipelineLayout(e_device, &pipelineLayoutInfo, NULL, &pipeline->layout) != VK_SUCCESS) {
        printf("failed to create pipeline layout!");
        exit(1);
    }

    free(push_ranges);

    //Сам пайплайн
    VkGraphicsPipelineCreateInfo pipelineInfo = {};

    VkPipelineTessellationStateCreateInfo *tessellationState;
    if(setting->flags & (ENGINE_PIPELINE_FLAG_TESSELLATION_CONTROL_SHADER | ENGINE_PIPELINE_FLAG_TESSELLATION_EVALUATION_SHADER))
    {
        tessellationState = calloc( 1, sizeof(VkPipelineTessellationStateCreateInfo));
        tessellationState->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationState->patchControlPoints = 4;//patchControlPoints;

        pipelineInfo.pTessellationState = tessellationState;
    }

    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = count_stages;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;

    if(setting->flags & ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW)
        pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = pipeline->layout;
    pipelineInfo.renderPass = render->render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.pDepthStencilState = &depthStencil;

    if (vkCreateGraphicsPipelines(e_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipeline->pipeline) != VK_SUCCESS) {
        printf("failed to create graphics pipeline!");
        exit(1);
    }

    //-----------------------

    for(int i=0;i < count_stages;i++)
        vkDestroyShaderModule(e_device, shaderStages[i].module, NULL);

    PipelineAcceptStack(pipeline->pipeline, pipeline->layout);
}

void PipelineCreateGraphics(GraphicsObject* graphObj){

    graphObj->gItems.num_shader_packs = graphObj->blueprints.num_blue_print_packs;

    for(int i=0; i < graphObj->blueprints.num_blue_print_packs; i++){

        ShaderPack *pack = &graphObj->gItems.shader_packs[i];
        BluePrintPack *b_pack = &graphObj->blueprints.blue_print_packs[i];

        for(int j=0; j < b_pack->num_settings; j++){

            PipelineMakePipeline(graphObj, i, j);

            pack->num_pipelines ++;

        }
    }
}

void PipelineCreateRenderPass() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;


    VkSubpassDependency* dependency = calloc(2, sizeof(VkSubpassDependency));
    dependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency[0].dstSubpass = 0;
    dependency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; //VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT; //0;
    dependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    dependency[1].srcSubpass = 0;
    dependency[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependency[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = dependency;

    if (vkCreateRenderPass(e_device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        printf("failed to create render pass!");
        exit(1);
    }

    free(dependency);
    dependency = NULL;    
}

void PipelineDestroy(ShaderPack *pack)
{

    for(int i=0;i < pack->num_pipelines;i++)
        PipelineDestroyStack(pack->pipelines[i].pipeline);

}

