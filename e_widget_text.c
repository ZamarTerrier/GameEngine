#include "e_widget_text.h"

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

    settings[0].scissor.offset.x = parentPos.x * WIDTH;
    settings[0].scissor.offset.y = parentPos.y * HEIGHT;
    settings[0].scissor.extent.height = parentSize.y * 2 * HEIGHT;
    settings[0].scissor.extent.width = parentSize.x * 2 * WIDTH;

    ShaderBuffer* sBuffer = wt->widget.go.graphObj.local.descriptors;

    TransformBuffer2D tbo;

    tbo.position = v2_muls(v2_subs(wt->widget.position, 0.5), 2.0);
    tbo.rotation = wt->widget.go.transform.rotation;
    tbo.scale = wt->widget.go.transform.scale;

    vkMapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tbo), 0, &data);
    memcpy(data, &tbo, sizeof(tbo));
    vkUnmapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex]);

}

void TextWidgetDrawDefault(EWidgetText* wt)
{
    for(int i=0; i < wt->widget.go.graphObj.gItems.pipelineCount; i++){
        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, wt->widget.go.graphObj.gItems.graphicsPipeline[i]);
        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, wt->widget.go.graphObj.gItems.pipelineLayout[i], 0, 1, &wt->widget.go.graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

        PipelineSetting *settings = &wt->widget.go.graphObj.gItems.settings[i];

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
            wt->widget.go.graphObj.local.descriptors[i].texture = (Texture2D *) calloc(1, sizeof(Texture2D));
            TextImageMakeTexture(&wt->widget.go, &wt->tData, wt->widget.go.graphObj.local.descriptors[i].texture);
        }
    }

}

void TextWidgetMakeLastText(EWidgetText *wt){
    if(strlen(wt->tData.text) > 0){
        TextWidgetSetText(wt, wt->tData.text);
    }
    else
        TextWidgetSetTextW(wt, wt->tData.wText);
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
    GameObject2DCreateDrawItems(wt);
    PipelineCreateGraphics(&wt->widget.go.graphObj);
    Transform2DReposition(wt);
    TextWidgetMakeLastText(wt);
}

void TextWidgetAddTexture(EWidgetText *wt){

    wt->widget.go.graphObj.local.descrCount ++;

    wt->widget.go.graphObj.local.descriptors = (ShaderBuffer *) realloc(wt->widget.go.graphObj.local.descriptors, wt->widget.go.graphObj.local.descrCount * sizeof(ShaderBuffer));

    wt->widget.go.graphObj.local.descriptors[wt->widget.go.graphObj.local.descrCount - 1].texture = (Texture2D *) calloc(1, sizeof(Texture2D));
    wt->widget.go.graphObj.local.descriptors[wt->widget.go.graphObj.local.descrCount - 1].descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    wt->widget.go.graphObj.local.descriptors[wt->widget.go.graphObj.local.descrCount - 1].size = 1;
    wt->widget.go.graphObj.local.descriptors[wt->widget.go.graphObj.local.descrCount - 1].stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    wt->widget.go.graphObj.local.descriptors[wt->widget.go.graphObj.local.descrCount - 1].image = NULL;

    TextImageMakeTexture(&wt->widget.go, &wt->tData, wt->widget.go.graphObj.local.descriptors[wt->widget.go.graphObj.local.descrCount - 1].texture);
}

void TextWidgetInit(EWidgetText *wt, int fontSize, DrawParam dParam, EWidget* parent){

    GameObject2DInit(wt);

    GraphicsObjectSetVertex(&wt->widget.go.graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObjectSetUpdateFunc(wt, (void *)TextWidgetUpdateUniformBufferDefault);
    GameObjectSetDrawFunc(wt, (void *)TextWidgetDrawDefault);
    GameObjectSetRecreateFunc(wt, (void *)TextWidgettRecreate);

    GraphicsObjectSetShadersPath(&wt->widget.go.graphObj, dParam.vertShader, dParam.fragShader);

    TextDataInit(&wt->tData, fontSize, dParam.filePath);

    BuffersAddUniformObject(&wt->widget.go.graphObj.local, sizeof(Transform2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    TextWidgetAddTexture(wt);

    GameObject2DCreateDrawItems(wt);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&wt->widget.go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_text_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_text_vert_spv_size);
        setting.fragShader = &_binary_shaders_text_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_text_frag_spv_size);
        setting.fromFile = 0;
    }
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    GameObject2DAddSettingPipeline(wt, setting);


    wt->widget.color = (vec4){0.4, 0.1, 0.1, 1.0};

    wt->widget.offset.x = 0;
    wt->widget.offset.y = 0;

    WidgetSetParent(&wt->widget, parent);

    wt->widget.in = wt->widget.was_in = wt->widget.was_out = wt->widget.out = false;

    wt->widget.callbacks.stack = (CallbackStruct *) calloc(0, sizeof(CallbackStruct));

    PipelineCreateGraphics(&wt->widget.go.graphObj);

}

void TextWidgetSetColor(EWidgetText* wt, vec3 color)
{
    TextDataSetTextColor(&wt->tData, color);
}

void TextWidgetSetText(EWidgetText* wt, const char* text)
{
    TextImageSetText(text, &wt->widget.go, &wt->tData);
}

void TextWidgetSetTextW(EWidgetText* wt, const wchar_t* text)
{
    TextImageSetTextW(text, &wt->widget.go, &wt->tData);
}
