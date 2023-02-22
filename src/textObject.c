#include "textObject.h"

#include <vulkan/vulkan.h>

#include "transform.h"

#include "bindDesciption.h"
#include "texture.h"
#include "buffers.h"

#include "camera.h"

#include "gameObject2D.h"

#include "stb_truetype.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

int fontResizer = 7;

FontCache *TextFontFind(char *path)
{
    if(path == NULL)
        return NULL;

    FontCache *buff_fonts = e_var_fonts;

    for(int i=0;i < e_var_num_fonts;i++)
    {
        if(strstr(buff_fonts[i].path, path))
            return &buff_fonts[i];
    }

    return NULL;
}

void TextImageMakeTexture(GameObject2D *go, TextData *tData, ShaderBuffer *descriptor){

    if(e_var_num_fonts + 1 >= MAX_FONTS)
    {
        printf("Превышен лимит по созданию Шрифтов!\n");
        return;
    }

    FontCache *temp_tex;

    FontCache *fonts = e_var_fonts;

    VkDeviceSize bufferSize, vertBufferSize;

    bufferSize = tData->font.fontWidth * tData->font.fontHeight; //TEXTOVERLAY_MAX_CHAR_COUNT *  sizeof(Vertex2D) * sizeof(float);

    vertBufferSize = BUFFER_SIZE * 4 * sizeof(Vertex2D);

    char *some_file_path = tData->font.fontpath;
    int font_len = strlen(some_file_path);

    char name_font[] = "define_font";

    if(font_len > 0)
    {
        temp_tex = TextFontFind(tData->font.fontpath);

        if(temp_tex != NULL)
        {
            descriptor->texture = temp_tex->texture;
            tData->font.info = temp_tex->info;
            tData->font.cdata = temp_tex->cdata;

            //--------------------
            //Создаем буфер вершин для плоскости
            BuffersCreate(vertBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shape.vParam.vertexBuffer, &go->graphObj.shape.vParam.vertexBufferMemory);

            return;
        }

        int len = strlen(tData->font.fontpath);
        memset(fonts[e_var_num_fonts].path, 0, 2048);
        memcpy(fonts[e_var_num_fonts].path, tData->font.fontpath, len);

    }
    else if(define_font_loaded)
    {
        temp_tex = TextFontFind(name_font);

        if(temp_tex != NULL)
        {
            descriptor->texture = temp_tex->texture;
            tData->font.info = temp_tex->info;
            tData->font.cdata = temp_tex->cdata;

            //--------------------
            //Создаем буфер вершин для плоскости
            BuffersCreate(vertBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shape.vParam.vertexBuffer, &go->graphObj.shape.vParam.vertexBufferMemory);
            return;
        }else
            exit(1);

    }else{

        int len = strlen(name_font);
        memset(fonts[e_var_num_fonts].path, 0, 2048);
        memcpy(fonts[e_var_num_fonts].path, name_font, len);

        define_font_loaded = 1;
    }

    fonts[e_var_num_fonts].info = (stbtt_fontinfo *) calloc(1, sizeof(stbtt_fontinfo));
    fonts[e_var_num_fonts].cdata = (stbtt_bakedchar *) calloc(1106, sizeof(stbtt_fontinfo));

    tData->font.cdata = fonts[e_var_num_fonts].cdata;
    tData->font.info = fonts[e_var_num_fonts].info;

    unsigned char ttf_buffer[1<<20];
    unsigned char temp_bitmap[tData->font.fontWidth * tData->font.fontHeight];

    //-------------------
    //Создаем текстуру шрифта
    if(font_len > 0)
    {

        FILE *file = fopen(tData->font.fontpath, "rb");

        if(file != NULL)
        {
            fread(ttf_buffer, 1, 1<<20, file);

            fclose(file);

            stbtt_InitFont(fonts[e_var_num_fonts].info , ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
            stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, tData->font.fontWidth, tData->font.fontHeight, 0, 1106, fonts[e_var_num_fonts].cdata); // no guarantee this fits!
        }else
        {
            stbtt_InitFont(fonts[e_var_num_fonts].info , &_binary_fonts_RobotoBlack_ttf_start, stbtt_GetFontOffsetForIndex(&_binary_fonts_RobotoBlack_ttf_start,0));
            stbtt_BakeFontBitmap(&_binary_fonts_RobotoBlack_ttf_start, 0, 32.0, temp_bitmap, tData->font.fontWidth, tData->font.fontHeight, 0, 1106, fonts[e_var_num_fonts].cdata); // no guarantee this fits!
        }
    }else{
        stbtt_InitFont(fonts[e_var_num_fonts].info , &_binary_fonts_RobotoBlack_ttf_start, stbtt_GetFontOffsetForIndex(&_binary_fonts_RobotoBlack_ttf_start,0));
        stbtt_BakeFontBitmap(&_binary_fonts_RobotoBlack_ttf_start, 0, 32.0, temp_bitmap, tData->font.fontWidth, tData->font.fontHeight, 0, 1106, fonts[e_var_num_fonts].cdata); // no guarantee this fits!
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    //--------------------
    //Создаем буфер вершин для плоскости
    BuffersCreate(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shape.vParam.vertexBuffer, &go->graphObj.shape.vParam.vertexBufferMemory);

    void* data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, temp_bitmap, tData->font.fontWidth * tData->font.fontHeight);
    vkUnmapMemory(e_device, stagingBufferMemory);

    fonts[e_var_num_fonts].texture = calloc(1, sizeof(Texture2D));

    Texture2D *texture = fonts[e_var_num_fonts].texture;

    createImage(tData->font.fontWidth, tData->font.fontHeight, VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->textureImage, &texture->textureImageMemory);

    transitionImageLayout(texture->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, texture->textureImage, tData->font.fontWidth, tData->font.fontHeight);
    transitionImageLayout(texture->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);

    texture->textureImageView = createImageView(texture->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    createTextureSampler(fonts[e_var_num_fonts].texture);

    descriptor->texture = fonts[e_var_num_fonts].texture;

    e_var_num_fonts ++;

}

void TextObjectMakeLastText(TextObject *to){
    TextImageSetText(to->textData.text, to, &to->textData);
}

void TextObjectRecreateUniform(TextObject *to){
    int count = to->go.graphObj.local.descrCount;

    for(int i=0;i < count;i++){
        if(to->go.graphObj.local.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
            to->go.graphObj.local.descriptors[i].uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
            to->go.graphObj.local.descriptors[i].uniform->size = to->go.graphObj.local.descriptors[i].buffsize;
            BuffersCreateUniform(to->go.graphObj.local.descriptors[i].uniform, i);
        }
        else
        {
            //TextImageMakeTexture(&to->go, &to->textData, &to->go.graphObj.local.descriptors[i]);
        }
    }

}

void TextObjectAddTexture(TextObject* to){

    if(to->go.graphObj.local.descrCount + 1 > MAX_UNIFORMS)
        return;

    ShaderBuffer *descriptor = &to->go.graphObj.local.descriptors[to->go.graphObj.local.descrCount];

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->image = NULL;

    TextImageMakeTexture(&to->go, &to->textData, descriptor);

    to->go.graphObj.local.descrCount ++;
}

void TextObjectUpdateUniformBufferDefault(TextObject* to) {

    if(to->go.graphObj.local.descriptors == NULL)
        return;

    void* data;

    Camera2D* cam = (Camera2D*) cam2D;

    ShaderBuffer* sBuffer = to->go.graphObj.local.descriptors;

    TransformBuffer2D tbo;

    tbo.position = to->go.transform.position;
    tbo.rotation = to->go.transform.rotation;
    tbo.scale = to->go.transform.scale;

    vkMapMemory(e_device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tbo), 0, &data);
    memcpy(data, &tbo, sizeof(tbo));
    vkUnmapMemory(e_device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex]);

}

void TextObjectDrawDefault(TextObject* to)
{
    for(int i=0; i < to->go.graphObj.gItems.pipelineCount; i++){
        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, to->go.graphObj.gItems.graphicsPipeline[i]);
        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, to->go.graphObj.gItems.pipelineLayout[i], 0, 1, &to->go.graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

        PipelineSetting *settings = &to->go.graphObj.gItems.settings[i];

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, &to->go.graphObj.shape.vParam.vertexBuffer, &offsets);
        for (uint32_t j = 0; j < to->textData.font.numLetters; j++)
        {
            vkCmdDraw(commandBuffers[imageIndex], 4, 1, j * 4, 0);
        }
    }
}

void TextDataSetFontPath(TextData* tData, const char* path){

    if(path == NULL)
        return;

    int len = strlen(path);

    memcpy( tData->font.fontpath, path, len);
}

void TextDataSetTextColor(TextData* tData, vec3 color){
    tData->font.color = color;
}

void TextDataSetTextSize(TextData* tData, float size){
    tData->font.fontSize = size;
}

void TextImageClearText(GameObject2D* go, TextData *tData)
{
    memset(tData->text, 0, sizeof(uint32_t) * BUFFER_SIZE);

    Vertex2D* mapped = NULL;

    vkMapMemory(e_device, go->graphObj.shape.vParam.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);

    memset(mapped, 0, BUFFER_SIZE * 4 * sizeof(Vertex2D));

    vkUnmapMemory(e_device, go->graphObj.shape.vParam.vertexBufferMemory);
    mapped = NULL;
}

void TextImageSetText(const uint32_t* text, GameObject2D* go, TextData *tData){
    int len = 0;

    if(text[len] == 0)
    {
        TextImageClearText(go, tData);
        return;
    }

    len = ToolsStr32BitLength(text);

    if(len >= BUFFER_SIZE)
        return;

    len ++;

    uint32_t buffer[len];

    memcpy(buffer, text, sizeof(uint32_t) * len);

    memset(tData->text, 0, BUFFER_SIZE * sizeof(uint32_t));
    memcpy(tData->text, buffer, len * sizeof(uint32_t));

    Vertex2D* mapped = NULL;
    stbtt_aligned_quad q;

    vkMapMemory(e_device, go->graphObj.shape.vParam.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);
    tData->font.numLetters = 0;

    float mulX = tData->font.fontSize / WIDTH / fontResizer;
    float mulY = tData->font.fontSize / HEIGHT / fontResizer;

    float x = 0.0f;
    float y = 0.0f;

    uint32_t *tempI = tData->text;

    // Generate a uv mapped quad per char in the new text
    for (int i=0;i < len;i++)
    {
        /*if(*tempI == '\n')
        {
            x = 0;
            y += (mulY * HEIGHT * 20);
            ++tempI;
            continue;
        }*/

        stbtt_GetBakedQuad(tData->font.cdata, 512,512, *tempI, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9

        mapped->position.x = (float)q.x0 * mulX;
        mapped->position.y =  (float)q.y0 * mulY;
        mapped->color = tData->font.color;
        mapped->texCoord.x = q.s0;
        mapped->texCoord.y = q.t0;
        mapped++;

        mapped->position.x = (float)q.x1 * mulX;
        mapped->position.y = (float)q.y0 * mulY;
        mapped->color = tData->font.color;
        mapped->texCoord.x = q.s1;
        mapped->texCoord.y = q.t0;
        mapped++;

        mapped->position.x = (float)q.x0 * mulX;
        mapped->position.y = (float)q.y1 * mulY;
        mapped->color = tData->font.color;
        mapped->texCoord.x = q.s0;
        mapped->texCoord.y = q.t1;
        mapped++;

        mapped->position.x = (float)q.x1 * mulX;
        mapped->position.y = (float)q.y1 * mulY;
        mapped->color = tData->font.color;
        mapped->texCoord.x = q.s1;
        mapped->texCoord.y = q.t1;
        mapped++;

        tData->font.numLetters++;
        ++tempI;
    }

    vkUnmapMemory(e_device, go->graphObj.shape.vParam.vertexBufferMemory);
    mapped = NULL;

    tData->textWidth = x;
    tData->textHeight = y;

}

void TextObjectSetTextU32(TextObject* to, const uint32_t* text)
{
    TextImageSetText(text, &to->go, &to->textData);
}

void TextObjectSetTextU8(TextObject* to, const char* text)
{
    uint32_t size = strlen(text);
    uint32_t buff[size + 1];

    ToolsStringToUInt32(buff, text);

    TextImageSetText(buff, &to->go, &to->textData);
}

void TextObjectRecreate(TextObject* to){

    PipelineSetting *settings = (PipelineSetting *)to->go.graphObj.gItems.settings;

    for(int i=0; i < to->go.graphObj.gItems.settingsCount;i++)
    {
        settings[i].scissor.offset.x = 0;
        settings[i].scissor.offset.y = 0;
        settings[i].scissor.extent.height = HEIGHT;
        settings[i].scissor.extent.width = WIDTH;
        settings[i].viewport.x = 0;
        settings[i].viewport.y = 0;
        settings[i].viewport.height = HEIGHT;
        settings[i].viewport.width = WIDTH;
    }

    TextObjectRecreateUniform(to);
    GraphicsObjectCreateDrawItems(&to->go.graphObj);
    PipelineCreateGraphics(&to->go.graphObj);
    Transform2DReposition(to);
    TextObjectMakeLastText(to);
}

void TextDataInit(TextData *tData, int fontSize, char* fontPath){

    tData->font.fontWidth = 512;
    tData->font.fontHeight = 512;
    tData->font.fontSize = fontSize;

    TextDataSetFontPath(tData, fontPath);
}

void TextObjectInit(TextObject* to, int fontSize, const char* fontPath)
{
    memcpy(to->go.name, "Object_Text", 10);

    GameObject2DInit(to);

    GameObjectSetUpdateFunc(to, (void *)TextObjectUpdateUniformBufferDefault);
    GameObjectSetDrawFunc(to, (void *)TextObjectDrawDefault);
    GameObjectSetRecreateFunc(to, (void *)TextObjectRecreate);

    //Загружаем шрифт и настраеваем его на работу
    TextDataInit(&to->textData, fontSize, fontPath);

    //----------------------------------

    BuffersAddUniformObject(&to->go.graphObj.local, sizeof(TransformBuffer2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    TextObjectAddTexture(to);

    GraphicsObjectCreateDrawItems(&to->go.graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_text_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_text_vert_spv_size);
        setting.fragShader = &_binary_shaders_text_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_text_frag_spv_size);
        setting.fromFile = 0;
    }

    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    GameObject2DAddSettingPipeline(to, &setting);

    PipelineCreateGraphics(&to->go.graphObj);
}
