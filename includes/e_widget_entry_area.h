#ifndef E_WIDGET_ENTRY_AREA_H
#define E_WIDGET_ENTRY_AREA_H

#include "e_widget_entry.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidgetEntry entry;
    float textHeight;
} EWidgetEntryArea;

void EntryAreaWidgetInit(EWidgetEntryArea *entry, int fontSize, DrawParam *dParam, EWidget* parent);
void EntryAreaUpdateLine();
void EntryAreaWidgetInsertText(EWidgetEntryArea *area, const char *src);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_ENTRY_AREA_H
