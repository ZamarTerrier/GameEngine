#include <e_widget_image.h>

#include <vulkan/vulkan.h>

#include "e_resource.h"

#include "e_math.h"

void ImageWidgetUpdateUniformBufferDefault(EWidgetImage* img) {

    void* data;

    double xpos, ypos;

    vec2 offset = {0, 0};
    if(img->widget.parent != NULL){
        offset = v2_div(img->widget.parent->offset, (vec2){ WIDTH, HEIGHT});
        img->widget.position = v2_add(v2_add(v2_add(img->widget.go.transform.position, img->widget.parent->position), offset), v2_divs(img->widget.go.transform.scale, 2));
    }
    else{
        img->widget.position = v2_add(img->widget.go.transform.position, v2_divs(img->widget.go.transform.scale, 2));
    }


    PipelineSetting *settings =  img->widget.go.graphObj.gItems.settings;

    vec2 parentPos = {0, 0};

    vec2 parentSize = {WIDTH, HEIGHT};

    EWidget* parent = img->widget.parent;

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

    ShaderBuffer* sBuffer = img->widget.go.graphObj.local.descriptors;

    TransformBuffer2D tbo;

    tbo.position = v2_muls(v2_subs(img->widget.position, 0.5), 2.0);
    tbo.rotation = img->widget.go.transform.rotation;
    tbo.scale = img->widget.go.transform.scale;

    vkMapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tbo), 0, &data);
    memcpy(data, &tbo, sizeof(tbo));
    vkUnmapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex]);

}


void ImageWidgetInit(EWidgetImage *img, DrawParam dParam, EWidget *parent){

    GameObject2DInit(img);

    GraphicsObjectSetVertex(&img->widget.go.graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObjectSetUpdateFunc(img, (void *)ImageWidgetUpdateUniformBufferDefault);

    GraphicsObjectSetShadersPath(&img->widget.go.graphObj, dParam.vertShader, dParam.fragShader);

    BuffersAddUniformObject(&img->widget.go.graphObj.local, sizeof(Transform2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    img->widget.go.image = calloc(1, sizeof(GameObjectImage));

    if(strlen(dParam.filePath) != 0)
    {
        int len = strlen(dParam.filePath);
        img->widget.go.image->path = calloc(len, sizeof(char));
        memcpy(img->widget.go.image->path, dParam.filePath, len);
        img->widget.go.image->path[len] = '\0';
        //img->widget.go.image->buffer = ToolsLoadImageFromFile(&img->widget.go.image->size, dParam.filePath);
        ImageAddTexture(&img->widget.go.graphObj.local, img->widget.go.image);
    }


    GameObject2DCreateDrawItems(img);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&img->widget.go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_sprite_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_sprite_vert_spv_size);
        setting.fragShader = &_binary_shaders_sprite_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_sprite_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(img, &setting);

    img->widget.color = (vec4){0.4, 0.1, 0.1, 1.0};

    img->widget.offset.x = 0;
    img->widget.offset.y = 0;

    WidgetSetParent(&img->widget, parent);

    img->widget.in = img->widget.was_in = img->widget.was_out = img->widget.out = false;

    img->widget.callbacks.stack = (CallbackStruct *) calloc(0, sizeof(CallbackStruct));

    PipelineCreateGraphics(&img->widget.go.graphObj);
}
