#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "stdinclude.h"

#include "graphicsObject.h"
#include "transform.h"

#include <wchar.h> //"Широкие" многобайтовые символы и их ввод-вывод
#include <wctype.h> //"Классификация" широких символов

typedef struct{
    struct{
        uint32_t fontWidth;
        uint32_t fontHeight;
        int numLetters;
    } font;
    GraphicsObject graphObj;
    Transform transform;
} TextObject;


void initTextObject(TextObject* to);

void preparePipeline(TextObject* to);

void addText(const char* text, TextObject* to);

void addTextW(const wchar_t* text, TextObject* to);

void updateTextUniformBuffer(TextObject* to);

void drawTextObject(TextObject* to);

void destroyTextObject(TextObject* to);

#endif // TEXTOBJECT_H
