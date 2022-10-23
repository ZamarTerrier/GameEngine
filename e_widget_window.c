#include "e_widget_window.h"

#include "e_math.h"

#include "engine.h"

vec2 mouse,  temp, tscale ;

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

        vec2 position = {(scale.x - 10) * 2 , 0};
        Transform2DSetPosition(widget->child->next->node, position);
        position = (vec2){(scale.x - 20) * 2 , 0};
        Transform2DSetPosition(widget->child->next->next->node, position);
        position = (vec2){(scale.x - 30) * 2 , 0};
        Transform2DSetPosition(widget->child->next->next->next->node, position);

        Transform2DSetScale(widget, scale);
        scale.x -= 2;
        scale.y -= 12;
        Transform2DSetScale(widget->child->node, scale);
    }

}

void WindowWidgetAction(EWidget* widget, void* entry, void *arg){

    vec2 pos = {0,0};

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

    addUniformObject(&widget->go.graphObj.local, sizeof(GUIBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

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

    widget->color = (vec3){0.4, 0.1, 0.1};

    widget->offset.x = 0;
    widget->offset.y = 0;

    widget->parent = NULL;
    widget->child = NULL;

    widget->in = widget->was_in = widget->was_out = widget->out = false;

    widget->callbacks.stack = (CallbackStruct *) calloc(0, sizeof(CallbackStruct));

    PipelineCreateGraphics(&widget->go.graphObj);

    Transform2DSetScale(widget, size);
    Transform2DSetPosition(widget, position);

    widget->color = (vec3){1, 1, 1};
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

    widget->color = (vec3){ 1.0f, 0.0f, 0.0f};

    Transform2DSetScale(widget, tempSize);
    Transform2DSetPosition(widget, position);
}

void InitResize(EWidget* widget, DrawParam dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, dParam, parent);

    vec2 tempSize = {10, 10};
    vec2 position = {(size.x - 20) * 2 , 0};

    widget->color = (vec3){ 0.0f, 1.0f, 0.0f};

    Transform2DSetScale(widget, tempSize);
    Transform2DSetPosition(widget, position);
}

void InitHide(EWidget* widget, DrawParam dParam, vec2 size, EWidget *parent){

    WidgetInit(widget, dParam, parent);

    vec2 tempSize = {10, 10};
    vec2 position = {(size.x - 30) * 2 , 0};

    widget->color = (vec3){ 0.0f, 0.0f, 1.0f};

    Transform2DSetScale(widget, tempSize);
    Transform2DSetPosition(widget, position);
}

void WindowWidgetInit(EWidgetWindow *ww, DrawParam dParam, vec2 size, vec2 position){

    InitTop(&ww->top, dParam, size, position);

    InitBot(&ww->widget, dParam, size, &ww->top);
    InitClose(&ww->close, dParam, size, &ww->top);
    InitResize(&ww->resize, dParam, size, &ww->top);
    InitHide(&ww->hide, dParam, size, &ww->top);

    WidgetConnect(&ww->top, GUI_TRIGGER_MOUSE_PRESS, WindowWidgetPress, NULL);
    WidgetConnect(&ww->top, GUI_TRIGGER_MOUSE_MOVE, WindowWidgetMove, NULL);

    WidgetConnect(&ww->close, GUI_TRIGGER_MOUSE_PRESS, WindowWidgetAction, NULL);
}

void WindowWidgetUpdate(EWidgetWindow *ww){
    WidgetEventsPipe(&ww->top);
}

void WindowWidgetDraw(EWidgetWindow *ww){
    WidgetDraw(&ww->top);
}

void WindowWidgetDestroy(EWidgetWindow *ww){
    WidgetDestroy(&ww->top);
}
