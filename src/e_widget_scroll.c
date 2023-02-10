#include "e_widget_scroll.h"

#include "e_math.h"

#include "e_widget_list.h"

vec2 scroll_mouse, scroll_temp;

void ScrollWidgetPress(EWidgetScroll *widget, void* entry, void* args){

    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    scroll_mouse.x = xpos;
    scroll_mouse.y = ypos;

    scroll_temp = Transform2DGetPosition(widget);
}

void ScrollWidgetMove(EWidget* widget, void* entry, EWidgetScroll* scroll){

    vec2 te, te2;
    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    te.x = xpos;
    te.y = ypos;

    te = v2_sub(te, scroll_mouse);
    te = v2_add(scroll_temp, v2_muls(te, 2));

    te2 = Transform2DGetPosition(widget);
    te.x = te2.x;

    vec2 size = v2_muls(Transform2DGetScale(scroll), 2);

    size.y -= scroll->scroll_size * 2;

    if(te.y  < 0)
        te.y = 0;

    if(te.y > size.y)
        te.y = size.y;

    float diff = te.y / size.y;

    Transform2DSetPosition(widget, te.x, te.y);

    WidgetConfirmTrigger(scroll, GUI_TRIGGER_SCROLL_CHANGE, &diff);
}

void ScrollWidgetInit(EWidgetScroll *scroll, uint32_t width, uint32_t height, DrawParam *dParam, EWidget *parent)
{
    WidgetInit(scroll, dParam, parent);
    scroll->widget.color = (vec4){1.0f, 1.0f, 1.0f, 1.0f};
    Transform2DSetScale(scroll, width, height);

    scroll->widget.transparent = 0.0f;

    WidgetInit(&scroll->scroll, dParam, scroll);
    scroll->scroll.color = (vec4){1.0f, 0.0f, 0.0f, 1.0f};


    Transform2DSetScale(&scroll->scroll, 20, 20);
    Transform2DSetPosition(&scroll->scroll, width * 2 - 40, 0);

    WidgetConnect(&scroll->scroll, GUI_TRIGGER_MOUSE_PRESS, ScrollWidgetPress, scroll);
    WidgetConnect(&scroll->scroll, GUI_TRIGGER_MOUSE_MOVE, ScrollWidgetMove, scroll);
}

void ScrollWidgetUpdate(EWidgetScroll *scroll, void *list)
{
    EWidgetList *some_list = list;

    vec2 mySize = Transform2DGetScale(scroll);

    float razn = mySize.y / ((some_list->size_y * some_list->size) - mySize.y);

    float result = mySize.y * razn / 2;

    if(result > mySize.y)
        result = mySize.y;

    scroll->scroll_size = result;

    Transform2DSetScale(&scroll->scroll, 20, result);
}

void ScrollWidgetSetScrollSize(EWidgetScroll *scroll, float percent)
{
    vec2 scale = Transform2DGetScale(scroll);

    Transform2DSetScale(&scroll->scroll, 20, 20);

}
