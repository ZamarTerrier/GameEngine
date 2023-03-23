#include <e_widget_image.h>

#include <vulkan/vulkan.h>

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

#include "e_math.h"

void ImageWidgetUpdateUniformBufferDefault(EWidgetImage* img) {

    vec2 offset = {0, 0};
    if(img->widget.parent != NULL){
        offset = v2_div(img->widget.parent->offset, (vec2){ WIDTH, HEIGHT});
        img->widget.position = v2_add(v2_add(v2_add(img->widget.go.transform.position, img->widget.parent->position), offset), v2_divs(img->widget.go.transform.scale, 2));
    }
    else{
        img->widget.position = v2_add(img->widget.go.transform.position, v2_divs(img->widget.go.transform.scale, 2));
    }

    PipelineSetting *settings =  img->widget.go.graphObj.gItems.settings;

    vec2 parent_pos = {0, 0};

    WidgetUpdateScissor(&img->widget, &settings[0].scissor, &parent_pos, &offset);

    TransformBuffer2D tbo;

    tbo.position = v2_muls(v2_subs(img->widget.position, 0.5), 2.0);
    tbo.rotation = img->widget.go.transform.rotation;
    tbo.scale = img->widget.go.transform.scale;

    DescriptorUpdate(img->widget.go.graphObj.blueprints.descriptors, 0, &tbo, sizeof(tbo));

    ImageBufferObjects ibo;
    memset(&ibo, 0, sizeof(ImageBufferObjects));

    ibo.scale.x = 1.0f;
    ibo.scale.y = 1.0f;

    DescriptorUpdate(img->widget.go.graphObj.blueprints.descriptors, 1, &ibo, sizeof(ibo));
}

void ImageWidgetCreateQuad(EWidgetImage *wi)
{
    Vertex2D *verts = calloc(4, sizeof(Vertex2D));

    float size = 0.5f;

    verts[0].position.x = -size;
    verts[0].position.y = -size;
    verts[0].texCoord.x = 0;
    verts[0].texCoord.y = 0;

    verts[1].position.x = size;
    verts[1].position.y = -size;
    verts[1].texCoord.x = 1.0f;
    verts[1].texCoord.y = 0;

    verts[2].position.x = size;
    verts[2].position.y = size;
    verts[2].texCoord.x = 1.0f;
    verts[2].texCoord.y = 1.0f;

    verts[3].position.x = -size;
    verts[3].position.y = size;
    verts[3].texCoord.x = 0;
    verts[3].texCoord.y = 1.0f;

    for(int i=0;i < 4;i++)
    {
        verts[i].color = (vec3){ 1, 1, 1};
    }

    uint32_t *tIndx = calloc(6, sizeof(uint32_t));

    uint32_t indx[] = {
      0, 1, 2, 2, 3, 0
    };

    memcpy(tIndx, indx, 6 * sizeof(uint32_t));

    GraphicsObjectSetVertex(&wi->widget.go.graphObj, verts, 4, tIndx, 6);
}


void ImageWidgetInit(EWidgetImage *img, char *image_path, EWidget *parent){

    GameObject2DInit(img);

    GameObjectSetUpdateFunc(&img->widget.go, (void *)ImageWidgetUpdateUniformBufferDefault);

    memcpy(img->widget.go.name, "Widget_Image", 10);
    img->widget.type = ENGINE_WIDGET_TYPE_IMAGE;

    GraphicsObjectSetVertexSize(&img->widget.go.graphObj, sizeof(Vertex2D), sizeof(uint32_t));

    ImageWidgetCreateQuad(img);

    BuffersAddUniformObject(&img->widget.go.graphObj.blueprints, sizeof(TransformBuffer2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&img->widget.go.graphObj.blueprints, sizeof(ImageBufferObjects), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    img->widget.go.image = calloc(1, sizeof(GameObjectImage));

    if(strlen(image_path) != 0)
    {
        memset(img->widget.go.image->path, 0, 256);
        int len = strlen(image_path);
        img->widget.go.image->path = calloc(len + 1, sizeof(char));
        memcpy(img->widget.go.image->path, image_path, len);
        img->widget.go.image->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    TextureImageAdd(&img->widget.go.graphObj.blueprints, img->widget.go.image);

    GraphicsObjectCreateDrawItems(&img->widget.go.graphObj, false);

    PipelineSetting setting;

    PipelineSettingSetDefault(&img->widget.go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_sprite_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_sprite_vert_spv_size);
        setting.fragShader = &_binary_shaders_sprite_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_sprite_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(&img->widget.go, &setting);

    PipelineCreateGraphics(&img->widget.go.graphObj, false);

    img->widget.color = (vec4){0.4, 0.1, 0.1, 1.0};

    img->widget.offset.x = 0;
    img->widget.offset.y = 0;
    img->widget.transparent = 1.0f;

    WidgetSetParent(&img->widget, parent);

    img->widget.widget_flags = ENGINE_FLAG_WIDGET_VISIBLE;

    img->widget.callbacks.stack = (CallbackStruct *) calloc(MAX_GUI_CALLBACKS, sizeof(CallbackStruct));
    img->widget.callbacks.size = 0;

}
