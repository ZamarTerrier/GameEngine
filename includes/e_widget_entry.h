#ifndef E_WIDGET_ENTRY_H
#define E_WIDGET_ENTRY_H

#include "e_widget.h"
#include "e_widget_text.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidget widget;
    EWidgetText text;
    uint32_t num_texts;
    uint32_t curr_texts;
    char **buffers;
    uint32_t num_lines;
    uint32_t curr_line;
    int currPos;
    float width;
    float height;
} EWidgetEntry;

extern bool e_ctrl_press, e_c_press, e_v_press, e_pasted;

int EntryWidgetDestroyDefault(EWidgetEntry *entry);

void EntryWidgetInit(EWidgetEntry *entry, int fontSize, EWidget* parent);
void EntryUpdateLine();
void EntryWidgetCharacterCallback(void* window, uint32_t codepoint);
void EntryWidgetKeyCallback(void* window,  int key, int scancode, int action, int mods);
char *EntryWidgetGetText(EWidgetEntry *entry);
void EntryWidgetSetText(EWidgetEntry *entry, char *text);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_ENTRY_H
