#ifndef E_WIDGET_H
#define E_WIDGET_H

#include "Variabels/engine_includes.h"

#include "Objects/gameObject2D.h"

#define MAX_WIDGET_MASKS 64
#define MAX_WIDGET_DRAW 256

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    ENGINE_WIDGET_TRIGGER_MOUSE_PRESS,
    ENGINE_WIDGET_TRIGGER_MOUSE_RELEASE,
    ENGINE_WIDGET_TRIGGER_MOUSE_MOVE,
    ENGINE_WIDGET_TRIGGER_MOUSE_IN,
    ENGINE_WIDGET_TRIGGER_MOUSE_OUT,
    ENGINE_WIDGET_TRIGGER_MOUSE_STAY,
    ENGINE_WIDGET_TRIGGER_WIDGET_FOCUS,
    ENGINE_WIDGET_TRIGGER_WIDGET_UNFOCUS,
    ENGINE_WIDGET_TRIGGER_BUTTON_PRESS,
    ENGINE_WIDGET_TRIGGER_COMBOBOX_PRESS,
    ENGINE_WIDGET_TRIGGER_COMBOBOX_CHANGE_SELLECTED_ITEM,
    ENGINE_WIDGET_TRIGGER_LIST_PRESS_ITEM,
    ENGINE_WIDGET_TRIGGER_MENU_PRESS_ITEM,
    ENGINE_WIDGET_TRIGGER_ENTRY_CHAR_INPUT,
    ENGINE_WIDGET_TRIGGER_ENTRY_UPDATE,
    ENGINE_WIDGET_TRIGGER_ENTRY_KEY_PRESS_INPUT,
    ENGINE_WIDGET_TRIGGER_ENTRY_KEY_REPEAT_INPUT,
    ENGINE_WIDGET_TRIGGER_ENTRY_KEY_RELEASE_INPUT,
    ENGINE_WIDGET_TRIGGER_WINDOW_OPEN,
    ENGINE_WIDGET_TRIGGER_WINDOW_CLOSE,
    ENGINE_WIDGET_TRIGGER_RANGE_CHANGE,
    ENGINE_WIDGET_TRIGGER_SCROLL_CHANGE,
    ENGINE_WIDGET_TRIGGER_ROLLER_MOVE,
} EngineWidgetTriggersEnum;

typedef enum{
    ENGINE_WIDGET_TYPE_WIDGET,
    ENGINE_WIDGET_TYPE_TEXT,
    ENGINE_WIDGET_TYPE_BUTTON,
    ENGINE_WIDGET_TYPE_LIST,
    ENGINE_WIDGET_TYPE_ROLLER,
    ENGINE_WIDGET_TYPE_SCROLL,
    ENGINE_WIDGET_TYPE_MENU,
    ENGINE_WIDGET_TYPE_COMBOBOX,
    ENGINE_WIDGET_TYPE_ENTRY,
    ENGINE_WIDGET_TYPE_ENTRY_AREA,
    ENGINE_WIDGET_TYPE_RANGE,
    ENGINE_WIDGET_TYPE_IMAGE,
    ENGINE_WIDGET_TYPE_WINDOW
} EngineWidgetTypeEnum;

typedef enum{
    ENGINE_FLAG_WIDGET_IN = 0x1,
    ENGINE_FLAG_WIDGET_WAS_IN = 0x2,
    ENGINE_FLAG_WIDGET_OUT = 0x4,
    ENGINE_FLAG_WIDGET_WAS_OUT = 0x8,
    ENGINE_FLAG_WIDGET_ACTIVE = 0x10,
    ENGINE_FLAG_WIDGET_VISIBLE = 0x20,
    ENGINE_FLAG_WIDGET_SELF_VISIBLE = 0x40,
    ENGINE_FLAG_WIDGET_ALLOCATED = 0x80,
} EngineWidgetFlag;

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
    uint32_t widget_flags, type;
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
    ObjectParams objs[MAX_WIDGET_MASKS];
    int size;
} MaskObjectBuffer;


typedef int(*widget_callback)(EWidget *widget, void *, void*);

void WidgetGUIBufferUpdate(EWidget *ew, BluePrintDescriptor *descriptor);

int WidgetFindIdChild(EWidget* widget);
ChildStack * WidgetFindChild(EWidget* widget, int num);
void WidgetUpdateScissor(EWidget *widget, EIRect2D *scissor, vec2 *parent_pos, vec2 *offset);
void WidgetSetParent(EWidget* ew, EWidget* parent);
void WidgetInit(EWidget *ew, DrawParam *dParam, EWidget *parent);
void WidgetAddDefault(EWidget *widget, void *render);
void WidgetConfirmTrigger(EWidget* widget, int trigger, void *entry);
void WidgetConnect(EWidget *widget, int trigger, widget_callback callback, void *args);
void WidgetRecreate(EWidget *widget);
void WidgetEventsPipe(EWidget *widget);
void WidgetDraw(EWidget *widget);
void WidgetDestroy(EWidget * widget);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_H
