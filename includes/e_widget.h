#ifndef E_WIDGET_H
#define E_WIDGET_H

#include "engine_includes.h"

#include "gameObject2D.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    GUI_TRIGGER_MOUSE_PRESS,
    GUI_TRIGGER_MOUSE_RELEASE,
    GUI_TRIGGER_MOUSE_MOVE,
    GUI_TRIGGER_MOUSE_IN,
    GUI_TRIGGER_MOUSE_OUT,
    GUI_TRIGGER_MOUSE_STAY,
    GUI_TRIGGER_WIDGET_FOCUS,
    GUI_TRIGGER_WIDGET_UNFOCUS,
    GUI_TRIGGER_BUTTON_PRESS,
    GUI_TRIGGER_COMBOBOX_PRESS,
    GUI_TRIGGER_COMBOBOX_CHANGE_SELLECTED_ITEM,
    GUI_TRIGGER_LIST_PRESS_ITEM,
    GUI_TRIGGER_ENTRY_CHAR_INPUT,
    GUI_TRIGGER_ENTRY_UPDATE,
    GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT,
    GUI_TRIGGER_ENTRY_KEY_REPEAT_INPUT,
    GUI_TRIGGER_ENTRY_KEY_RELEASE_INPUT,
    GUI_TRIGGER_RANGE_CHANGE,
    GUI_TRIGGER_SCROLL_CHANGE,
    GUI_TRIGGER_ROLLER_MOVE,
} TriggersEnum;

typedef enum{
    GUI_TYPE_WIDGET,
    GUI_TYPE_TEXT,
    GUI_TYPE_BUTTON,
    GUI_TYPE_LIST,
    GUI_TYPE_COMBOBOX,
    GUI_TYPE_ENTRY,
    GUI_TYPE_RANGE,
    GUI_TYPE_WINDOW
} TypeEnum;

typedef struct{
    void* func;
    void *args;
    int trigger;
} CallbackStruct;

typedef struct{
    CallbackStruct* stack;
    int size;
} CallbackStack;

typedef struct EWidget{
    GameObject2D go;
    vec2 offset;
    vec4 color;
    vec2 position;
    vec2 scale;
    float transparent;
    struct EWidget* parent;
    struct ChildStack* child;
    struct ChildStack* first;
    struct ChildStack* last;
    bool in, was_in, out, was_out, active, visible, self_visible;
    TypeEnum type;
    CallbackStack callbacks;
} EWidget;

typedef struct{
    vec2 position;
    vec2 size;
}ObjectParams;

typedef struct{
    vec2 offset;
    vec2 position;
    vec2 size;
    vec4 color __attribute__ ((aligned (16)));
    float transparent __attribute__ ((aligned (16)));
} GUIBuffer;

typedef struct{
    ObjectParams objs[256];
    int size;
} MaskObjectBuffer;

 ChildStack * WidgetFindChild(EWidget* widget, int num);
void WidgetSetParent(EWidget* ew, EWidget* parent);
void WidgetInit(EWidget *ew, DrawParam *dParam, EWidget *parent);
void WidgetConfirmTrigger(EWidget* widget, int trigger, void *entry);
void WidgetConnect(EWidget *widget, int trigger, void* callback, void *args);
void WidgetRecreate(EWidget *widget);
void WidgetEventsPipe(EWidget *widget);
void WidgetDraw(EWidget *widget);
void WidgetDestroy(EWidget * widget);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_H
