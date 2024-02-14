#ifndef E_WIDGET_TEXT_H
#define E_WIDGET_TEXT_H

#include "Variabels/engine_includes.h"

#include "Objects/textObject.h"
#include "Objects/gameObject2D.h"

#include "e_widget.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidget widget;
    TextData tData;
} EWidgetText;

void TextWidgetInitDefault(EWidgetText *wt, int fontSize, DrawParam *dParam, EWidget* parent);
void TextWidgetInit(EWidgetText *wt, int fontSize, DrawParam *dParam, EWidget* parent);

void TextWidgetAddDefault(EWidgetText *wt, void *render);

void TextWidgetSetColor(EWidgetText* wt, vec3 color);
void TextWidgetSetText(EWidgetText* wt, const char* text);
void TextWidgetGetText(EWidgetText* wt, char* dest);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_TEXT_H
