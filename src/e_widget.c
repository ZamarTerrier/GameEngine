#include "e_widget.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "e_widget_entry.h"

#include "e_math.h"


#include "engine.h"

#include "pipeline.h"
#include "buffers.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_export.h"

bool e_var_wasReleased = true, e_var_leftMouse = false;

EWidget* e_var_sellected = NULL;
EWidget* e_var_last_sellected = NULL;

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
    gb.transparent = ew->transparent;

    vkMapMemory(e_device, ew->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(gb), 0, &data);
    memcpy(data, &gb, sizeof(gb));
    vkUnmapMemory(e_device,  ew->go.graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

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

            if(iter >= MAX_WIDGET_MASKS)
                break;
        }

        mbo.size = iter;
    }
    else
    {
        mbo.size = 0;
    }

    vkMapMemory(e_device, ew->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
    memcpy(data, &mbo, sizeof(mbo));
    vkUnmapMemory(e_device,  ew->go.graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);

}

void WidgetSetParent(EWidget* ew, EWidget* parent){

    ew->parent = parent;
    ew->child = NULL;
    ew->last = NULL;

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
            child->next->before = child;
            child->next->node = ew;

            parent->last = child->next;

        }else{
            parent->child = (ChildStack *)calloc(1, sizeof(ChildStack));
            parent->child->next = NULL;
            parent->child->before = NULL;
            parent->child->node = ew;
            parent->last = parent->child;
        }
    }
}

int WidgetFindIdChild(EWidget* widget)
{
    EWidget *parent = widget->parent;

    ChildStack *child = parent->child;
    int counter = 0;

    while(child != NULL && child->node != widget)
    {
        counter ++;
        child = child->next;
    }

    if(child == NULL && counter == 0)
        return -1;

    return counter;
}

ChildStack * WidgetFindChild(EWidget* widget, int num)
{
    ChildStack *child = widget->child;
    int counter = 0;

    if(child != NULL)
    {
        while(child->next != NULL && counter < num)
        {
            counter ++;
            child = child->next;
        }

        if(counter != num)
            return NULL;
    }

    return child;
}

void WidgetInit(EWidget* ew, DrawParam *dParam, EWidget* parent){

    memcpy(ew->go.name, "Widget", 6);

    ew->type = ENGINE_WIDGET_TYPE_WIDGET;

    GameObject2DInit(&ew->go);

    GraphicsObjectSetVertexSize(&ew->go.graphObj, sizeof(Vertex2D), sizeof(uint32_t));
    GraphicsObjectSetVertex(&ew->go.graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObject2DSetLinkedShape(ew);

    GameObjectSetUpdateFunc(&ew->go, (void *)WidgetUniformUpdate);

    if(dParam != NULL)
        GraphicsObjectSetShadersPath(&ew->go.graphObj, dParam->vertShader, dParam->fragShader);

    BuffersAddUniformObject(&ew->go.graphObj.local, sizeof(GUIBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);
    BuffersAddUniformObject(&ew->go.graphObj.local, sizeof(MaskObjectBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);

    ew->go.image = calloc(1, sizeof(GameObjectImage));

    if(dParam != NULL)
        if(strlen(dParam->diffuse) != 0)
        {
            int len = strlen(dParam->diffuse);
            ew->go.image->path = calloc(len + 1, sizeof(char));
            memcpy(ew->go.image->path, dParam->diffuse, len);
            ew->go.image->path[len] = '\0';
            //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
        }

    TextureImageAdd(&ew->go.graphObj.local, ew->go.image, 0, 0);

    GraphicsObjectCreateDrawItems(&ew->go.graphObj);

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

    GameObject2DAddSettingPipeline(&ew->go, &setting);

    ew->color = (vec4){0.2, 0.2, 0.2, 1.0};

    ew->offset.x = 0;
    ew->offset.y = 0;
    ew->transparent = 1.0f;

    WidgetSetParent(ew, parent);

    ew->widget_flags = ENGINE_FLAG_WIDGET_ACTIVE | ENGINE_FLAG_WIDGET_VISIBLE | ENGINE_FLAG_WIDGET_SELF_VISIBLE;

    ew->callbacks.stack = (CallbackStruct *) calloc(MAX_GUI_CALLBACKS, sizeof(CallbackStruct));
    ew->callbacks.size = 0;

    PipelineCreateGraphics(&ew->go.graphObj);
}

void WidgetConnect(EWidget* widget, int trigger, widget_callback callback, void* args){

    if(widget->callbacks.size + 1 >= MAX_GUI_CALLBACKS)
    {
        printf("Слишком много калбэков!\n");
        return;
    }

    widget->callbacks.stack[widget->callbacks.size].args = args;
    widget->callbacks.stack[widget->callbacks.size].func = callback;
    widget->callbacks.stack[widget->callbacks.size].trigger = trigger;

    widget->callbacks.size ++;
}

void WidgetConfirmTrigger(EWidget* widget, int trigger, void *entry){
    int res = 0;

    for(int i=0;i < widget->callbacks.size;i++)
    {
        if(widget->callbacks.stack[i].trigger == trigger)
        {
            widget_callback temp = widget->callbacks.stack[i].func;
            res = temp(widget, entry,  widget->callbacks.stack[i].args);

            if(res < 0)
                return;
        }
    }
}

EWidget* WidgetCheckMouseInner(EWidget* widget){

    if(!(widget->widget_flags & ENGINE_FLAG_WIDGET_ACTIVE) || !(widget->widget_flags & ENGINE_FLAG_WIDGET_VISIBLE))
        return NULL;

    double xpos, ypos;
    glfwGetCursorPos(e_window, &xpos, &ypos);

    xpos /=WIDTH;
    ypos /=HEIGHT;

    if(xpos > widget->position.x && xpos < (widget->position.x + (widget->scale.x * 2)) &&
            ypos > widget->position.y && ypos < (widget->position.y + (widget->scale.y * 2)))
    {

        ChildStack *next = widget->last;

        while(next != NULL)
        {
            if(next->node != NULL)
            {
                EWidget* res = WidgetCheckMouseInner(next->node);

                if(res != NULL)
                {
                    widget->widget_flags |= ENGINE_FLAG_WIDGET_OUT;
                    return res;
                }
            }

            next = next->before;
        }

        return widget;
    }

    widget->widget_flags |= ENGINE_FLAG_WIDGET_OUT;
    return NULL;
}


void WidgetEventsPipe(EWidget* widget)
{

    if(e_var_wasReleased && e_var_sellected == NULL){
        e_var_sellected = WidgetCheckMouseInner(widget);
    }

    if(e_var_sellected != NULL)
    {
        widget->widget_flags |= ENGINE_FLAG_WIDGET_IN;

        if((e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_OUT) && !(e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_IN))
            WidgetConfirmTrigger(e_var_sellected, ENGINE_WIDGET_TRIGGER_MOUSE_IN, NULL);
        else if(!(e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_OUT) && (e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_IN))
            WidgetConfirmTrigger(e_var_sellected, ENGINE_WIDGET_TRIGGER_MOUSE_OUT, NULL);
        else if(!(e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_OUT)&& (e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_IN))
            WidgetConfirmTrigger(e_var_sellected, ENGINE_WIDGET_TRIGGER_MOUSE_STAY, NULL);

        if(e_var_leftMouse && e_var_wasReleased)
        {
            if(e_var_current_entry != e_var_sellected)
                e_var_current_entry = NULL;

            WidgetConfirmTrigger(e_var_sellected, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, NULL);
            e_var_wasReleased = false;

            if(e_var_last_sellected != e_var_sellected){

                if(e_var_last_sellected != NULL)
                    WidgetConfirmTrigger(e_var_last_sellected, ENGINE_WIDGET_TRIGGER_WIDGET_UNFOCUS, NULL);

                WidgetConfirmTrigger(e_var_sellected, ENGINE_WIDGET_TRIGGER_WIDGET_FOCUS, NULL);
            }

            e_var_last_sellected = e_var_sellected;
        }
        else if(e_var_leftMouse)
            WidgetConfirmTrigger(e_var_sellected, ENGINE_WIDGET_TRIGGER_MOUSE_MOVE, NULL);
        else if(!e_var_leftMouse && !e_var_wasReleased)
        {
            WidgetConfirmTrigger(e_var_sellected, ENGINE_WIDGET_TRIGGER_MOUSE_RELEASE, NULL);
            e_var_wasReleased = true;
        }else
        {
            e_var_sellected->widget_flags |= (((e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_OUT) | (e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_OUT)) | \
                                            ((e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_WAS_IN) | (e_var_sellected->widget_flags & ENGINE_FLAG_WIDGET_IN)));
        }

    }

    int state = glfwGetMouseButton(e_window, GLFW_MOUSE_BUTTON_LEFT);

    if(state == GLFW_PRESS)
        e_var_leftMouse = true;
    else
        e_var_leftMouse = false;

    if(e_var_wasReleased){
        e_var_sellected = NULL;
    }

}

void WidgetDraw(EWidget * widget)
{
    if(!(widget->widget_flags & ENGINE_FLAG_WIDGET_VISIBLE))
        return;

    ChildStack *child = widget->child;
    EngineDraw(widget);
    while(child != NULL)
    {
        WidgetDraw(child->node);

        child = child->next;
    }
}

void WidgetRecreate(EWidget * widget){
    EngineDeviceWaitIdle();
    ChildStack *child = widget->child;
    GameObjectClean(widget);
    GameObjectRecreate(widget);
    while(child != NULL)
    {
        WidgetRecreate(child->node);
        child = child->next;
    }
}

void WidgetDestroy(EWidget *widget){
    ChildStack *child = widget->child;
    ChildStack *lastChild;
    while(child != NULL)
    {
        WidgetDestroy(child->node);
        lastChild = child;
        child = child->next;
        free(lastChild);
    }
    GameObjectDestroy(widget);

    free(widget->callbacks.stack);

    if((widget->widget_flags & ENGINE_FLAG_WIDGET_ALLOCATED))
        free(widget);
}
