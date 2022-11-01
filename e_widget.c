#include "e_widget.h"
#include "e_widget_entry.h"

#include "e_math.h"

#include <GLFW/glfw3.h>

#include "engine.h"

#include "pipeline.h"

#include "e_resource.h"

bool wasReleased = true, leftMouse = false;

EWidget* sellected = NULL;
EWidget* last_sellected = NULL;

void WidgetUniformUpdate(EWidget *ew){

    void* data;

    GUIBuffer gb = {};
    gb.offset.x = ew->offset.x > 0 ? ew->offset.x / (WIDTH) : 0;
    gb.offset.y = ew->offset.y > 0 ? ew->offset.y / (HEIGHT) : 0;

    vec2 offset = {0, 0};
    if(ew->parent != NULL)
    {
        offset = v2_div(ew->parent->offset, (vec2){ WIDTH, HEIGHT});
        ew->position = v2_add(v2_add(ew->go.transform.position, ew->parent->position), offset);
    }
    else
        ew->position = ew->go.transform.position;

    ew->scale = ew->go.transform.scale;

    gb.position = ew->position;
    gb.size = ew->scale;
    gb.color = ew->color;

    vkMapMemory(device, ew->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(gb), 0, &data);
    memcpy(data, &gb, sizeof(gb));
    vkUnmapMemory(device,  ew->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

    MaskObjectBuffer mbo = {};

    if(ew->parent != NULL)
    {

        EWidget* parent= ew->parent;

        int iter = 0;

        while(parent != NULL)
        {
            offset = v2_div(parent->offset, (vec2){ WIDTH, HEIGHT});
            mbo.objs[iter].position = v2_add(parent->position, offset);
            mbo.objs[iter].size = v2_sub(parent->go.transform.scale, v2_divs(offset, 2));
            iter ++;
            parent = parent->parent;
        }

        mbo.size = iter;
    }
    else
    {
        mbo.size = 0;
    }

    vkMapMemory(device, ew->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
    memcpy(data, &mbo, sizeof(mbo));
    vkUnmapMemory(device,  ew->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);

}

void WidgetSetParent(EWidget* ew, EWidget* parent){

    ew->parent = parent;
    ew->child = NULL;

    if(parent != NULL)
    {
        if(parent->child != NULL)
        {
            ChildStack *child = parent->child;

            while(child->next != NULL)
            {
                child = child->next;
            }

            child->next = (ChildStack *)calloc(1, sizeof(ChildStack));
            child->next->next = NULL;
            child->next->node = ew;
        }else{
            parent->child = (ChildStack *)calloc(1, sizeof(ChildStack));
            parent->child->next = NULL;
            parent->child->node = ew;
        }
    }
}

 ChildStack * WidgetFindChild(EWidget* widget, int num)
{
    ChildStack *child = widget->child;
    int counter = 0;

    if(child != NULL)
    {
        while(child->next != NULL && num > 0 && counter != num)
        {
            counter ++;
            child = child->next;
        }
    }

    return child;
}

void WidgetInit(EWidget* ew, DrawParam dParam, EWidget* parent){
    GameObject2DInit(&ew->go);

    GraphicsObject2DSetVertex(&ew->go.graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObjectSetUpdateFunc(&ew->go, (void *)WidgetUniformUpdate);

    GraphicsObjectSetShadersPath(&ew->go.graphObj, dParam.vertShader, dParam.fragShader);

    BuffersAddUniformObject(&ew->go.graphObj.local, sizeof(GUIBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
    BuffersAddUniformObject(&ew->go.graphObj.local, sizeof(MaskObjectBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    GameObject2DCreateDrawItems(&ew->go);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&ew->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_gui_widget_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_gui_widget_vert_spv_size);
        setting.fragShader = &_binary_shaders_gui_widget_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_gui_widget_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(&ew->go, setting);

    ew->color = (vec4){0.4, 0.1, 0.1, 1.0};

    ew->offset.x = 0;
    ew->offset.y = 0;

    WidgetSetParent(ew, parent);

    ew->in = ew->was_in = ew->was_out = ew->out = false;

    ew->callbacks.stack = (CallbackStruct *) calloc(0, sizeof(CallbackStruct));

    PipelineCreateGraphics(&ew->go.graphObj);

    ew->active = true;
}

void WidgetConnect(EWidget* widget, int trigger, void* callback, void* args){
    widget->callbacks.size ++;
    widget->callbacks.stack = (CallbackStruct *) realloc(widget->callbacks.stack, widget->callbacks.size * sizeof(CallbackStruct));
    widget->callbacks.stack[widget->callbacks.size - 1].args = args;
    widget->callbacks.stack[widget->callbacks.size - 1].func = callback;
    widget->callbacks.stack[widget->callbacks.size - 1].trigger = trigger;
}

void WidgetConfirmTrigger(EWidget* widget, int trigger, void *entry){
    for(int i=0;i < widget->callbacks.size;i++)
    {
        if(widget->callbacks.stack[i].trigger == trigger)
        {
            void(*func)(EWidget *widget, void *, void*) = widget->callbacks.stack[i].func;
            func(widget, entry,  widget->callbacks.stack[i].args);
        }
    }
}


EWidget* WidgetCheckMouseInner(EWidget* widget){

    GameObjectUpdate(widget);

    if(!widget->active)
        return NULL;

    double xpos, ypos;
    glfwGetCursorPos(e_window, &xpos, &ypos);

    xpos /=WIDTH;
    ypos /=HEIGHT;

    if(xpos > widget->position.x && xpos < (widget->position.x + (widget->scale.x * 2)) &&
            ypos > widget->position.y && ypos < (widget->position.y + (widget->scale.y * 2)))
    {

        ChildStack *next = widget->child;

        while(next != NULL)
        {
            EWidget* res = WidgetCheckMouseInner(next->node);

            if(res != NULL)
            {
                widget->out = true;
                return res;
            }

            next = next->next;
        }

        return widget;
    }

    widget->out = true;
    return NULL;
}


void WidgetEventsPipe(EWidget* widget)
{

    if(wasReleased && sellected == NULL){
        sellected = WidgetCheckMouseInner(widget);
    }

    if(sellected != NULL)
    {
        sellected->in = true;

        if(sellected->was_out && !sellected->was_in)
            WidgetConfirmTrigger(sellected, GUI_TRIGGER_MOUSE_IN, NULL);
        else if(!sellected->was_out && sellected->was_in)
            WidgetConfirmTrigger(sellected, GUI_TRIGGER_MOUSE_OUT, NULL);
        else if(!sellected->was_out && sellected->was_in)
            WidgetConfirmTrigger(sellected, GUI_TRIGGER_MOUSE_STAY, NULL);

        if(leftMouse && wasReleased)
        {
            if(current_entry != sellected)
                current_entry = NULL;

            WidgetConfirmTrigger(sellected, GUI_TRIGGER_MOUSE_PRESS, NULL);
            wasReleased = false;

            if(last_sellected != sellected){

                if(last_sellected != NULL)
                    WidgetConfirmTrigger(last_sellected, GUI_TRIGGER_WIDGET_UNFOCUS, NULL);

                WidgetConfirmTrigger(sellected, GUI_TRIGGER_WIDGET_FOCUS, NULL);
            }

            last_sellected = sellected;
        }
        else if(leftMouse)
            WidgetConfirmTrigger(sellected, GUI_TRIGGER_MOUSE_MOVE, NULL);
        else if(!leftMouse && !wasReleased)
        {
            WidgetConfirmTrigger(sellected, GUI_TRIGGER_MOUSE_RELEASE, NULL);
            wasReleased = true;
        }

        sellected->was_out = sellected->out;
        sellected->was_in = sellected->in;
    }

    int state = glfwGetMouseButton(e_window, GLFW_MOUSE_BUTTON_LEFT);

    if(state == GLFW_PRESS)
        leftMouse = true;
    else
        leftMouse = false;

    if(wasReleased){
        sellected = NULL;
    }

}

void WidgetDraw(EWidget * widget){
    ChildStack *child = widget->child;
    GameObjectDraw(widget);
    while(child != NULL)
    {
        WidgetDraw(child->node);
        child = child->next;
    }
}

void WidgetDestroy(EWidget * widget){
    ChildStack *child = widget->child;
    while(child != NULL)
    {
        WidgetDestroy(child->node);
        child = child->next;
    }
    GameObjectDestroy(widget);
}
