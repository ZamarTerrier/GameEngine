#include "textObject.h"

#include "texture.h"

#include "buffers.h"

#define TEXTOVERLAY_MAX_CHAR_COUNT 2048


TextObject* textInit()
{

    TextObject* to = (TextObject *)calloc(1, sizeof(TextObject));

    to->font.fontWidth = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
    to->font.fontHeight = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;

    to->font.stbFontData = (stb_fontchar *) calloc(STB_FONT_consolas_24_latin1_NUM_CHARS, sizeof(stb_fontchar));

    to->font.font24pixels = (unsigned char**) calloc(to->font.fontWidth, sizeof(unsigned char*));

    for(int i =0;i < to->font.fontWidth;i++)
        to->font.font24pixels[i] = (unsigned char*) calloc(to->font.fontHeight, sizeof(unsigned char));

    stb_font_consolas_24_latin1(to->font.stbFontData, to->font.font24pixels, to->font.fontHeight);

    createVertexBuffer(&to->shape.vertex);
    createIndexBuffer(&to->shape.index);

    to->aShader.vertShader = "/home/ilia/Projects/Graphics/shaders/text/vert.spv";
    to->aShader.fragShader = "/home/ilia/Projects/Graphics/shaders/text/frag.spv";

    to->local.textures = (Texture2D *) calloc(1, sizeof(Texture2D));

    createImage(to->font->fontWidth, to->font->fontHeight, VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &to->local.textures[0].textureImage, &to->local.textures[0].textureImageMemory);

    to->local.textures[0].textureImageView = createImageView(to->local.textures[0].textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    createTextureSampler(&to->local.textures[0]);

    to->local.texturesCount = 1;

    addUniformObject(to->local, sizeof(ViewUniformObject));
    addUniformObject(to->local, sizeof(UniformBufferObject));
    addUniformObject(to->local, sizeof(ImgUniformParam));

    createDrawningParams(to->go);

    return to;

}
