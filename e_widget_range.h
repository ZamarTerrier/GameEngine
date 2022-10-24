#ifndef E_WIDGET_RANGE_H
#define E_WIDGET_RANGE_H

#include "e_widget.h"
#include "e_widget_text.h"

typedef struct{
    EWidget widget;
    EWidget range;
    EWidget line;
    float min;
    float max;
} EWidgetRange;

void RangeWidgetInit(EWidgetRange *range, vec2 size, int min, int max, EWidget *parent);

#endif // E_WIDGET_RANGE_H
