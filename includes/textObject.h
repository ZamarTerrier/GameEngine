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

#ifdef __cplusplus
extern "C"
{
#endif

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
    float textWidth;
    float textHeight;
    uint32_t text[BUFFER_SIZE];
} TextData;

typedef struct{
    GameObject2D go;
    TextData textData;
} TextObject;

void TextImageMakeTexture(GameObject2D *go, TextData *tData, ShaderBuffer *descriptor);

void TextObjectMakeLastText(TextObject *to);

void TextDataInit(TextData *tData, int fontSize, char* fontPath);
void TextObjectInit(TextObject* to, int fontSize,  const char* fontPath);

void TextDataSetFontPath(TextData* tData, const char* path);
void TextDataSetTextColor(TextData* tData, vec3 color);
void TextDataSetTextSize(TextData* tData, float size);

void TextImageSetText(const uint32_t* text, GameObject2D* go, TextData *tData);

void TextObjectSetTextU32(TextObject* to, const uint32_t* text);
void TextObjectSetTextU8(TextObject* to, const char* text);

#ifdef __cplusplus
}
#endif

#define TextObjectSetText(arg1, arg2)\
    _Generic((arg2),\
    char *: TextObjectSetTextU8,\
    unsigned int *: TextObjectSetTextU32\
    )(arg1, arg2)

#endif // TEXTOBJECT_H
