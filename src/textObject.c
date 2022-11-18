#include "textObject.h"

#include <vulkan/vulkan.h>

#include "transform.h"

#include "bindDesciption.h"
#include "texture.h"
#include "buffers.h"

#include "camera.h"

#include "gameObject2D.h"

#include "e_resource.h"

#include "stb_truetype.h"

int fontResizer = 7;

void TextImageMakeTexture(GameObject2D *go, TextData *tData, Texture2D *texturePoint){

    tData->font.info = (stbtt_fontinfo *) calloc(1, sizeof(stbtt_fontinfo));
    tData->font.cdata = (stbtt_bakedchar *) calloc(1103, sizeof(stbtt_fontinfo));

    unsigned char ttf_buffer[1<<20];
    unsigned char temp_bitmap[tData->font.fontWidth * tData->font.fontHeight];

    if(strlen(tData->font.fontpath ) > 0)
    {

        FILE *file = fopen(tData->font.fontpath, "rb");

        if(file != NULL)
        {
            fread(ttf_buffer, 1, 1<<20, file);

            fclose(file);

            stbtt_InitFont(tData->font.info , ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
        }else
            stbtt_InitFont(tData->font.info , &_binary_fonts_RobotoBlack_ttf_start, stbtt_GetFontOffsetForIndex(&_binary_fonts_RobotoBlack_ttf_start,0));
    }else
        stbtt_InitFont(tData->font.info , &_binary_fonts_RobotoBlack_ttf_start, stbtt_GetFontOffsetForIndex(&_binary_fonts_RobotoBlack_ttf_start,0));

    //--------------------
    //Создаем буфер вершин для плоскости
    VkDeviceSize bufferSize = tData->font.fontWidth * tData->font.fontHeight; //TEXTOVERLAY_MAX_CHAR_COUNT *  sizeof(Vertex2D) * sizeof(float);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shape.vParam.vertexBuffer, &go->graphObj.shape.vParam.vertexBufferMemory);
    //-------------------
    //Создаем текстуру шрифта

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    if(strlen(tData->font.fontpath) > 0)
        stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, tData->font.fontWidth, tData->font.fontHeight, 0, 1103, tData->font.cdata); // no guarantee this fits!
    else
        stbtt_BakeFontBitmap(&_binary_fonts_RobotoBlack_ttf_start, 0, 32.0, temp_bitmap, tData->font.fontWidth, tData->font.fontHeight, 0, 1103, tData->font.cdata); // no guarantee this fits!

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, temp_bitmap, tData->font.fontWidth * tData->font.fontHeight);
    vkUnmapMemory(device, stagingBufferMemory);

    createImage(tData->font.fontWidth, tData->font.fontHeight, VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texturePoint->textureImage, &texturePoint->textureImageMemory);

    transitionImageLayout(texturePoint->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, texturePoint->textureImage, tData->font.fontWidth, tData->font.fontHeight);
    transitionImageLayout(texturePoint->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);


    texturePoint->textureImageView = createImageView(texturePoint->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    createTextureSampler(texturePoint);

}

void TextObjectMakeLastText(TextObject *to){
    TextObjectSetText(to->textData.text, to);
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
            to->go.graphObj.local.descriptors[i].texture = (Texture2D *) calloc(1, sizeof(Texture2D));
            TextImageMakeTexture(&to->go, &to->textData, to->go.graphObj.local.descriptors[i].texture);
        }
    }

}

void TextObjectAddTexture(TextObject* to){

    to->go.graphObj.local.descrCount ++;

    to->go.graphObj.local.descriptors = (ShaderBuffer *) realloc(to->go.graphObj.local.descriptors, to->go.graphObj.local.descrCount * sizeof(ShaderBuffer));

    to->go.graphObj.local.descriptors[to->go.graphObj.local.descrCount - 1].texture = (Texture2D *) calloc(1, sizeof(Texture2D));
    to->go.graphObj.local.descriptors[to->go.graphObj.local.descrCount - 1].descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    to->go.graphObj.local.descriptors[to->go.graphObj.local.descrCount - 1].size = 1;
    to->go.graphObj.local.descriptors[to->go.graphObj.local.descrCount - 1].stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;

    TextImageMakeTexture(&to->go, &to->textData, to->go.graphObj.local.descriptors[to->go.graphObj.local.descrCount - 1].texture);
}


void TextObjectAddSettingPipeline(TextObject* to, PipelineSetting setting){
    PipelineSetting* settings;

    to->go.graphObj.gItems.settingsCount++;
    to->go.graphObj.gItems.settings = realloc(to->go.graphObj.gItems.settings, to->go.graphObj.gItems.settingsCount * sizeof(PipelineSetting));

    settings = (PipelineSetting *) to->go.graphObj.gItems.settings;

    memcpy(&settings[to->go.graphObj.gItems.settingsCount - 1], &setting, sizeof(PipelineSetting));
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

    vkMapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tbo), 0, &data);
    memcpy(data, &tbo, sizeof(tbo));
    vkUnmapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex]);

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

    int len = strlen(path);

    memcpy( tData->font.fontpath, path, len);
}

void TextDataSetTextColor(TextData* tData, vec3 color){
    tData->font.color = color;
}

void TextDataSetTextSize(TextData* tData, float size){
    tData->font.fontSize = size;
}

void TextImageSetText(const uint32_t* text, GameObject2D* go, TextData *tData){
    int len = 0;

    if(text[len] == 0)
        return;

    while(text[len] != 0)
        len++;

    len++;

    uint32_t buffer[len];

    memcpy(buffer, text, sizeof(uint32_t) * len);

    memset(tData->text, 0, BUFFER_SIZE * BUFFER_SIZE * sizeof(uint32_t));
    memcpy(tData->text, buffer, len * sizeof(uint32_t));

    Vertex2D* mapped = NULL;
    stbtt_aligned_quad q;

    vkMapMemory(device, go->graphObj.shape.vParam.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);
    tData->font.numLetters = 0;

    float  mulX  = tData->font.fontSize / WIDTH / fontResizer;
    float  mulY  = tData->font.fontSize / HEIGHT / fontResizer;

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

    vkUnmapMemory(device, go->graphObj.shape.vParam.vertexBufferMemory);
    mapped = NULL;

    tData->textWidth = x;
    tData->textHeight = y;

}

void TextObjectSetText(const uint32_t* text, TextObject* to)
{
    TextImageSetText(text, &to->go, &to->textData);
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
    GameObject2DCreateDrawItems(to);
    PipelineCreateGraphics(&to->go.graphObj);
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

    GameObject2DInit(to);

    GameObjectSetUpdateFunc(to, (void *)TextObjectUpdateUniformBufferDefault);
    GameObjectSetDrawFunc(to, (void *)TextObjectDrawDefault);
    GameObjectSetRecreateFunc(to, (void *)TextObjectRecreate);

    Transform2DSetPosition(to, 0.0f, 0.0f);

    //Загружаем шрифт и настраеваем его на работу
    TextDataInit(&to->textData, fontSize, fontPath);

    //----------------------------------

    BuffersAddUniformObject(&to->go.graphObj.local, sizeof(TransformBuffer2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    TextObjectAddTexture(to);

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

    TextObjectAddSettingPipeline(to, setting);

    GameObject2DCreateDrawItems(to);
    PipelineCreateGraphics(&to->go.graphObj);
}
