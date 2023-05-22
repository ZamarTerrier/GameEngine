#include "e_widget_text.h"

#include "vulkan/vulkan.h"

#include "e_buffer.h"
#include "graphicsItems.h"

#include "e_math.h"
#include "pipeline.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void TextWidgetUpdateUniformBufferDefault(EWidgetText* wt, BluePrintDescriptor *descriptor) {

    vec2 offset = {0, 0};
    if(wt->widget.parent != NULL){
        offset = v2_div(wt->widget.parent->offset, vec2_f(WIDTH, HEIGHT));
        wt->widget.position = v2_add(v2_add(wt->widget.go.transform.position, v2_muls(wt->widget.parent->position, 2.0)), offset);
    }
    else{
        wt->widget.position = wt->widget.go.transform.position;
    }

    PipelineSetting *settings = wt->widget.go.graphObj.blueprints.blue_print_packs[0].settings;

    vec2 parent_pos = {0, 0};

    WidgetUpdateScissor(&wt->widget, &settings[0].scissor, &parent_pos, &offset);

    /*if(wt->widget.position.y + (wt->tData.font.fontSize * 2) < parent_pos.y)
        wt->widget.widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);
    else
        wt->widget.widget_flags |= ENGINE_FLAG_WIDGET_VISIBLE;*/

    TransformBuffer2D tbo;

    tbo.position = v2_subs(wt->widget.position, 1.0f);
    tbo.rotation = wt->widget.go.transform.rotation;
    tbo.scale = wt->widget.go.transform.scale;

    DescriptorUpdate(descriptor, &tbo, sizeof(tbo));
}

void TextWidgetDrawDefault(EWidgetText* wt, void *command)
{
    for(int i=0; i < wt->widget.go.graphObj.blueprints.num_blue_print_packs; i++)
    {
        BluePrintPack *pack = &wt->widget.go.graphObj.blueprints.blue_print_packs[i];

        if(pack->render_point == current_render)
        {
            ShaderPack *shader_pack = &wt->widget.go.graphObj.gItems.shader_packs[i];

            for(int j=0; j < shader_pack->num_pipelines; j++){

                vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pack->pipelines[j].pipeline);
                vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_pack->pipelines[j].layout, 0, 1, &shader_pack->descriptor.descr_sets[imageIndex], 0, NULL);

                PipelineSetting *settings = &pack->settings[j];

                if(settings->flags & ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW){
                    vkCmdSetViewport(command, 0, 1, &settings->viewport);
                    vkCmdSetScissor(command, 0, 1, &settings->scissor);
                }

                VkDeviceSize offsets = 0;
                vkCmdBindVertexBuffers(command, 0, 1, &wt->widget.go.graphObj.shapes[settings->vert_indx].vParam.vertexBuffer, &offsets);
                for (uint32_t j = 0; j < wt->tData.font.numLetters; j++)
                {
                    vkCmdDraw(command, 4, 1, j * 4, 0);
                }

            }
        }
    }

}

//Не корректно работает
void TextWidgetRecreateUniform(EWidgetText *wt){
    /*
    int count = wt->widget.go.graphObj.blueprints.count;

    for(int i=0;i < count;i++){
        if(wt->widget.go.graphObj.blueprints.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
            wt->widget.go.graphObj.blueprints.descriptors[i].uniform.size = wt->widget.go.graphObj.blueprints.descriptors[i].buffsize;
            BuffersCreateUniform(&wt->widget.go.graphObj.blueprints.descriptors[i].uniform);
        }
        else
        {
            /*ImageDestroyTexture(wt->widget.go.graphObj.local.descriptors[i].texture);
            free(wt->widget.go.graphObj.local.descriptors[i].texture);
            wt->widget.go.graphObj.local.descriptors[i].texture = (Texture2D *) calloc(1, sizeof(Texture2D));
            TextImageMakeTexture(&wt->widget.go, &wt->tData, wt->widget.go.graphObj.local.descriptors[i].texture);
        }
    }*/

}

void TextWidgetMakeLastText(EWidgetText *wt){
    TextImageSetText(wt->tData.text, wt, &wt->tData);
}

//Не корректно работает
void TextWidgettRecreate(EWidgetText* wt){

    /*
    for(int i=0; i < wt->widget.go.graphObj.blueprints.num_blue_print_packs; i++)
    {
        BluePrintPack *pack = wt->widget.go.graphObj.blueprints.blue_print_packs[i];

        for(int j=0; j < pack->num_settings;j++)
        {
            pack->settings[i].scissor.offset.x = 0;
            pack->settings[i].scissor.offset.y = 0;
            pack->settings[i].scissor.extent.height = HEIGHT;
            pack->settings[i].scissor.extent.width = WIDTH;
            pack->settings[i].viewport.x = 0;
            pack->settings[i].viewport.y = 0;
            pack->settings[i].viewport.height = HEIGHT;
            pack->settings[i].viewport.width = WIDTH;
        }
    }

    TextWidgetRecreateUniform(wt);
    GraphicsObjectCreateDrawItems(&wt->widget.go.graphObj, false);
    PipelineCreateGraphics(&wt->widget.go.graphObj, false);
    Transform2DReposition(wt);
    TextWidgetMakeLastText(wt);*/
}

void TextWidgetAddTexture(EWidgetText *wt, uint32_t indx_pack){

    BluePrintPack *pack = &wt->widget.go.graphObj.blueprints.blue_print_packs[indx_pack];

    if(pack->num_descriptors + 1 > MAX_UNIFORMS)
        return;

    BluePrintDescriptor *descriptor = &pack->descriptors[pack->num_descriptors];

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->image = NULL;
    descriptor->flags = ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE;

    TextImageMakeTexture(&wt->widget.go, &wt->tData, descriptor);

    pack->num_descriptors ++;
}

void TextWidgetInit(EWidgetText *wt, int fontSize, DrawParam *dParam, EWidget* parent){

    memcpy(wt->widget.go.name, "Widget_Text", 10);

    wt->widget.type = ENGINE_WIDGET_TYPE_TEXT;

    GameObject2DInit(wt);

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

    wt->widget.type = ENGINE_WIDGET_TYPE_TEXT;
    wt->widget.color = (vec4){0.4, 0.1, 0.1, 1.0};

    wt->widget.offset.x = 0;
    wt->widget.offset.y = 0;

    WidgetSetParent(&wt->widget, parent);

    wt->widget.widget_flags = ENGINE_FLAG_WIDGET_ACTIVE | ENGINE_FLAG_WIDGET_VISIBLE;

    wt->widget.callbacks.stack = (CallbackStruct *) calloc(MAX_GUI_CALLBACKS, sizeof(CallbackStruct));
    wt->widget.callbacks.size = 0;
}

void TextWidgetAddDefault(EWidgetText *wt, void *render)
{

    uint32_t nums = wt->widget.go.graphObj.blueprints.num_blue_print_packs;
    wt->widget.go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&wt->widget.go.graphObj.blueprints, nums, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT, (void *)TextWidgetUpdateUniformBufferDefault, 0);

    TextWidgetAddTexture(wt, nums);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&wt->widget.go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_text_vert_spv_start, (size_t)(&_binary_shaders_text_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_text_frag_spv_start, (size_t)(&_binary_shaders_text_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.flags |= ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE;
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    GameObject2DAddSettingPipeline(wt, nums, &setting);

    wt->widget.go.graphObj.blueprints.num_blue_print_packs ++;
}

void TextWidgetInitDefault(EWidgetText *wt, int fontSize, DrawParam *dParam, EWidget* parent)
{
    TextWidgetInit(wt, fontSize, dParam, parent);
    TextWidgetAddDefault(wt, dParam->render);
    GameObject2DInitDraw(wt);
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
