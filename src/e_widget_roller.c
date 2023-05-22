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

    roller->widget.color.x = roller->selfColor.x - 0.2f;
    roller->widget.color.y = roller->selfColor.y - 0.2f;
    roller->widget.color.z = roller->selfColor.z - 0.2f;
    roller->widget.color.w = roller->selfColor.w;

    return 0;
}

int RollerWidgetRelease(EWidget* widget, void* entry, void* args){

    EWidgetRoller *roller = widget;

    roller->widget.color = roller->selfColor;
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

void RollerWidgetInit(EWidgetRoller *roller, DrawParam *dParam, EWidget *parent)
{
    WidgetInit(roller, dParam, parent);
    WidgetAddDefault(roller, dParam->render);
    GameObject2DInitDraw(roller);

    memcpy(roller->widget.go.name, "Widget_Roller", 11);
    roller->widget.type = ENGINE_WIDGET_TYPE_ROLLER;

    roller->widget.color = roller->selfColor = vec4_f(0.6, 0, 0, 1.0);
    roller->source = NULL;
    WidgetConnect(roller, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, RollerMousePress, NULL);
    WidgetConnect(roller, ENGINE_WIDGET_TRIGGER_MOUSE_MOVE, RollerMouseMove, NULL);
    WidgetConnect(roller, ENGINE_WIDGET_TRIGGER_MOUSE_RELEASE, RollerWidgetRelease, NULL);
}

void RollerWidgetSetSource(EWidgetRoller *roller, float *source)
{
    roller->source = source;
}
