#include "e_widget.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "e_widget_entry.h"

#include "e_math.h"


#include "engine.h"

#include "pipeline.h"
#include "e_buffer.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_export.h"

bool e_var_wasReleased = true, e_var_leftMouse = false;

EWidget* e_var_sellected = NULL;
EWidget* e_var_last_sellected = NULL;

void WidgetUpdateScissor(EWidget *widget, EIRect2D *scissor, vec2 *parent_pos, vec2 *offset)
{

    vec2 parentSize = {WIDTH, HEIGHT};

    EWidget* parent = widget->parent;

    if(parent != NULL)
    {
        *parent_pos = parent->position;
        parentSize = parent->scale;

        while(parent->parent != NULL){

            parent = parent->parent;

            *offset = v2_div(parent->offset, vec2_f( WIDTH, HEIGHT));

            vec2 temp = v2_add(parent->position, *offset);

            if(parent_pos->x < temp.x)
            {
                parentSize.x = parentSize.x - (temp.x - parent_pos->x) / 2;
                parent_pos->x = temp.x;
            }

            if(parent_pos->y < temp.y)
            {
                parentSize.y = parentSize.y - (temp.y - parent_pos->y) / 2;
                parent_pos->y = temp.y;
            }

            if(parent_pos->x + (parentSize.x * 2) > parent->position.x + (parent->scale.x * 2))
            {
                parent_pos->x = parent->position.x;
                parentSize.x = parent->scale.x;
            }

            if(parent_pos->y + (parentSize.y * 2) > parent->position.y + (parent->scale.y * 2))
            {
                parent_pos->y = parent->position.y;
                parentSize.y = parent->scale.y;
            }

        }

    }

    scissor->offset.x = parent_pos->x * WIDTH;

    if(scissor->offset.x < 0)
        scissor->offset.x = 0;

    scissor->offset.y = parent_pos->y * HEIGHT;

    if(scissor->offset.y < 0)
        scissor->offset.y = 0;

    scissor->extent.height = parentSize.y * 2 * HEIGHT;

    if(scissor->extent.height > HEIGHT)
        scissor->extent.height = 0;

    scissor->extent.width = parentSize.x * 2 * WIDTH;

    if(scissor->extent.width > WIDTH)
        scissor->extent.width = 0;

}

void WidgetGUIBufferUpdate(EWidget *ew, BluePrintDescriptor *descriptor)
{
    GUIBuffer gb;
    memset(&gb, 0, sizeof(GUIBuffer));

    gb.offset.x = ew->offset.x > 0 ? ew->offset.x / (WIDTH) : 0;
    gb.offset.y = ew->offset.y > 0 ? ew->offset.y / (HEIGHT) : 0;

    vec2 offset = {0, 0};
    if(ew->parent != NULL)
    {
        offset = v2_div(ew->parent->offset, vec2_f( WIDTH / 2, HEIGHT / 2));
        ew->position = v2_add(v2_add(v2_divs(ew->go.transform.position, 2.0), ew->parent->position), offset);
    }
    else
        ew->position = v2_divs(ew->go.transform.position, 2.0);

    ew->scale = ew->go.transform.scale;

    gb.position = ew->position;
    gb.size = ew->scale;
    gb.color = ew->color;
    gb.transparent = ew->transparent;

    DescriptorUpdate(descriptor, &gb, sizeof(gb));
}

void WidgetMaskObjectUpdate(EWidget *ew, BluePrintDescriptor *descriptor)
{
    MaskObjectBuffer mbo = {};

    vec2 offset = {0, 0};
    if(ew->parent != NULL)
    {

        EWidget* parent= ew->parent;

        int iter = 0;

        while(parent != NULL)
        {
            offset = v2_div(parent->offset, vec2_f( WIDTH, HEIGHT));
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

    DescriptorUpdate(descriptor, &mbo, sizeof(mbo));
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

    GraphicsObjectSetVertex(&ew->go.graphObj, projPlaneVert, 4, sizeof(Vertex2D), projPlaneIndx, 6, sizeof(uint32_t));

    if(dParam != NULL)
        GraphicsObjectSetShadersPath(&ew->go.graphObj, dParam->vertShader, dParam->fragShader);

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

    ew->color = (vec4){0.2, 0.2, 0.2, 1.0};

    ew->offset.x = 0;
    ew->offset.y = 0;
    ew->transparent = 1.0f;

    WidgetSetParent(ew, parent);

    ew->widget_flags = ENGINE_FLAG_WIDGET_ACTIVE | ENGINE_FLAG_WIDGET_VISIBLE | ENGINE_FLAG_WIDGET_SELF_VISIBLE;

    ew->callbacks.stack = (CallbackStruct *) calloc(MAX_GUI_CALLBACKS, sizeof(CallbackStruct));
    ew->callbacks.size = 0;

}

void WidgetAddDefault(EWidget *widget, void *render)
{
    uint32_t nums = widget->go.graphObj.blueprints.num_blue_print_packs;
    widget->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&widget->go.graphObj.blueprints, nums, sizeof(GUIBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)WidgetGUIBufferUpdate, 0);
    BluePrintAddUniformObject(&widget->go.graphObj.blueprints, nums, sizeof(MaskObjectBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)WidgetMaskObjectUpdate, 0);

    BluePrintAddTextureImage(&widget->go.graphObj.blueprints, nums, widget->go.image, VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&widget->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_gui_widget_vert_spv_start, (size_t)(&_binary_shaders_gui_widget_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_gui_widget_frag_spv_start, (size_t)(&_binary_shaders_gui_widget_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;

    GameObject2DAddSettingPipeline(&widget->go, nums, &setting);

    widget->go.graphObj.blueprints.num_blue_print_packs ++;
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
