#include "e_widget_window.h"

#include "e_math.h"

#include "engine.h"

vec2 mouse,  temp, tscale ;

void WindowWidgetSetSize(EWidgetWindow* window, vec2 size)
{

    Transform2DSetScale(&window->top, size);

    vec2 position = {(size.x - 10) * 2 , 0};
    Transform2DSetPosition(&window->close, position);
    position = (vec2){(size.x - 20) * 2 , 0};
    Transform2DSetPosition(&window->resize, position);
    position = (vec2){(size.x - 30) * 2 , 0};
    Transform2DSetPosition(&window->hide, position);

    size.y -= 12;
    size.x -= 2;
    Transform2DSetScale(&window->widget, size);
}

void WindowWidgetPress(EWidget* widget, void* entry, void* args)
{

    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    mouse.x = xpos;
    mouse.y = ypos;

    temp = Transform2DGetPosition(widget);
    tscale = Transform2DGetScale(widget);
}

void WindowWidgetMove(EWidget* widget, void* entry, void* args)
{

    EWidgetWindow *window = (EWidgetWindow *)args;

    vec2 te;
    double xpos, ypos;

    EngineGetCursorPos(&xpos, &ypos);
    te.x = xpos;
    te.y = ypos;


    if(mouse.y < temp.y + 20)
    {
        te = v2_sub(te, mouse);
        te = v2_add(temp, te);
        Transform2DSetPosition(widget, te);
    }
    else
    {
        te = v2_sub(te, mouse);
        vec2 scale = v2_add(tscale, v2_divs(te, 2));

        WindowWidgetSetSize(window, scale);

        window->wasHide = false;
        window->wasResize = false;
    }

}

void WindowWidgetCloseButton(EWidget* widget, void* entry, void *arg){

    EWidgetWindow *window = (EWidgetWindow *)arg;

    window->show = false;
}

void WindowWidgetResizeButton(EWidget* widget, void* entry, void *arg){

    EWidgetWindow *window = (EWidgetWindow *)arg;

    if(!window->wasResize && !window->wasHide)
    {
        window->lastPos = Transform2DGetPosition(&window->top);
        window->lastSize = Transform2DGetScale(&window->top);
    }else if(window->wasResize){
        Transform2DSetPosition(&window->top, window->lastPos);
        WindowWidgetSetSize(window, window->lastSize);
        window->wasHide = false;
        window->wasResize = false;
        return;
    }

    window->wasResize = true;

    vec2 pos = {0,0};
    vec2 size = {WIDTH / 2, HEIGHT / 2};

    Transform2DSetPosition(&window->top, pos);

    WindowWidgetSetSize(window, size);

}

void WindowWidgetHideButton(EWidget* widget, void* entry, void *arg){

    EWidgetWindow *window = (EWidgetWindow *)arg;

    if(!window->wasHide && !window->wasResize)
    {
        window->lastPos = Transform2DGetPosition(&window->top);
        window->lastSize = Transform2DGetScale(&window->top);
    }else if(window->wasHide){
        Transform2DSetPosition(&window->top, window->lastPos);
        WindowWidgetSetSize(window, window->lastSize);
        window->wasHide = false;
        window->wasResize = false;
        return;
    }

    window->wasHide = true;

    vec2 pos = {0,0};
    vec2 size = {50, 12};

    WindowWidgetSetSize(window, size);
    Transform2DSetPosition(&widget->parent->go, pos);

}

void WindowWidgetUniformUpdate(EWidget *ew){

    void* data;

    GUIBuffer gb = {};
    gb.offset.x = ew->offset.x > 0 ? ew->offset.x / (WIDTH) : 0;
    gb.offset.y = ew->offset.y > 0 ? ew->offset.y / (HEIGHT) : 0;

    ew->position = ew->go.transform.position;

    ew->scale = ew->go.transform.scale;

    gb.position = ew->position;
    gb.size = ew->scale;
    gb.color = ew->color;

    vkMapMemory(device, ew->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(gb), 0, &data);
    memcpy(data, &gb, sizeof(gb));
    vkUnmapMemory(device,  ew->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

}

void InitTop(EWidget* widget, DrawParam dParam, vec2 size, vec2 position){

    GameObject2DInit(&widget->go);

    GraphicsObject2DSetVertex(&widget->go.graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObjectSetUpdateFunc(&widget->go, (void *)WindowWidgetUniformUpdate);

    GraphicsObjectSetShadersPath(&widget->go.graphObj, dParam.vertShader, dParam.fragShader);

    BuffersAddUniformObject(&widget->go.graphObj.local, sizeof(GUIBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    GameObject2DCreateDrawItems(&widget->go);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&widget->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_gui_widget_window_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_gui_widget_window_vert_spv_size);
        setting.fragShader = &_binary_shaders_gui_widget_window_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_gui_widget_window_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(&widget->go, setting);

    widget->color = (vec4){0.4, 0.1, 0.1, 1.0};

    widget->offset.x = 0;
    widget->offset.y = 0;

    widget->parent = NULL;
    widget->child = NULL;

    widget->in = widget->was_in = widget->was_out = widget->out = false;

    widget->callbacks.stack = (CallbackStruct *) calloc(0, sizeof(CallbackStruct));

    PipelineCreateGraphics(&widget->go.graphObj);

    Transform2DSetScale(widget, size);
    Transform2DSetPosition(widget, position);

    widget->color = (vec4){1, 1, 1, 1.0};

    widget->active = true;
}

void InitName(EWidget* widget, char* name, DrawParam dParam, EWidget *parent)
{
    vec2 position = {0, 16};
    TextWidgetInit(widget, 9, dParam, parent);

    TextWidgetSetText(widget, name);

    Transform2DSetPosition(widget, position);
}

void InitBot(EWidget* widget, DrawParam dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, dParam, parent);

    vec2 botSize = size;
    botSize.y -= 12;
    botSize.x -= 2;
    vec2 position = {2, 22};

    Transform2DSetScale(widget, botSize);
    Transform2DSetPosition(widget, position);
}

void InitClose(EWidget* widget, DrawParam dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, dParam, parent);

    vec2 tempSize = {10, 10};
    vec2 position = {(size.x - 10) * 2 , 0};

    widget->color = (vec4){ 1.0f, 0.0f, 0.0f, 1.0f};

    Transform2DSetScale(widget, tempSize);
    Transform2DSetPosition(widget, position);
}

void InitResize(EWidget* widget, DrawParam dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, dParam, parent);

    vec2 tempSize = {10, 10};
    vec2 position = {(size.x - 20) * 2 , 0};

    widget->color = (vec4){ 0.0f, 1.0f, 0.0f, 1.0};

    Transform2DSetScale(widget, tempSize);
    Transform2DSetPosition(widget, position);
}

void InitHide(EWidget* widget, DrawParam dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, dParam, parent);

    vec2 tempSize = {10, 10};
    vec2 position = {(size.x - 30) * 2 , 0};

    widget->color = (vec4){ 0.0f, 0.0f, 1.0f, 1.0f};

    Transform2DSetScale(widget, tempSize);
    Transform2DSetPosition(widget, position);
}

void WindowWidgetInit(EWidgetWindow *ww, char* name, vec2 size, DrawParam dParam, vec2 position)
{

    InitTop(&ww->top, dParam, size, position);

    InitName(&ww->name, name, dParam, &ww->top);
    InitBot(&ww->widget, dParam, size, &ww->top);
    InitClose(&ww->close, dParam, size, &ww->top);
    InitResize(&ww->resize, dParam, size, &ww->top);
    InitHide(&ww->hide, dParam, size, &ww->top);

    WidgetConnect(&ww->top, GUI_TRIGGER_MOUSE_PRESS, WindowWidgetPress, NULL);
    WidgetConnect(&ww->top, GUI_TRIGGER_MOUSE_MOVE, WindowWidgetMove, ww);

    WidgetConnect(&ww->close, GUI_TRIGGER_MOUSE_PRESS, WindowWidgetCloseButton, ww);
    WidgetConnect(&ww->resize, GUI_TRIGGER_MOUSE_PRESS, WindowWidgetResizeButton, ww);
    WidgetConnect(&ww->hide, GUI_TRIGGER_MOUSE_PRESS, WindowWidgetHideButton, ww);

    ww->show = true;
    ww->wasHide = false;
    ww->wasResize = false;
}

void WindowWidgetShow(EWidgetWindow *ww){
    ww->show = true;
}

void WindowWidgetHide(EWidgetWindow *ww){
    ww->show = false;
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
