#include "e_widget_text.h"

#include "vulkan/vulkan.h"

#include "buffers.h"
#include "graphicsItems.h"

#include "e_math.h"
#include "pipeline.h"

void TextWidgetUpdateUniformBufferDefault(EWidgetText* wt) {

    void* data;

    double xpos, ypos;

    vec2 offset = {0, 0};
    if(wt->widget.parent != NULL){
        offset = v2_div(wt->widget.parent->offset, (vec2){ WIDTH, HEIGHT});
        wt->widget.position = v2_add(v2_add(wt->widget.go.transform.position, wt->widget.parent->position), offset);
    }
    else{
        wt->widget.position = wt->widget.go.transform.position;
    }


    PipelineSetting *settings =  wt->widget.go.graphObj.gItems.settings;

    vec2 parentPos = {0, 0};

    vec2 parentSize = {WIDTH, HEIGHT};

    EWidget* parent = wt->widget.parent;

    if(parent != NULL)
    {
        parentPos = parent->position;
        parentSize = parent->scale;

        while(parent->parent != NULL){

            parent = parent->parent;

            offset = v2_div(parent->offset, (vec2){ WIDTH, HEIGHT});

            vec2 temp = v2_add(parent->position, offset);

            if(parentPos.x < temp.x)
            {
                parentSize.x = parentSize.x - (temp.x - parentPos.x) / 2;
                parentPos.x = temp.x;
            }

            if(parentPos.y < temp.y)
            {
                parentSize.y = parentSize.y - (temp.y - parentPos.y) / 2;
                parentPos.y = temp.y;
            }

            if(parentPos.x + (parentSize.x * 2) > parent->position.x + (parent->scale.x * 2))
            {
                parentPos.x = parent->position.x;
                parentSize.x = parent->scale.x;
            }

            if(parentPos.y + (parentSize.y * 2) > parent->position.y + (parent->scale.y * 2))
            {
                parentPos.y = parent->position.y;
                parentSize.y = parent->scale.y;
            }

        }

    }

    if(wt->widget.position.y + (wt->tData.font.fontSize * 2) < parentPos.y)
        wt->widget.widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);
    else
        wt->widget.widget_flags |= ENGINE_FLAG_WIDGET_VISIBLE;

    settings[0].scissor.offset.x = parentPos.x * WIDTH;

    if(settings[0].scissor.offset.x < 0)
        settings[0].scissor.offset.x = 0;

    settings[0].scissor.offset.y = parentPos.y * HEIGHT;

    if(settings[0].scissor.offset.y < 0)
        settings[0].scissor.offset.y = 0;

    settings[0].scissor.extent.height = parentSize.y * 2 * HEIGHT;

    if(settings[0].scissor.extent.height > 2000)
        settings[0].scissor.extent.height = 0;

    settings[0].scissor.extent.width = parentSize.x * 2 * WIDTH;

    if(settings[0].scissor.extent.width > 2000)
        settings[0].scissor.extent.width = 0;


    ShaderBuffer * sBuffer = wt->widget.go.graphObj.local.descriptors;

    TransformBuffer2D tbo;

    tbo.position = v2_muls(v2_subs(wt->widget.position, 0.5), 2.0);
    tbo.rotation = wt->widget.go.transform.rotation;
    tbo.scale = wt->widget.go.transform.scale;

    vkMapMemory(e_device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tbo), 0, &data);
    memcpy(data, &tbo, sizeof(tbo));
    vkUnmapMemory(e_device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex]);

}

void TextWidgetDrawDefault(EWidgetText* wt)
{
    GraphicItems *gItems = &wt->widget.go.graphObj.gItems;
    for(int i=0; i < gItems->pipelineCount; i++){
        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, gItems->graphicsPipeline[i]);
        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, gItems->pipelineLayout[i], 0, 1, &gItems->descriptorSets[imageIndex], 0, NULL);

        PipelineSetting *settings = &gItems->settings[i];

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, &wt->widget.go.graphObj.shape.vParam.vertexBuffer, &offsets);
        for (uint32_t j = 0; j < wt->tData.font.numLetters; j++)
        {
            vkCmdDraw(commandBuffers[imageIndex], 4, 1, j * 4, 0);
        }
    }
}

void TextWidgetRecreateUniform(EWidgetText *wt){
    int count = wt->widget.go.graphObj.local.descrCount;

    for(int i=0;i < count;i++){
        if(wt->widget.go.graphObj.local.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
            wt->widget.go.graphObj.local.descriptors[i].uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
            wt->widget.go.graphObj.local.descriptors[i].uniform->size = wt->widget.go.graphObj.local.descriptors[i].buffsize;
            BuffersCreateUniform(wt->widget.go.graphObj.local.descriptors[i].uniform, i);
        }
        else
        {
            /*ImageDestroyTexture(wt->widget.go.graphObj.local.descriptors[i].texture);
            free(wt->widget.go.graphObj.local.descriptors[i].texture);
            wt->widget.go.graphObj.local.descriptors[i].texture = (Texture2D *) calloc(1, sizeof(Texture2D));
            TextImageMakeTexture(&wt->widget.go, &wt->tData, wt->widget.go.graphObj.local.descriptors[i].texture);*/
        }
    }

}

void TextWidgetMakeLastText(EWidgetText *wt){
    TextImageSetText(wt->tData.text, wt, &wt->tData);
}

void TextWidgettRecreate(EWidgetText* wt){

    PipelineSetting *settings = (PipelineSetting *)wt->widget.go.graphObj.gItems.settings;

    for(int i=0; i < wt->widget.go.graphObj.gItems.settingsCount;i++)
    {
        settings[i].scissor.offset.x = 0;
        settings[i].scissor.offset.y = 0;
        settings[i].scissor.extent.height = HEIGHT;
        settings[i].scissor.extent.width = WIDTH;
        settings[i].viewport.x = 0;
        settings[i].viewport.y = 0;
        settings[i].viewport.height = HEIGHT;
        settings[i].viewport.width = WIDTH;
    }

    TextWidgetRecreateUniform(wt);
    GraphicsObjectCreateDrawItems(&wt->widget.go.graphObj);
    PipelineCreateGraphics(&wt->widget.go.graphObj);
    Transform2DReposition(wt);
    TextWidgetMakeLastText(wt);
}

void TextWidgetAddTexture(EWidgetText *wt){

    ShaderBuffer *descriptor = &wt->widget.go.graphObj.local.descriptors[wt->widget.go.graphObj.local.descrCount];

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->image = NULL;

    TextImageMakeTexture(&wt->widget.go, &wt->tData, descriptor);

    wt->widget.go.graphObj.local.descrCount ++;
}

void TextWidgetInit(EWidgetText *wt, int fontSize, DrawParam *dParam, EWidget* parent){

    memcpy(wt->widget.go.name, "Widget_Text", 10);

    wt->widget.type = GUI_TYPE_TEXT;

    GameObject2DInit(wt);

    GameObjectSetUpdateFunc(wt, (void *)TextWidgetUpdateUniformBufferDefault);
    GameObjectSetDrawFunc(wt, (void *)TextWidgetDrawDefault);
    GameObjectSetRecreateFunc(wt, (void *)TextWidgettRecreate);

    if(dParam != NULL)
    {
        GraphicsObjectSetShadersPath(&wt->widget.go.graphObj, dParam->vertShader, dParam->fragShader);

        TextDataInit(&wt->tData, fontSize, dParam->font);
    }else
    {
        char temp[256];
        memset(temp, 0, 256);
        TextDataInit(&wt->tData, fontSize, temp);
    }

    BuffersAddUniformObject(&wt->widget.go.graphObj.local, sizeof(TransformBuffer2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    TextWidgetAddTexture(wt);

    GraphicsObjectCreateDrawItems(&wt->widget.go.graphObj);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&wt->widget.go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.obj_type = ENGINE_TYPE_TEXT_OBJECT;
        setting.vertShader = &_binary_shaders_text_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_text_vert_spv_size);
        setting.fragShader = &_binary_shaders_text_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_text_frag_spv_size);
        setting.fromFile = 0;
    }
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    GameObject2DAddSettingPipeline(wt, &setting);

    wt->widget.type = GUI_TYPE_TEXT;
    wt->widget.color = (vec4){0.4, 0.1, 0.1, 1.0};

    wt->widget.offset.x = 0;
    wt->widget.offset.y = 0;

    WidgetSetParent(&wt->widget, parent);

    wt->widget.widget_flags = ENGINE_FLAG_WIDGET_ACTIVE | ENGINE_FLAG_WIDGET_VISIBLE;

    wt->widget.callbacks.stack = (CallbackStruct *) calloc(MAX_GUI_CALLBACKS, sizeof(CallbackStruct));
    wt->widget.callbacks.size = 0;

    PipelineCreateGraphics(&wt->widget.go.graphObj);

}

void TextWidgetSetColor(EWidgetText* wt, vec3 color)
{
    TextDataSetTextColor(&wt->tData, color);
}

void TextWidgetSetText(EWidgetText* wt, const char* text)
{
    uint32_t size = strlen(text);
    uint32_t buff[size + 1];

    ToolsStringToUInt32(buff, text);

    TextImageSetText(buff, &wt->widget.go, &wt->tData);

    return;
}

void TextWidgetGetText(EWidgetText* wt, char* dest)
{
    ToolsUInt32ToString(dest, wt->tData.text);
}
