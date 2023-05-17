#include "e_widget_window.h"

#include <vulkan/vulkan.h>

#include "e_math.h"

#include "engine.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_export.h"

vec2 e_var_mouse, e_var_temp, e_var_tscale ;

int WindowWidgetSetSize(EWidgetWindow* window, float x, float y)
{

    Transform2DSetScale(&window->top, x, y);

    vec2 position = {};
    Transform2DSetPosition(&window->close, (x - 10) * 2 , 0);
    Transform2DSetPosition(&window->resize, (x - 20) * 2 , 0);
    Transform2DSetPosition(&window->hide, (x - 30) * 2 , 0);

    y -= 14;
    x -= 4;
    Transform2DSetScale(&window->widget, x, y);

    return 0;
}

int WindowWidgetPress(EWidget* widget, void* entry, void* args)
{

    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    e_var_mouse.x = xpos;
    e_var_mouse.y = ypos;

    e_var_temp = Transform2DGetPosition(widget);
    e_var_tscale = Transform2DGetScale(widget);

    return 0;
}

int WindowWidgetMove(EWidget* widget, void* entry, void* args)
{

    EWidgetWindow *window = (EWidgetWindow *)args;

    vec2 te;
    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    te.x = xpos;
    te.y = ypos;


    if(e_var_mouse.y > e_var_temp.y + 10 && e_var_mouse.y < e_var_temp.y && window->resizeble)
    {
        te = v2_sub(te, e_var_mouse);
        vec2 scale = v2_add(e_var_tscale, te);

        scale.x = scale.x <= 0 ? 100 : scale.x;
        scale.y = scale.y <= 0 ? 20 : scale.y;
        WindowWidgetSetSize(window, scale.x, scale.y);

        window->wasHide = false;
        window->wasResize = false;
    }
    else
    {
        te = v2_muls(v2_sub(te, e_var_mouse), 2.0);
        te = v2_add(e_var_temp, te);
        Transform2DSetPosition(widget, te.x, te.y);
    }

    return 0;
}

int WindowWidgetCloseButton(EWidget* widget, void* entry, void *arg){

    EWidgetWindow *window = (EWidgetWindow *)arg;

    window->show = false;

    WidgetConfirmTrigger(window, ENGINE_WIDGET_TRIGGER_WINDOW_CLOSE, NULL);

    return 0;
}

int WindowWidgetResizeButton(EWidget* widget, void* entry, void *arg){

    EWidgetWindow *window = (EWidgetWindow *)arg;

    if(!window->resizeble)
        return;

    if(!window->wasResize && !window->wasHide)
    {
        window->lastPos = Transform2DGetPosition(&window->top);
        window->lastSize = Transform2DGetScale(&window->top);
    }else if(window->wasResize){
        Transform2DSetPosition(&window->top, window->lastPos.x, window->lastPos.y);
        WindowWidgetSetSize(window, window->lastSize.x, window->lastSize.y);
        window->wasHide = false;
        window->wasResize = false;
        return;
    }

    window->wasResize = true;

    Transform2DSetPosition(&window->top, 0, 0);

    WindowWidgetSetSize(window, WIDTH, HEIGHT);

    return 0;
}

int WindowWidgetHideButton(EWidget* widget, void* entry, void *arg){

    EWidgetWindow *window = (EWidgetWindow *)arg;

    if(!window->wasHide && !window->wasResize)
    {
        window->lastPos = Transform2DGetPosition(&window->top);
        window->lastSize = Transform2DGetScale(&window->top);
    }else if(window->wasHide){
        Transform2DSetPosition(&window->top, window->lastPos.x, window->lastPos.y);
        WindowWidgetSetSize(window, window->lastSize.x, window->lastSize.y);
        window->wasHide = false;
        window->wasResize = false;
        return;
    }

    window->wasHide = true;

    WindowWidgetSetSize(window, 100, 12);
    Transform2DSetPosition(&widget->parent->go, 20, (HEIGHT * 2) - 40);

    return 0;
}

void WindowWidgetAddDefault(EWidgetWindow *window, void *render)
{
    uint32_t nums = window->top.go.graphObj.blueprints.num_blue_print_packs;
    window->top.go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&window->top.go.graphObj.blueprints, nums, sizeof(GUIBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)WidgetGUIBufferUpdate, 0);

    BluePrintAddTextureImage(&window->top.go.graphObj.blueprints, nums, window->top.go.image, VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&window->top.go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_gui_widget_window_vert_spv_start, (size_t)(&_binary_shaders_gui_widget_window_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_gui_widget_window_frag_spv_start, (size_t)(&_binary_shaders_gui_widget_window_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;

    GameObject2DAddSettingPipeline(&window->top.go, nums, &setting);

    window->top.go.graphObj.blueprints.num_blue_print_packs ++;
}

void InitTop(EWidget* widget, DrawParam *dParam, vec2 size, vec2 position){

    GameObject2DInit(&widget->go);

    GraphicsObjectSetVertex(&widget->go.graphObj, projPlaneVert, 4, sizeof(Vertex2D), projPlaneIndx, 6, sizeof(uint32_t));

    if(dParam != NULL)
        GraphicsObjectSetShadersPath(&widget->go.graphObj, dParam->vertShader, dParam->fragShader);

    widget->go.image = calloc(1, sizeof(GameObjectImage));

    if(dParam != NULL)
        if(strlen(dParam->second) != 0)
        {
            int len = strlen(dParam->second);
            widget->go.image->path = calloc(len + 1, sizeof(char));
            memcpy(widget->go.image->path, dParam->second, len);
            widget->go.image->path[len] = '\0';
            //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
        }

    widget->offset.x = 0;
    widget->offset.y = 0;

    widget->parent = NULL;
    widget->child = NULL;

    widget->widget_flags = ENGINE_FLAG_WIDGET_ACTIVE | ENGINE_FLAG_WIDGET_VISIBLE;

    widget->callbacks.stack = (CallbackStruct *) calloc(MAX_GUI_CALLBACKS, sizeof(CallbackStruct));
    widget->callbacks.size = 0;

    Transform2DSetScale(widget, size.x, size.y);
    Transform2DSetPosition(widget, position.x, position.y);

    widget->color = (vec4){1, 1, 1, 1.0};
    widget->transparent = 1.0f;

}

void WindowWidgetInitDraw(EWidgetWindow *window)
{
    GraphicsObjectCreateDrawItems(&window->top.go.graphObj);

    PipelineCreateGraphics(&window->top.go.graphObj);
}

void InitName(EWidget* widget, uint32_t* name, DrawParam *dParam, EWidget *parent)
{
    TextWidgetInit(widget, 9, NULL, parent);
    TextWidgetAddDefault(widget, dParam->render);
    GameObject2DInitDraw(widget);

    TextWidgetSetText(widget, name);

    Transform2DSetPosition(widget, 0, 22);
}

void InitBot(EWidget* widget, DrawParam *dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, NULL, parent);
    WidgetAddDefault(widget, dParam->render);
    GameObject2DInitDraw(widget);

    memcpy(widget->go.name, "Widget_Window", 12);
    widget->type = ENGINE_WIDGET_TYPE_WINDOW;

    vec2 botSize = size;
    botSize.y -= 14;
    botSize.x -= 4;

    Transform2DSetScale(widget, botSize.x, botSize.y);
    Transform2DSetPosition(widget, 4, 24);
}

void InitClose(EWidget* widget, DrawParam *dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, NULL, parent);
    WidgetAddDefault(widget, dParam->render);
    GameObject2DInitDraw(widget);

    widget->color = (vec4){ 1.0f, 0.0f, 0.0f, 1.0f};

    Transform2DSetScale(widget, 10, 10);
    Transform2DSetPosition(widget, (size.x - 10) * 2 , 0);
}

void InitResize(EWidget* widget, DrawParam *dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, NULL, parent);
    WidgetAddDefault(widget, dParam->render);
    GameObject2DInitDraw(widget);

    widget->color = (vec4){ 0.0f, 1.0f, 0.0f, 1.0};

    Transform2DSetScale(widget, 10, 10);
    Transform2DSetPosition(widget, (size.x - 20) * 2 , 0);
}

void InitHide(EWidget* widget, DrawParam *dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, NULL, parent);
    WidgetAddDefault(widget, dParam->render);
    GameObject2DInitDraw(widget);

    widget->color = (vec4){ 0.0f, 0.0f, 1.0f, 1.0f};

    Transform2DSetScale(widget, 10, 10);
    Transform2DSetPosition(widget, (size.x - 30) * 2 , 0);
}

void WindowWidgetInit(EWidgetWindow *ww, char* name, vec2 size, DrawParam *dParam, vec2 position)
{
    InitTop(&ww->top, dParam, size, position);
    InitName(&ww->name, name, dParam, &ww->top);
    InitBot(&ww->widget, dParam, size, &ww->top);

    WindowWidgetAddDefault(ww, dParam->render);
    GameObject2DInitDraw(&ww->top);

    InitClose(&ww->close, dParam, size, &ww->top);
    InitResize(&ww->resize, dParam, size, &ww->top);
    InitHide(&ww->hide, dParam, size, &ww->top);

    WidgetConnect(&ww->top, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, WindowWidgetPress, NULL);
    WidgetConnect(&ww->top, ENGINE_WIDGET_TRIGGER_MOUSE_MOVE, WindowWidgetMove, ww);

    WidgetConnect(&ww->close, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, WindowWidgetCloseButton, ww);
    WidgetConnect(&ww->resize, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, WindowWidgetResizeButton, ww);
    WidgetConnect(&ww->hide, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, WindowWidgetHideButton, ww);

    ww->top.type = ENGINE_WIDGET_TYPE_WINDOW;
    ww->show = true;
    ww->wasHide = false;
    ww->wasResize = false;
    ww->resizeble = true;
}

void WindowWidgetShow(EWidgetWindow *ww){
    ww->show = true;

    WidgetConfirmTrigger(ww, ENGINE_WIDGET_TRIGGER_WINDOW_OPEN, NULL);
}

void WindowWidgetHide(EWidgetWindow *ww){
    ww->show = false;

    WidgetConfirmTrigger(ww, ENGINE_WIDGET_TRIGGER_WINDOW_CLOSE, NULL);
}

void WindowWidgetUpdate(EWidgetWindow *ww){

    if(!ww->show)
        return;

    WidgetEventsPipe(&ww->top);
}

void WindowWidgetDraw(EWidgetWindow *ww){

    if(!ww->show)
        return;

    WidgetDraw(&ww->top);
}

void WindowWidgetDestroy(EWidgetWindow *ww){
    WidgetDestroy(&ww->top);
}
