#include "textObject.h"

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
            stbtt_InitFont(tData->font.info , &_binary_fonts_arial_ttf_start, stbtt_GetFontOffsetForIndex(&_binary_fonts_arial_ttf_start,0));
    }else
        stbtt_InitFont(tData->font.info , &_binary_fonts_arial_ttf_start, stbtt_GetFontOffsetForIndex(&_binary_fonts_arial_ttf_start,0));

    //--------------------
    //Создаем буфер вершин для плоскости
    VkDeviceSize bufferSize = tData->font.fontWidth * tData->font.fontHeight; //TEXTOVERLAY_MAX_CHAR_COUNT *  sizeof(Vertex2D) * sizeof(float);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shape.vParam.vertexBuffer, &go->graphObj.shape.vParam.vertexBufferMemory);
    //-------------------
    //Создаем текстуру шрифта

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    if(strlen(tData->font.fontpath) > 0)
        stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, tData->font.fontWidth, tData->font.fontHeight, 0, 1103, tData->font.cdata); // no guarantee this fits!
    else
        stbtt_BakeFontBitmap(&_binary_fonts_arial_ttf_start, 0, 32.0, temp_bitmap, tData->font.fontWidth, tData->font.fontHeight, 0, 1103, tData->font.cdata); // no guarantee this fits!

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
    if(strlen(to->textData.text) > 0){
        TextObjectSetText(to->textData.text, to);
    }
    else
        TextObjectSetTextW(to->textData.wText, to);
}

void TextObjectRecreateUniform(TextObject *to){
    int count = to->go.graphObj.local.descrCount;

    for(int i=0;i < count;i++){
        if(to->go.graphObj.local.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
            to->go.graphObj.local.descriptors[i].uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
            to->go.graphObj.local.descriptors[i].uniform->size = to->go.graphObj.local.descriptors[i].buffsize;
            createUniformBuffer(to->go.graphObj.local.descriptors[i].uniform, i);
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

void TextImageSetText(const char* text, GameObject2D* go, TextData *tData){
    int len = strlen(text);

    char buffer[len];

    memcpy(buffer, text, sizeof(char) * len);

    memset(tData->wText, 0, BUFFER_SIZE * BUFFER_SIZE);
    memset(tData->text, 0, BUFFER_SIZE * BUFFER_SIZE);
    memcpy(tData->text, buffer, len);

    Vertex2D* mapped = NULL;
    stbtt_aligned_quad q;

    vkMapMemory(device, go->graphObj.shape.vParam.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);
    tData->font.numLetters = 0;

    float  mulX  = tData->font.fontSize / WIDTH / fontResizer;
    float  mulY  = tData->font.fontSize / HEIGHT / fontResizer;

    float x = 0.0f;
    float y = 0.0f;

    char *tempI = tData->text;

    // Generate a uv mapped quad per char in the new text
    for (i=0;i < len;i++)
    {
        if(*tempI == '\n')
        {
            x = 0;
            y += (mulY * 20000);
            ++tempI;
            continue;
        }

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
}

void TextImageSetTextW(const wchar_t* text, GameObject2D* go, TextData *tData){
    int len = wcslen(text);

    wchar_t buffer[len];

    memcpy(buffer, text, sizeof(wchar_t) * len);

    memset(tData->text, 0, BUFFER_SIZE * BUFFER_SIZE);
    memset(tData->wText, 0, BUFFER_SIZE * BUFFER_SIZE * sizeof(wchar_t));
    memcpy(tData->wText, buffer, len * sizeof(wchar_t));

    //русские буквы в пределах 1040-1103

    Vertex2D* mapped = NULL;
    stbtt_aligned_quad q;

    vkMapMemory(device, go->graphObj.shape.vParam.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);
    tData->font.numLetters = 0;

    float  mulX  = tData->font.fontSize / WIDTH / fontResizer;
    float  mulY  = tData->font.fontSize / HEIGHT / fontResizer;

    float x = 0.0f;
    float y = 0.0f;


    // Generate a uv mapped quad per char in the new text
    for (i=0;i<len;i++)
    {
        if(tData->wText[i] == '\n')
        {
            x = 0;
            y += (mulY * 13000);
            continue;
        }

        stbtt_GetBakedQuad(tData->font.cdata, 512,512, tData->wText[i], &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9

        mapped->position.x = (float)q.x0 * mulX;
        mapped->position.y = (float)q.y0 * mulY;
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
        mapped->color =tData->font.color;
        mapped->texCoord.x = q.s1;
        mapped->texCoord.y = q.t1;
        mapped++;

        tData->font.numLetters++;

    }

    vkUnmapMemory(device, go->graphObj.shape.vParam.vertexBufferMemory);
    mapped = NULL;
}

void TextObjectSetText(const char* text, TextObject* to)
{

    TextImageSetText(text, &to->go, &to->textData);

}

void TextObjectSetTextW(const wchar_t* text, TextObject* to)
{
    TextImageSetTextW(text, &to->go, &to->textData);
}

void TextObjectRecreate(TextObject* to){
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

void TextObjectInit(TextObject* to, int fontSize, const char* fontPath, const char* vertShader, const char* fragShader)
{

    GameObject2DInit(to);

    GameObjectSetUpdateFunc(to, (void *)TextObjectUpdateUniformBufferDefault);
    GameObjectSetDrawFunc(to, (void *)TextObjectDrawDefault);
    GameObjectSetRecreateFunc(to, (void *)TextObjectRecreate);

    Transform2DSetPosition(to, (vec2){0.0f, 0.0f});

    //Загружаем шрифт и настраеваем его на работу

    GraphicsObjectSetShadersPath(&to->go.graphObj, vertShader, fragShader);

    TextDataInit(&to->textData, fontSize, fontPath);

    //----------------------------------

    addUniformObject(&to->go.graphObj.local, sizeof(TransformBuffer2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    TextObjectAddTexture(to);

    PipelineSetting setting;
    setting.poligonMode = VK_POLYGON_MODE_FILL;
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    setting.vertShader = to->go.graphObj.aShader.vertShader;
    setting.fragShader = to->go.graphObj.aShader.fragShader;
    setting.drawType = 0;
    setting.scissor.offset.x = 0;
    setting.scissor.offset.y = 0;
    setting.scissor.extent = swapChainExtent;
    setting.viewport.x = 0.0f;
    setting.viewport.y = 0.0f;
    setting.viewport.width = (float) swapChainExtent.width;
    setting.viewport.height = (float) swapChainExtent.height;
    setting.viewport.minDepth = 0.0f;
    setting.viewport.maxDepth = 1.0f;
    TextObjectAddSettingPipeline(to, setting);

    GameObject2DCreateDrawItems(to);
    PipelineCreateGraphics(&to->go.graphObj);
}
