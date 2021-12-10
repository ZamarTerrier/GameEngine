#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "stdinclude.h"

#include "stb_font_consolas_24_latin1.inl"

#include "gameObject.h"

typedef struct{
    localParam local;
    struct{
    stb_fontchar* stbFontData;

    uint32_t fontWidth;
    uint32_t fontHeight;

    unsigned char** font24pixels;
    } font;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
    GraphicItems gItems;
    aboutShader aShader;
    Shape shape;
    vec2 pos;
    vec2 scale;
} TextObject;


TextObject* textInit();

#endif // TEXTOBJECT_H
