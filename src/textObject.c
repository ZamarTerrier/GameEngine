#include "textObject.h"

#include <vulkan/vulkan.h>

#include "e_transform.h"

#include "e_descriptor.h"
#include "e_texture.h"
#include "e_buffer.h"

#include "e_camera.h"

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

void TextImageMakeTexture(GameObject2D *go, TextData *tData, BluePrintDescriptor *descriptor){

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

    descriptor->textures = calloc(1, sizeof(Texture2D *));

    if(font_len > 0)
    {
        temp_tex = TextFontFind(tData->font.fontpath);

        if(temp_tex != NULL)
        {
            descriptor->textures[0] = temp_tex->texture;
            tData->font.info = temp_tex->info;
            tData->font.cdata = temp_tex->cdata;

            //--------------------
            //Создаем буфер вершин для плоскости
            BuffersCreate(vertBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shapes[0].vParam.vertexBuffer, &go->graphObj.shapes[0].vParam.vertexBufferMemory, ENGINE_BUFFER_ALLOCATE_VERTEX);

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
            descriptor->textures[0] = temp_tex->texture;
            tData->font.info = temp_tex->info;
            tData->font.cdata = temp_tex->cdata;

            //--------------------
            //Создаем буфер вершин для плоскости
            BuffersCreate(vertBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shapes[0].vParam.vertexBuffer, &go->graphObj.shapes[0].vParam.vertexBufferMemory, ENGINE_BUFFER_ALLOCATE_VERTEX);
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

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory, ENGINE_BUFFER_ALLOCATE_STAGING);

    //--------------------
    //Создаем буфер вершин для плоскости
    BuffersCreate(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &go->graphObj.shapes[0].vParam.vertexBuffer, &go->graphObj.shapes[0].vParam.vertexBufferMemory, ENGINE_BUFFER_ALLOCATE_VERTEX);

    void* data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, temp_bitmap, tData->font.fontWidth * tData->font.fontHeight);
    vkUnmapMemory(e_device, stagingBufferMemory);

    fonts[e_var_num_fonts].texture = calloc(1, sizeof(Texture2D));

    Texture2D *texture = fonts[e_var_num_fonts].texture;

    TextureCreateImage(tData->font.fontWidth, tData->font.fontHeight, 1,VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0, &texture->textureImage, &texture->textureImageMemory);

    ToolsTransitionImageLayout(texture->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    ToolsCopyBufferToImage(stagingBuffer, texture->textureImage, tData->font.fontWidth, tData->font.fontHeight);
    ToolsTransitionImageLayout(texture->textureImage, VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

    BuffersDestroyBuffer(stagingBuffer);

    texture->textureImageView = TextureCreateImageView(texture->textureImage, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, 1);

    TextureCreateSampler(&texture->textureSampler, texture->textureType, 1);

    descriptor->textures[0] = fonts[e_var_num_fonts].texture;

    e_var_num_fonts ++;

}

void TextObjectMakeLastText(TextObject *to){
    TextImageSetText(to->textData.text, to, &to->textData);
}

//Не корректно
void TextObjectRecreateUniform(TextObject *to){
    /*
    int count = to->go.graphObj.blueprints.count;

    for(int i=0;i < count;i++){
        if(to->go.graphObj.blueprints.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
            to->go.graphObj.blueprints.descriptors[i].uniform.size = to->go.graphObj.blueprints.descriptors[i].buffsize;
            BuffersCreateUniform(&to->go.graphObj.blueprints.descriptors[i].uniform);
        }
        else
        {
            //TextImageMakeTexture(&to->go, &to->textData, &to->go.graphObj.local.descriptors[i]);
        }
    }*/

}

void TextObjectAddTexture(TextObject* to, uint32_t indx_pack){

    BluePrintPack *pack = &to->go.graphObj.blueprints.blue_print_packs[indx_pack];

    if(pack->num_descriptors + 1 > MAX_UNIFORMS)
        return;

    BluePrintDescriptor *descriptor = &pack->descriptors[pack->num_descriptors];

    descriptor->descrType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor->descrCount = 1;
    descriptor->size = 1;
    descriptor->stageflag = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptor->image = NULL;
    descriptor->flags = ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE;

    TextImageMakeTexture(&to->go, &to->textData, descriptor);

    pack->num_descriptors ++;
}

void TextObjectTransformBufferUpdate(GameObject2D *go, BluePrintDescriptor *descriptor) {

    TransformBuffer2D tbo;

    tbo.position = go->transform.position;
    tbo.rotation = go->transform.rotation;
    tbo.scale = go->transform.scale;

    DescriptorUpdate(descriptor, &tbo, sizeof(tbo));
}

void TextObjectDrawDefault(TextObject* to, void *command)
{
    for(int i=0; i < to->go.graphObj.gItems.num_shader_packs;i++)
    {
        BluePrintPack *pack = &to->go.graphObj.blueprints.blue_print_packs[i];

        if(pack->render_point == current_render)
        {
            ShaderPack *pack = &to->go.graphObj.gItems.shader_packs[i];

            for(int j=0; j < pack->num_pipelines; j++){
                vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].pipeline);

                PipelineSetting *settings = &to->go.graphObj.blueprints.blue_print_packs[i].settings[j];
                vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].layout, 0, 1, &pack->descriptor.descr_sets[imageIndex], 0, NULL);

                if(settings->flags & ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW){
                    vkCmdSetViewport(command, 0, 1, &settings->viewport);
                    vkCmdSetScissor(command, 0, 1, &settings->scissor);
                }

                VkDeviceSize offsets = 0;
                vkCmdBindVertexBuffers(command, 0, 1, &to->go.graphObj.shapes[settings->vert_indx].vParam.vertexBuffer, &offsets);

                for (uint32_t l = 0; l < to->textData.font.numLetters; l++)
                {
                    vkCmdDraw(command, 4, 1, l * 4, 0);
                }
            }
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

    vkMapMemory(e_device, go->graphObj.shapes[0].vParam.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);

    memset(mapped, 0, BUFFER_SIZE * 4 * sizeof(Vertex2D));

    vkUnmapMemory(e_device, go->graphObj.shapes[0].vParam.vertexBufferMemory);
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

    vkMapMemory(e_device, go->graphObj.shapes[0].vParam.vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mapped);
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

    vkUnmapMemory(e_device, go->graphObj.shapes[0].vParam.vertexBufferMemory);
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

//Не корректно
void TextObjectRecreate(TextObject* to){

    for(int i=0; i < to->go.graphObj.gItems.num_shader_packs;i++)
    {
        BluePrintPack *pack = &to->go.graphObj.blueprints.blue_print_packs[i];

        PipelineSetting *settings = pack->settings;

        for(int i=0; i < pack->num_settings;i++)
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

    GameObjectSetDrawFunc(to, (void *)TextObjectDrawDefault);
    GameObjectSetRecreateFunc(to, (void *)TextObjectRecreate);

    //Загружаем шрифт и настраеваем его на работу
    TextDataInit(&to->textData, fontSize, fontPath);

    to->go.graphObj.num_shapes = 1;
}

void TextObjectAddDefault(TextObject* to, void *render)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT, (void *)TextObjectTransformBufferUpdate, 0);

    TextObjectAddTexture(to, nums);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_text_vert_spv_start, (size_t)(&_binary_shaders_text_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_text_frag_spv_start, (size_t)(&_binary_shaders_text_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    setting.flags |= ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE;

    GameObject2DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;
}

void TextObjectInitDefault(TextObject *to, int fontSize, const char* fontPath, DrawParam *dParam)
{
    TextObjectInit(to, fontSize, fontPath);
    TextObjectAddDefault(to, dParam->render);
    GameObject2DInitDraw(to);
}
