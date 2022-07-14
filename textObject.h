#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "stdinclude.h"

#include "graphicsObject.h"
#include "transform.h"

#include <wchar.h> //"Широкие" многобайтовые символы и их ввод-вывод
#include <wctype.h> //"Классификация" широких символов

typedef struct{
    GraphicsObject graphObj;
    Transform2D transform;
    struct{
        uint32_t fontWidth;
        uint32_t fontHeight;
        int numLetters;
        const char* fontpath;
        vec3 color;
    } font;
} TextObject;


void initTextObject(TextObject* to, const char* fontPath, const char* vertShader, const char* fragShader);

void preparePipeline(TextObject* to);

void addText(const char* text, TextObject* to);

void addTextW(const wchar_t* text, TextObject* to);

void SetFontPath(TextObject* to, const char* path);

void SetTextColor(TextObject* to, vec3 color);

void updateTextUniformBuffer(TextObject* to);

void drawTextObject(TextObject* to);

void createDrawItemsTextObject(TextObject* to);

void cleanTextObject(TextObject* to);

void destroyTextObject(TextObject* to);

#endif // TEXTOBJECT_H
