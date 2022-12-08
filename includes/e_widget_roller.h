#ifndef E_WIDGET_ROLLER_H
#define E_WIDGET_ROLLER_H

#include "e_widget.h"

typedef struct{
    EWidget widget;
    bool vertical;
    float *source;
    float move_val;
    float stable_val;
} EWidgetRoller;

void RollerWidgetInit(EWidgetRoller *roller, EWidget *parent);
void RollerWidgetSetSource(EWidgetRoller *roller, float *source);

#endif // E_WIDGET_ROLLER_H
