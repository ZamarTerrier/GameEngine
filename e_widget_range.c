#include "e_widget_range.h"

#include "e_math.h"

vec2 range_mouse,  range_temp;

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
        te.x = range->max;

    float diff =  len / size.x;

    float val = te.x * diff + range->min;

    Transform2DSetPosition(widget, te);

    WidgetConfirmTrigger(range, GUI_TRIGGER_RANGE_CHANGE, &val);
}

void RangeWidgetInit(EWidgetRange *range, int min, int max, EWidget *parent){

    DrawParam param = {};
    WidgetInit(&range->widget, param, parent);
    WidgetInit(&range->range, param, &range->widget);

    vec2 wSize = {50, 20};
    range->widget.color = (vec3){0.7, 0.7, 0.7};
    Transform2DSetScale(&range->widget, wSize);

    range->min = min;
    range->max = max;

    vec2 size = {10, 20};
    vec2 pos = {0, wSize.y - (size.y)};
    range->range.color = (vec3){0.3, 0, 0};
    Transform2DSetScale(&range->range, size);
    Transform2DSetPosition(&range->range, pos);

    WidgetConnect(&range->range, GUI_TRIGGER_MOUSE_PRESS, RangeWidgetPress, NULL);
    WidgetConnect(&range->range, GUI_TRIGGER_MOUSE_MOVE, RangeWidgetMove, range);
}
