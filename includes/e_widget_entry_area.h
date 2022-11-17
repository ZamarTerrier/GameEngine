#ifndef E_WIDGET_ENTRY_AREA_H
#define E_WIDGET_ENTRY_AREA_H

#include "e_widget_entry.h"

typedef struct{
    EWidgetEntry entry;
    uint32_t lineNum;
} EWidgetEntryArea;

void EntryAreaWidgetInit(EWidgetEntryArea *entry, int fontSize, EWidget* parent);
void EntryAreaUpdateLine();

#endif // E_WIDGET_ENTRY_AREA_H
