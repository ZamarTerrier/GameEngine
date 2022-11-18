#include "e_widget_entry.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "e_resource.h"

#include "tools.h"

bool e_ctrl_press = false, e_c_press = false, e_v_press = false, e_pasted = false, e_copied = false;

void EntryWidgetCharInput(EWidget* widget, uint32_t codepoint, void *arg){

    EWidgetEntry *temp = widget;

    EWidgetText *text = WidgetFindChild(&temp->widget, temp->curr_texts)->node;

    if(text->tData.textWidth >= temp->width)
        return;

    temp->buffers[temp->curr_line][temp->currPos] = codepoint;
    temp->currPos++;

    TextWidgetSetText(text, temp->buffers[temp->curr_line]);
}

void EntryWidgetKeyPressInput(EWidget* widget, int key, void *arg){

    EWidgetEntry *temp = widget;

    if(key == GLFW_KEY_BACKSPACE)
    {
        temp->buffers[temp->curr_line][temp->currPos] = 0;

        temp->currPos--;

        if(temp->currPos < 0)
        {
            temp->currPos = 0;
        }

        temp->buffers[temp->curr_line][temp->currPos] = 0;
    }

    if(temp->text.tData.textWidth >= temp->width)
        return;

    if(e_ctrl_press == true && e_v_press == true && !e_pasted)
    {
        uint32_t *point = &temp->buffers[temp->curr_line][temp->currPos];

        char *text = glfwGetClipboardString(e_window);

        uint32_t size = strlen(text);

        for(int i=0;i < size;i++)
        {
            point[i] = text[i];

            temp->currPos ++;

            EWidgetText *text = WidgetFindChild(&temp->widget, temp->curr_texts)->node;
            TextWidgetSetText(text, temp->buffers[temp->curr_line]);

            if(text->tData.textWidth >= temp->width)
            {
                e_pasted = true;
                break;
            }
        }

        e_pasted = true;
    }
}

void EntryWidgetKeyRepeatInput(EWidget* widget, int key, void *arg){
    EWidgetEntry *temp = widget;

    if(key == GLFW_KEY_BACKSPACE)
    {
        temp->buffers[temp->curr_line][temp->currPos] = 0;

        temp->currPos--;

        if(temp->currPos < 0)
        {
            temp->currPos = 0;
        }

        temp->buffers[temp->curr_line][temp->currPos] = 0;
    }
}

void EntryWidgetCharacterCallback(void* arg, uint32_t codepoint)
{
    if(e_var_current_entry == NULL)
            return;

    WidgetConfirmTrigger(e_var_current_entry, GUI_TRIGGER_ENTRY_CHAR_INPUT, codepoint);
}

void EntryWidgetKeyCallback(void* arg,  int key, int scancode, int action, int mods)
{
    if(e_var_current_entry == NULL)
            return;

    if(key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
        e_ctrl_press = true;
    else if(key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE){
        e_ctrl_press = false;

        if(e_v_press == false)
            e_pasted = false;

        if(e_c_press == false)
            e_copied = false;
    }

    if(key == GLFW_KEY_V && action == GLFW_PRESS)
        e_v_press = true;
    else if(key == GLFW_KEY_V && action == GLFW_RELEASE){
        e_v_press = false;
        e_pasted = false;
    }

    if(key == GLFW_KEY_C && action == GLFW_PRESS)
        e_c_press = true;
    else if(key == GLFW_KEY_C && action == GLFW_RELEASE){
        e_c_press = false;
        e_copied = false;
    }

    if(action == GLFW_PRESS)
        WidgetConfirmTrigger(e_var_current_entry, GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT, key);
    else if(action == GLFW_REPEAT)
        WidgetConfirmTrigger(e_var_current_entry, GUI_TRIGGER_ENTRY_KEY_REPEAT_INPUT, key);
    else if(action == GLFW_RELEASE)
        WidgetConfirmTrigger(e_var_current_entry, GUI_TRIGGER_ENTRY_KEY_RELEASE_INPUT, key);

}

void EntryWidgetPress(EWidget *widget, void *entry, void *arg){

    e_var_current_entry = (EWidget *)widget;

}

void EntryWidgetUnfocus(EWidget *widget, void *entry, void *arg){

    EWidgetEntry *temp = widget;

    temp->buffers[temp->curr_line][temp->currPos] = ' ';

    EWidgetText *text = WidgetFindChild(&temp->widget, temp->curr_texts)->node;

    TextWidgetSetText(text, temp->buffers[temp->curr_line]);
}

void EntryUpdateLine(){

    if(e_var_current_entry == NULL)
            return;

    EWidgetEntry *temp = e_var_current_entry;


    vec2 scale = Transform2DGetScale(temp);
    temp->width = scale.x * 3;
    temp->height = scale.y * 2.5f;

    temp->buffers[temp->curr_line][temp->currPos] = L'|';

    EWidgetText *text = WidgetFindChild(&temp->widget, temp->curr_texts)->node;

    TextWidgetSetText(text, temp->buffers[temp->curr_line]);

    WidgetConfirmTrigger(temp, GUI_TRIGGER_ENTRY_UPDATE, NULL);
}

void EntryWidgetInit(EWidgetEntry *entry, int fontSize, EWidget* parent){

    DrawParam dParam = {};
    memset(&dParam, 0, sizeof(DrawParam));

    if(fontSize > 16)
        fontSize = 16;
    else if(fontSize <= 0)
        fontSize = 2;

    WidgetInit(&entry->widget, dParam, parent);
    entry->widget.color = (vec4){0.7, 0.7, 0.7, 1.0f};

    TextWidgetInit(&entry->text, fontSize, dParam, &entry->widget);
    Transform2DSetPosition(&entry->text, 0, 25 - (25 / fontSize) );

    entry->currPos = 0;

    entry->buffers = calloc(1, sizeof(uint32_t *));
    entry->buffers[0] = calloc(BUFFER_SIZE, sizeof(uint32_t));
    memset(entry->buffers[0], 0, BUFFER_SIZE * sizeof(uint32_t));
    entry->num_lines = 1;
    entry->curr_line = 0;

    WidgetConnect(entry, GUI_TRIGGER_MOUSE_PRESS, EntryWidgetPress, NULL);
    WidgetConnect(entry, GUI_TRIGGER_WIDGET_UNFOCUS, EntryWidgetUnfocus, NULL);

    WidgetConnect(entry, GUI_TRIGGER_ENTRY_CHAR_INPUT, EntryWidgetCharInput, NULL);
    WidgetConnect(entry, GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT, EntryWidgetKeyPressInput, NULL);
    WidgetConnect(entry, GUI_TRIGGER_ENTRY_KEY_REPEAT_INPUT, EntryWidgetKeyRepeatInput, NULL);

}
