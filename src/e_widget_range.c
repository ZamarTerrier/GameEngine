
#include "e_widget_range.h"

#include "engine.h"

#include "e_math.h"

vec2 range_mouse, range_temp;

void RangeWidgetPress(EWidget* widget, void* entry, void* args){

    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    range_mouse.x = xpos;
    range_mouse.y = ypos;

    range_temp = Transform2DGetPosition(widget);
}

void RangeWidgetMove(EWidget* widget, void* entry, void* args){

    vec2 te, te2;
    double xpos, ypos;

    EWidgetRange *range = (EWidgetRange *)args;

    EngineGetCursorPos(&xpos, &ypos);
    te.x = xpos;
    te.y = ypos;

    te2 = Transform2DGetPosition(widget);
    te = v2_sub(te, range_mouse);
    te.x += range_temp.x;
    te.y = te2.y;

    int len = range->max - range->min;

    vec2 size = v2_muls(Transform2DGetScale(&range->widget), 2);
    size.x -= 20;

    if(te.x  < 0)
        te.x = 0;

    if(te.x > size.x)
        te.x = size.x;

    float diff =  len / size.x;

    float val = te.x * diff + range->min;

    if(range->dest != NULL)
        *range->dest = val;

    Transform2DSetPosition(widget, te.x, te.y);

    WidgetConfirmTrigger(range, GUI_TRIGGER_RANGE_CHANGE, &val);
}

void RangeWidgetInit(EWidgetRange *range, float size_x, float size_y, float min, float max, EWidget *parent){

    DrawParam param = {};
    WidgetInit(&range->widget, NULL, parent);
    memcpy(range->widget.go.name, "Widget_Range", 11);
    WidgetInit(&range->line, NULL, &range->widget);
    WidgetInit(&range->range, NULL, &range->widget);

    range->widget.type = GUI_TYPE_RANGE;
    range->line.color = (vec4){0.7, 0.7, 0.7, 1.0};
    Transform2DSetScale(&range->line, size_x, 2);
    Transform2DSetPosition(&range->line, 0, size_y - 4);
    range->line.widget_flags &= ~(ENGINE_FLAG_WIDGET_ACTIVE);

    range->widget.color = (vec4){0.7, 0.7, 0.7, 0.0};
    range->widget.transparent = 0.0f;
    Transform2DSetScale(&range->widget, size_x, size_y);

    range->min = min;
    range->max = max;
    range->dest = NULL;

    range->range.color = (vec4){0.3, 0, 0, 1.0};
    Transform2DSetScale(&range->range, 10, size_y);
    Transform2DSetPosition(&range->range, 0, 0);

    WidgetConnect(&range->range, GUI_TRIGGER_MOUSE_PRESS, RangeWidgetPress, NULL);
    WidgetConnect(&range->range, GUI_TRIGGER_MOUSE_MOVE, RangeWidgetMove, range);
}

void RangeWidgetSetValueDestin(EWidgetRange *range, float *val_dest)
{
    range->dest = val_dest;
}

void RangeWidgetSetValue(EWidgetRange *range, float val)
{

    vec2 size = v2_muls(Transform2DGetScale(&range->widget), 2);
    size.x -= 20;

    vec2 te = Transform2DGetPosition(&range->range);

    int len = range->max - range->min;
    float diff =  len / size.x;

    te.x = (val - range->min) / diff;
    Transform2DSetPosition(&range->range, te.x, te.y);
}
