#include "e_widget_roller.h"

#include "engine.h"

#include "e_math.h"

vec2 roller_mouse;

int RollerMousePress(EWidget *widget, void *entry, void *args)
{
    EWidgetRoller *roller = widget;

    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    roller_mouse.x = xpos;
    roller_mouse.y = ypos;

    roller->stable_val = *roller->source;

    return 0;
}

int RollerMouseMove(EWidget *widget, void *entry, void *args)
{
    EWidgetRoller *roller = widget;

    double xpos, ypos;
    vec2 te;

    EngineGetCursorPos(&xpos, &ypos);
    te.x = xpos;
    te.y = ypos;

    te = v2_sub(te, roller_mouse);
    te = v2_divs(te, 2);

    roller->move_val = roller->stable_val + te.y;

    WidgetConfirmTrigger(roller, ENGINE_WIDGET_TRIGGER_ROLLER_MOVE, &roller->move_val);

    return 0;
}

void RollerWidgetInit(EWidgetRoller *roller, EWidget *parent)
{
    WidgetInit(roller, NULL, parent);

    memcpy(roller->widget.go.name, "Widget_Roller", 11);
    roller->widget.type = ENGINE_WIDGET_TYPE_ROLLER;

    roller->widget.color = (vec4){ 0.6, 0, 0, 1.0};
    roller->source = NULL;
    WidgetConnect(roller, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, RollerMousePress, NULL);
    WidgetConnect(roller, ENGINE_WIDGET_TRIGGER_MOUSE_MOVE, RollerMouseMove, NULL);
}

void RollerWidgetSetSource(EWidgetRoller *roller, float *source)
{
    roller->source = source;
}
