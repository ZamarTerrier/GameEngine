#include "e_painter.h"

#include <vulkan/vulkan.h>

#include "graphicsObject.h"

#include "buffers.h"

#include "e_resource_data.h"
#include "e_resource_shapes.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

PaintDrawFunc some_func;

vec4 brush_color = {1, 1, 1, 1};

void PainterObjectUniformUpdate(EPainter *painter)
{
    some_func(painter);

    void* data;

    PainterBuffer pb = {};
    pb.offset.x = 0;
    pb.offset.y = 0;

    vec2 offset = {0, 0};

    pb.position = painter->go.transform.position;
    pb.size = painter->go.transform.scale;

    DescriptorUpdate(painter->go.graphObj.blueprints.descriptors, 0, &pb, sizeof(pb));

    DescriptorUpdate(painter->go.graphObj.blueprints.descriptors, 1, &painter->drawObjects, sizeof(painter->drawObjects));;

    memset(&painter->drawObjects, 0, sizeof(painter->drawObjects));

}

void PainterObjectInit(EPainter *painter)
{
    memset(painter, 0, sizeof(EPainter));
    memcpy(painter->go.name, "Painter", 7);

    GameObject2DInit(painter);
    GameObjectSetUpdateFunc(painter, (void *)PainterObjectUniformUpdate);

    GraphicsObjectSetVertexSize(&painter->go.graphObj, sizeof(Vertex2D), sizeof(uint32_t));
    GraphicsObjectSetVertex(&painter->go.graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObject2DSetLinkedShape(painter);

    BuffersAddUniformObject(&painter->go.graphObj.blueprints, sizeof(PainterBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);
    BuffersAddUniformObject(&painter->go.graphObj.blueprints, sizeof(DrawObjectsBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);

    GraphicsObjectCreateDrawItems(&painter->go.graphObj, false);

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

    GameObject2DAddSettingPipeline(&painter->go, &setting);

    PipelineCreateGraphics(&painter->go.graphObj, false);
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

