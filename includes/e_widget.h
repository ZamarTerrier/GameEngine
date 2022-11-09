#ifndef E_WIDGET_H
#define E_WIDGET_H

#include "stdinclude.h"

#include "gameObject2D.h"

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
    GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT,
    GUI_TRIGGER_ENTRY_KEY_RELEASE_INPUT,
    GUI_TRIGGER_RANGE_CHANGE,
} TriggersEnum;

typedef enum{
    GUI_TYPE_WIDGET,
    GUI_TYPE_TEXT,
    GUI_TYPE_BUTTON,
    GUI_TYPE_LIST,
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

typedef struct ChildStack{
    struct ChildStack* next;
    void *node;
} ChildStack;

typedef struct EWidget{
    GameObject2D go;
    vec2 offset;
    vec4 color;
    vec2 position;
    vec2 scale;
    struct EWidget* parent;
    struct ChildStack* child;
    bool in, was_in, out, was_out, active;
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
} GUIBuffer;

typedef struct{
    ObjectParams objs[256];
    int size;
} MaskObjectBuffer;

 ChildStack * WidgetFindChild(EWidget* widget, int num);
void WidgetSetParent(EWidget* ew, EWidget* parent);
void WidgetInit(EWidget *ew, DrawParam dParam, EWidget *parent);
void WidgetConfirmTrigger(EWidget* widget, int trigger, void *entry);
void WidgetConnect(EWidget *widget, int trigger, void* callback, void *args);
void WidgetRecreate(EWidget *widget);
void WidgetEventsPipe(EWidget *widget);
void WidgetDraw(EWidget *widget);
void WidgetDestroy(EWidget * widget);

#endif // E_WIDGET_H
