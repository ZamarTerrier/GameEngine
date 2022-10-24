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
        te.x = size.x;

    float diff =  len / size.x;

    float val = te.x * diff + range->min;

    Transform2DSetPosition(widget, te);

    WidgetConfirmTrigger(range, GUI_TRIGGER_RANGE_CHANGE, &val);
}

void RangeWidgetInit(EWidgetRange *range, vec2 size, int min, int max, EWidget *parent){

    DrawParam param = {};
    WidgetInit(&range->widget, param, parent);
    WidgetInit(&range->line, param, &range->widget);
    WidgetInit(&range->range, param, &range->widget);

    vec2 sizeLine = {size.x, 2};
    vec2 posLine = {0, size.y - 4};
    range->line.color = (vec4){0.7, 0.7, 0.7, 1.0};
    Transform2DSetScale(&range->line, sizeLine);
    Transform2DSetPosition(&range->line, posLine);
    range->line.active = false;

    range->widget.color = (vec4){0.7, 0.7, 0.7, 0.0};
    Transform2DSetScale(&range->widget, size);

    range->min = min;
    range->max = max;

    vec2 sizeVol = {10, size.y};
    vec2 pos = {0, size.y - (sizeVol.y)};
    range->range.color = (vec4){0.3, 0, 0, 1.0};
    Transform2DSetScale(&range->range, sizeVol);
    Transform2DSetPosition(&range->range, pos);

    WidgetConnect(&range->range, GUI_TRIGGER_MOUSE_PRESS, RangeWidgetPress, NULL);
    WidgetConnect(&range->range, GUI_TRIGGER_MOUSE_MOVE, RangeWidgetMove, range);
}
