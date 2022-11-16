#ifndef E_WIDGET_ENTRY_H
#define E_WIDGET_ENTRY_H

#include "e_widget.h"
#include "e_widget_text.h"

typedef struct{
    EWidget widget;
    EWidgetText text;
    uint32_t buffer[MAX_LINE_CHAR];
    int currPos;
    int linePos;
    uint32_t width;
    uint32_t height;
} EWidgetEntry;

void EntryWidgetInit(EWidgetEntry *entry, int fontSize, EWidget* parent);
void EntryUpdateLine();
void EntryWidgetCharacterCallback(void* window, uint32_t codepoint);
void EntryWidgetKeyCallback(void* window,  int key, int scancode, int action, int mods);

#endif // E_WIDGET_ENTRY_H
