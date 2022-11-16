
#include "e_widget_entry_area.h"

#include <GLFW/glfw3.h>

#include "e_resource.h"

void EntryAreaWidgetKeyPressInput(EWidget* widget, int key, void *arg){

    EWidgetEntry *temp = widget;

    if(key == GLFW_KEY_ENTER)
    {
        temp->linePos = 0;
        temp->buffer[temp->currPos] = '\n';
        temp->currPos++;

        if(temp->currPos > MAX_LINE_CHAR)
            temp->currPos = MAX_LINE_CHAR;
    }

    TextWidgetSetText(&temp->text, temp->buffer);
}

void EntryAreaWidgetInit(EWidgetEntryArea *entry, int fontSize, EWidget* parent){

    EntryWidgetInit(entry, fontSize, parent);

    WidgetConnect(entry, GUI_TRIGGER_ENTRY_KEY_PRESS_INPUT, EntryAreaWidgetKeyPressInput, NULL);

}
