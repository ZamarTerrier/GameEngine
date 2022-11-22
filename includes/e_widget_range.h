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
    float *dest;
} EWidgetRange;

void RangeWidgetInit(EWidgetRange *range, float size_x, float size_y, float min, float max, EWidget *parent);
void RangeWidgetSetValueDestin(EWidgetRange *range, float *val_dest);

#endif // E_WIDGET_RANGE_H
