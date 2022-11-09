#include "e_widget_entry.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "e_resource.h"

void EntryWidgetCharInput(EWidget* widget, uint32_t codepoint, void *arg){

    EWidgetEntry *temp = widget;

    temp->buffer[temp->currPos] = codepoint;
    temp->currPos++;

    TextWidgetSetText(&temp->text, temp->buffer);

    if(temp->currPos > MAX_LINE_CHAR)
        temp->currPos = MAX_LINE_CHAR;
}

void EntryWidgetKeyPressInput(EWidget* widget, int key, void *arg){

    EWidgetEntry *temp = widget;

    if(key == GLFW_KEY_BACKSPACE)
    {
        temp->buffer[temp->currPos] = 0;

        temp->currPos--;

        if(temp->currPos < 0)
            temp->currPos = 0;

        temp->buffer[temp->currPos] = 0;
    }

    TextWidgetSetText(&temp->text, temp->buffer);
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

    if(action == GLFW_PRESS)
        WidgetConfirmTrigger(e_var_current_entry, GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT, key);
    else
        WidgetConfirmTrigger(e_var_current_entry, GUI_TRIGGER_ENTRY_KEY_RELEASE_INPUT, key);

}

void EntryWidgetPress(EWidget *widget, void *entry, void *arg){

    e_var_current_entry = (EWidget *)widget;

}

void EntryWidgetUnfocus(EWidget *widget, void *entry, void *arg){

    EWidgetEntry *temp = widget;

    temp->buffer[temp->currPos] = 0;

    TextWidgetSetText(&temp->text, temp->buffer);
}

void EntryUpdateLine(){

    if(e_var_current_entry == NULL)
            return;

    EWidgetEntry *temp = e_var_current_entry;

    temp->buffer[temp->currPos] = '|';

    TextWidgetSetText(&temp->text, temp->buffer);
}

void EntryWidgetInit(EWidgetEntry *entry, int fontSize, EWidget* parent){

    DrawParam dParam = {};

    if(fontSize > 16)
        fontSize = 16;
    else if(fontSize <= 0)
        fontSize = 2;

    WidgetInit(&entry->widget, dParam, parent);
    entry->widget.color = (vec4){0.7, 0.7, 0.7, 1.0f};

    TextWidgetInit(&entry->text, fontSize, dParam, &entry->widget);
    Transform2DSetPosition(&entry->text, 0, 25 - (25 / fontSize) );

    entry->currPos = 0;

    memset(entry->buffer, 0, MAX_LINE_CHAR);

    WidgetConnect(entry, GUI_TRIGGER_MOUSE_PRESS, EntryWidgetPress, NULL);
    WidgetConnect(entry, GUI_TRIGGER_WIDGET_UNFOCUS, EntryWidgetUnfocus, NULL);

    WidgetConnect(entry, GUI_TRIGGER_ENTRY_CHAR_INPUT, EntryWidgetCharInput, NULL);
    WidgetConnect(entry, GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT, EntryWidgetKeyPressInput, NULL);

}
