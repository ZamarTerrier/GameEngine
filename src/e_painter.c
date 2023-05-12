#include "e_painter.h"

#include <vulkan/vulkan.h>

#include "graphicsObject.h"

#include "e_buffer.h"

#include "e_resource_data.h"
#include "e_resource_shapes.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

PaintDrawFunc some_func;

vec4 brush_color = {1, 1, 1, 1};

void PainterObjectPainterBufferUpdate(EPainter *painter, BluePrintDescriptor *descriptor)
{
    some_func(painter);

    void* data;

    PainterBuffer pb = {};
    pb.offset.x = 0;
    pb.offset.y = 0;

    vec2 offset = {0, 0};

    pb.position = painter->go.transform.position;
    pb.size = painter->go.transform.scale;

    DescriptorUpdate(descriptor, &pb, sizeof(pb));
}

void PainterObjectDrawObjectsBufferUpdate(EPainter *painter, BluePrintDescriptor *descriptor)
{
    some_func(painter);

    DescriptorUpdate(descriptor, &painter->drawObjects, sizeof(painter->drawObjects));;

    memset(&painter->drawObjects, 0, sizeof(painter->drawObjects));
}

void PainterObjectInit(EPainter *painter)
{
    memset(painter, 0, sizeof(EPainter));
    memcpy(painter->go.name, "Painter", 7);

    GameObject2DInit(painter);

    GraphicsObjectSetVertex(&painter->go.graphObj, projPlaneVert, 4, sizeof(Vertex2D), projPlaneIndx, 6, sizeof(uint32_t));
}

void PainterObjectAddDefault(EPainter *painter, void *render)
{
    uint32_t nums = painter->go.graphObj.blueprints.num_blue_print_packs;
    painter->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&painter->go.graphObj.blueprints, nums, sizeof(PainterBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)PainterObjectPainterBufferUpdate, 0);
    BluePrintAddUniformObject(&painter->go.graphObj.blueprints, nums, sizeof(DrawObjectsBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)PainterObjectDrawObjectsBufferUpdate, 0);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&painter->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_gui_painter_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_gui_painter_vert_spv_size);
        setting.fragShader = &_binary_shaders_gui_painter_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_gui_painter_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(&painter->go, nums, &setting);

    painter->go.graphObj.blueprints.num_blue_print_packs ++;
}

void PainterObjectInitDefault(EPainter *painter, DrawParam *dParam)
{
    PainterObjectInit(painter);
    PainterObjectAddDefault(painter, dParam->render);
    GameObject2DInitDraw(painter);
}

void PainterObjectSetPaintFunc(PaintDrawFunc paint_func)
{
    some_func = paint_func;
}

void PainterObjectSetColor(float r, float g, float b)
{
    brush_color.x = r;
    brush_color.y = g;
    brush_color.z = b;
}

void PainterObjectMakeRectangle(EPainter *painter, float x, float y, float width, float height)
{
    int size = painter->drawObjects.size;
    painter->drawObjects.objs[size].type = ENGINE_PAINTER_TYPE_BOX;
    painter->drawObjects.objs[size].color = brush_color;
    painter->drawObjects.objs[size].position.x = x / WIDTH;
    painter->drawObjects.objs[size].position.y = y / HEIGHT;
    painter->drawObjects.objs[size].size.x = width / WIDTH;
    painter->drawObjects.objs[size].size.y = height / HEIGHT;
    painter->drawObjects.objs[size].transparent = 1.0;

    painter->drawObjects.size ++;
}

void PainterObjectMakeCircle(EPainter *painter, float x, float y, float radius)
{
    int size = painter->drawObjects.size;
    painter->drawObjects.objs[size].type = ENGINE_PAINTER_TYPE_CIRCLE;
    painter->drawObjects.objs[size].color = brush_color;
    painter->drawObjects.objs[size].position.x = x / WIDTH;
    painter->drawObjects.objs[size].position.y = y / HEIGHT;
    painter->drawObjects.objs[size].radius = radius / 1000;
    painter->drawObjects.objs[size].transparent = 1.0;

    painter->drawObjects.size ++;
}


void PainterObjectMakeOrientedBox(EPainter *painter, float x, float y, float width, float height, float angle)
{
    int size = painter->drawObjects.size;
    painter->drawObjects.objs[size].type = ENGINE_PAINTER_TYPE_ORIENTED_BOX;
    painter->drawObjects.objs[size].color = brush_color;
    painter->drawObjects.objs[size].position.x = x / WIDTH;
    painter->drawObjects.objs[size].position.y = y / HEIGHT;
    painter->drawObjects.objs[size].size.x = width / WIDTH;
    painter->drawObjects.objs[size].size.y = height / HEIGHT;
    painter->drawObjects.objs[size].angle = angle;
    painter->drawObjects.objs[size].transparent = 1.0;

    painter->drawObjects.size ++;
}


void PainterObjectMakeSegment(EPainter *painter, float x, float y, float x2, float y2, float radius)
{
    int size = painter->drawObjects.size;
    painter->drawObjects.objs[size].type = ENGINE_PAINTER_TYPE_SEGMENT;
    painter->drawObjects.objs[size].color = brush_color;
    painter->drawObjects.objs[size].position.x = x / WIDTH;
    painter->drawObjects.objs[size].position.y = y / HEIGHT;
    painter->drawObjects.objs[size].size.x = x2 / WIDTH;
    painter->drawObjects.objs[size].size.y = y2 / HEIGHT;
    painter->drawObjects.objs[size].radius = radius / 1000;

    painter->drawObjects.objs[size].transparent = 1.0;

    painter->drawObjects.size ++;
}

