#include "e_widget_entry.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "e_tools.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

bool e_ctrl_press = false, e_c_press = false, e_v_press = false, e_pasted = false, e_copied = false;

int EntryWidgetCharInput(EWidget* widget, uint32_t codepoint, void *arg){

    EWidgetEntry *temp = widget;

    EWidgetText *text = WidgetFindChild(&temp->widget, temp->curr_texts)->node;

    if(text->tData.textWidth >= temp->width)
        return;

    if(codepoint > 256)
    {
        for(int i=0;i < 66; i++)
        {
            if(fontIndexes[i].FindLetter == codepoint)
            {
                uint16_t t = fontIndexes[i].IndexLetter & 0xFF;
                temp->buffers[temp->curr_line][temp->currPos + 1] = t;
                t = fontIndexes[i].IndexLetter >> 8;
                temp->buffers[temp->curr_line][temp->currPos] = t;
                temp->currPos+=2;
            }
        }
    }else{
        temp->buffers[temp->curr_line][temp->currPos] = codepoint;
        temp->currPos++;
    }

    TextWidgetSetText(text, temp->buffers[temp->curr_line]);

    return 0;
}

int EntryWidgetKeyPressInput(EWidget* widget, int key, void *arg){

    EWidgetEntry *temp = widget;

    if(key == GLFW_KEY_BACKSPACE)
    {

        temp->buffers[temp->curr_line][temp->currPos] = 0;

        temp->currPos--;

        if(temp->currPos < 0)
        {
            temp->currPos = 0;
        }

        if(temp->buffers[temp->curr_line][temp->currPos] < 0)
        {
            temp->buffers[temp->curr_line][temp->currPos] = 0;

            temp->currPos--;
        }

        temp->buffers[temp->curr_line][temp->currPos] = 0;
    }

    if(temp->text.tData.textWidth >= temp->width)
        return;

    if(e_ctrl_press == true && e_v_press == true && !e_pasted)
    {

        char *clipboard = glfwGetClipboardString(e_window);

        uint32_t size = strlen(clipboard);

        char *point = &temp->buffers[temp->curr_line][temp->currPos];

        for(int i=0;i < size;i++)
        {
            point[i] = clipboard[i];

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

    return 0;
}

int EntryWidgetKeyRepeatInput(EWidget* widget, int key, void *arg){
    EWidgetEntry *temp = widget;

    if(key == GLFW_KEY_BACKSPACE)
    {
        temp->buffers[temp->curr_line][temp->currPos] = 0;

        temp->currPos--;

        if(temp->currPos < 0)
        {
            temp->currPos = 0;
        }


        if(temp->buffers[temp->curr_line][temp->currPos] < 0)
        {
            temp->buffers[temp->curr_line][temp->currPos] = 0;

            temp->currPos--;
        }

        temp->buffers[temp->curr_line][temp->currPos] = 0;
    }

    return 0;
}

void EntryWidgetCharacterCallback(void* arg, uint32_t codepoint)
{
    if(e_var_current_entry == NULL)
            return;

    WidgetConfirmTrigger(e_var_current_entry, ENGINE_WIDGET_TRIGGER_ENTRY_CHAR_INPUT, codepoint);

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
        WidgetConfirmTrigger(e_var_current_entry, ENGINE_WIDGET_TRIGGER_ENTRY_KEY_PRESS_INPUT, key);
    else if(action == GLFW_REPEAT)
        WidgetConfirmTrigger(e_var_current_entry, ENGINE_WIDGET_TRIGGER_ENTRY_KEY_REPEAT_INPUT, key);
    else if(action == GLFW_RELEASE)
        WidgetConfirmTrigger(e_var_current_entry, ENGINE_WIDGET_TRIGGER_ENTRY_KEY_RELEASE_INPUT, key);
}

int EntryWidgetPress(EWidget *widget, void *entry, void *arg){

    e_var_current_entry = (EWidget *)widget;

    return 0;
}

int EntryWidgetUnfocus(EWidget *widget, void *entry, void *arg){

    EWidgetEntry *temp = widget;

    temp->buffers[temp->curr_line][temp->currPos] = 0;

    EWidgetText *text = WidgetFindChild(&temp->widget, temp->curr_texts)->node;

    TextWidgetSetText(text, temp->buffers[temp->curr_line]);

    return 0;
}

void EntryWidgetUpdate(){

    if(e_var_current_entry == NULL)
            return;

    EWidgetEntry *temp = e_var_current_entry;

    vec2 scale = Transform2DGetScale(temp);
    temp->width = scale.x * 1.5f * (9 / temp->text.tData.font.fontSize);
    temp->height = scale.y * 2.5f;

    temp->buffers[temp->curr_line][temp->currPos] = L'|';

    EWidgetText *text = WidgetFindChild(&temp->widget, temp->curr_texts)->node;

    TextWidgetSetText(text, temp->buffers[temp->curr_line]);

    WidgetConfirmTrigger(temp, ENGINE_WIDGET_TRIGGER_ENTRY_UPDATE, NULL);
}

int EntryWidgetDestroyDefault(EWidgetEntry *entry)
{
    GameObject2DDestroy(entry);

    for(int i=0; i < entry->num_lines;i++)
    {
        free(entry->buffers[i]);
    }
    free(entry->buffers);
}

void EntryWidgetInit(EWidgetEntry *entry, int fontSize, DrawParam *dParam, EWidget* parent){


    if(fontSize > 16)
        fontSize = 16;
    else if(fontSize <= 0)
        fontSize = 2;

    WidgetInit(&entry->widget, dParam, parent);
    WidgetAddDefault(&entry->widget, dParam->render);
    GameObject2DInitDraw(&entry->widget);

    GameObjectSetDestroyFunc(entry, (void *)EntryWidgetDestroyDefault);

    memcpy(entry->widget.go.name, "Entry", 5);
    entry->widget.type = ENGINE_WIDGET_TYPE_ENTRY;

    entry->widget.color = (vec4){0.7, 0.7, 0.7, 1.0f};

    TextWidgetInitDefault(&entry->text, fontSize, dParam, &entry->widget);
    Transform2DSetPosition(&entry->text, 0, 10 + (fontSize * 2) );

    entry->currPos = 0;

    entry->buffers = calloc(1, sizeof(uint32_t *));
    entry->buffers[0] = calloc(BUFFER_SIZE, sizeof(uint32_t));
    memset(entry->buffers[0], 0, BUFFER_SIZE * sizeof(uint32_t));
    entry->num_lines = 1;
    entry->curr_line = 0;

    WidgetConnect(entry, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, EntryWidgetPress, NULL);
    WidgetConnect(entry, ENGINE_WIDGET_TRIGGER_WIDGET_UNFOCUS, EntryWidgetUnfocus, NULL);

    WidgetConnect(entry, ENGINE_WIDGET_TRIGGER_ENTRY_CHAR_INPUT, EntryWidgetCharInput, NULL);
    WidgetConnect(entry, ENGINE_WIDGET_TRIGGER_ENTRY_KEY_PRESS_INPUT, EntryWidgetKeyPressInput, NULL);
    WidgetConnect(entry, ENGINE_WIDGET_TRIGGER_ENTRY_KEY_REPEAT_INPUT, EntryWidgetKeyRepeatInput, NULL);

}

char *EntryWidgetGetText(EWidgetEntry *entry)
{
    return entry->buffers[entry->curr_line];
}

void EntryWidgetSetText(EWidgetEntry *entry, char *text)
{
    EWidgetText *temp = WidgetFindChild(&entry->widget, entry->curr_texts)->node;

    memset(entry->buffers[entry->curr_line], 0, BUFFER_SIZE);

    int len = strlen(text);
    for(int i=0;i < len;i++)
    {
        entry->buffers[entry->curr_line][i] = text[i];
    }

    entry->currPos = len;

    TextWidgetSetText(temp, entry->buffers[entry->curr_line]);
}

void EntryWidgetCleartext(EWidgetEntry *entry)
{
    memset(entry->buffers[entry->curr_line], 0, BUFFER_SIZE);

    TextImageClearText(&entry->text, &entry->text.tData);
}

void EntryWidgetSetTextW(EWidgetEntry *entry, uint32_t *text)
{
    EWidgetText *temp = WidgetFindChild(&entry->widget, entry->curr_texts)->node;

    memset(entry->buffers[entry->curr_line], 0, BUFFER_SIZE);

    int len = strlen(text);
    for(int i=0;i < len;i++)
    {
        entry->buffers[entry->curr_line][i] = text[i];
    }

    entry->currPos = len;

    TextWidgetSetText(temp, entry->buffers[entry->curr_line]);
}

