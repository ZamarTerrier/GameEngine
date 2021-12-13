#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "stdinclude.h"

#include "graphicsObject.h"
#include "transform.h"

#include "stb_font_consolas_24_latin1.inl"

typedef struct{
    struct{
        stb_fontchar stbFontData[STB_FONT_consolas_24_latin1_NUM_CHARS];
        uint32_t fontWidth;
        uint32_t fontHeight;
        int numLetters;
    } font;
    GraphicsObject graphObj;
    Transform transform;
} TextObject;


void initTextObject(TextObject* to);

void preparePipeline(TextObject* to);

void addText(const char* text, float x, float y, TextObject* to);

void updateTextUniformBuffer(TextObject* to);

void drawTextObject(TextObject* to);

void destroyTextObject(TextObject* to);

#endif // TEXTOBJECT_H
