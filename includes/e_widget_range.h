#ifndef E_WIDGET_RANGE_H
#define E_WIDGET_RANGE_H

#include "e_widget.h"
#include "e_widget_text.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
void RangeWidgetSetValue(EWidgetRange *range, float val);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_RANGE_H
