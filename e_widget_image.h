#ifndef E_WIDGET_IMAGE_H
#define E_WIDGET_IMAGE_H

#include <gameObject2D.h>

#include <e_widget.h>

typedef struct{
    EWidget widget;
} EWidgetImage;

void ImageWidgetInit(EWidgetImage *img, DrawParam dParam, EWidget *parent);

#endif // E_WIDGET_IMAGE_H