#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "engine_includes.h"

#include "gameObject.h"
#include "gameObject2D.h"
#include "graphicsObject.h"
#include "transform.h"

#include <wchar.h> //"Широкие" многобайтовые символы и их ввод-вывод
#include <wctype.h> //"Классификация" широких символов


#define STB_TRUETYPE_IMPLEMENTATION
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048
#define BUFFER_SIZE 2048

typedef struct{
    struct{
        uint32_t fontWidth;
        uint32_t fontHeight;
        void *cdata;
        void *info;
        float fontSize;
        int numLetters;
        const char fontpath[256];
        vec3 color;
    } font;
    uint32_t text[BUFFER_SIZE * BUFFER_SIZE];
} TextData;

typedef struct{
    GameObject2D go;
    TextData textData;
} TextObject;

void TextImageMakeTexture(GameObject2D *go, TextData *tData, Texture2D *texturePoint);

void TextObjectMakeLastText(TextObject *to);

void TextDataInit(TextData *tData, int fontSize, char* fontPath);
void TextObjectInit(TextObject* to, int fontSize,  const char* fontPath);

void TextDataSetFontPath(TextData* tData, const char* path);
void TextDataSetTextColor(TextData* tData, vec3 color);
void TextDataSetTextSize(TextData* tData, float size);

void TextImageSetText(const uint32_t* text, GameObject2D* go, TextData *tData);

void TextObjectSetText(const uint32_t* text, TextObject* to);

#endif // TEXTOBJECT_H
