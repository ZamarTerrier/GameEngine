#include "models.h"

#include "camera.h"
#include "gameObject.h"
#include "buffers.h"
#include "texture.h"

#include "e_math.h"

#include "objLoader.h"
#include "fbxLoader.h"

//Описание вертекса
VkVertexInputBindingDescription ModelObject3DGetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(ModelVertex3D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

void ModelDefaultDraw(ModelObject3D* mo){

    for(int i=0; i < mo->num_draw_nodes;i++){
        for(int j=0; j < mo->nodes[i].num_models; j++){

            ModelStruct *model = &mo->nodes[i].models[j];

            for(int p=0;p < model->graphObj.gItems.pipelineCount;p++)
            {
                vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, model->graphObj.gItems.graphicsPipeline[p]);

                PipelineSetting *settings = &model->graphObj.gItems.settings[p];

                vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
                vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

                VkBuffer vertexBuffers[] = {model->graphObj.shape.vParam.vertexBuffer};
                VkDeviceSize offsets[] = {0};

                vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

                vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, model->graphObj.gItems.pipelineLayout[p], 0, 1, &model->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

                switch(settings->drawType){
                    case 0:
                        vkCmdBindIndexBuffer(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                        vkCmdDrawIndexed(commandBuffers[imageIndex], model->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                        break;
                    case 1:
                        vkCmdDraw(commandBuffers[imageIndex], model->graphObj.shape.vParam.verticesSize, 1, 0, 0);
                        break;
                }
            }
        }
    }
}

void ModelDefaultUpdate(ModelObject3D* mo){

    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            if(mo->nodes[i].models[j].graphObj.local.descriptors == NULL)
                return;

            Camera3D* cam = (Camera3D*) cam3D;
            void* data;

            ModelBuffer3D mbo = {};
            vec3 cameraUp = {0.0f,1.0f, 0.0f};

            mat4 *mat = mo->obj + (i * mo->obj_size);
            mo->nodes[i].model = mat[0];

            mbo.model = mo->nodes[i].model;
            mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
            mbo.proj = m4_perspective(45.0f, 0.01f, 1000.0f);
            mbo.proj.m[1][1] *= -1;


            vkMapMemory(device, mo->nodes[i].models[j].graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
            memcpy(data, &mbo, sizeof(mbo));
            vkUnmapMemory(device, mo->nodes[i].models[j].graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

            InvMatrixsBuffer imb = {};
            memset(&imb, 0, sizeof(InvMatrixsBuffer));

            vkMapMemory(device, mo->nodes[i].models[j].graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(imb), 0, &data);
            memcpy(data, &imb, sizeof(imb));
            vkUnmapMemory(device, mo->nodes[i].models[j].graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);

            LightBuffer3D lbo = {};
            lbo.lights[0].position.x = 0;
            lbo.lights[0].position.y = 0;
            lbo.lights[0].position.z = 9.5f;
            lbo.lights[0].color.x = 0.0f;
            lbo.lights[0].color.y = 0.0f;
            lbo.lights[0].color.z = 0.0f;

            lbo.size = 0;

            vkMapMemory(device, mo->nodes[i].models[j].graphObj.local.descriptors[2].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(lbo), 0, &data);
            memcpy(data, &lbo, sizeof(lbo));
            vkUnmapMemory(device, mo->nodes[i].models[j].graphObj.local.descriptors[2].uniform->uniformBuffersMemory[imageIndex]);
        }
    }
}

void ModelClean(ModelObject3D* mo){
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            GraphicsObjectClean(&mo->nodes[i].models[j].graphObj);
        }
    }
}

void ModelRecreate(ModelObject3D* mo){
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            PipelineSetting *settings = (PipelineSetting *)mo->nodes[i].models[j].graphObj.gItems.settings;

            for(int i=0; i < mo->nodes[i].models[j].graphObj.gItems.settingsCount;i++)
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

            BuffersRecreateUniform(&mo->nodes[i].models[j].graphObj.local);
            GameObject3DCreateDrawItems(&mo->nodes[i].models[j].graphObj);
            PipelineCreateGraphics(&mo->nodes[i].models[j].graphObj);
        }
    }
}

void ModelDestroy(ModelObject3D* mo){
    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            GraphicsObjectDestroy(&mo->nodes[i].models[j].graphObj);
        }
    }
}

void ModelAddSettingPipeline(ModelStruct* model, PipelineSetting setting){
    PipelineSetting* settings;

    model->graphObj.gItems.settingsCount++;
    model->graphObj.gItems.settings = realloc(model->graphObj.gItems.settings, model->graphObj.gItems.settingsCount * sizeof(PipelineSetting));

    settings = (PipelineSetting *) model->graphObj.gItems.settings;

    memcpy(&settings[model->graphObj.gItems.settingsCount - 1], &setting, sizeof(PipelineSetting));
}

void* ModelDefaultInit(ModelStruct *model, DrawParam dParam, void* stbi_point){

    BuffersAddUniformObject(&model->graphObj.local, sizeof(ModelBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&model->graphObj.local, sizeof(InvMatrixsBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&model->graphObj.local, sizeof(LightBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    void *stbi_info = NULL;

    if(strlen(dParam.filePath) != 0)
        stbi_info = ImageAddTexture(&model->graphObj.local, dParam.filePath, stbi_point);

    GameObject3DCreateDrawItems(&model->graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&model->graphObj, &setting);

    model->graphObj.gItems.settingsCount = 0;

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_model_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_model_vert_spv_size);
        setting.fragShader = &_binary_shaders_model_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_model_frag_spv_size);
        setting.fromFile = 0;
    }
    else
        GraphicsObjectSetShadersPath(&model->graphObj, dParam.vertShader, dParam.fragShader);

    ModelAddSettingPipeline(model, setting);

    PipelineCreateGraphics(&model->graphObj);

    return stbi_info;
}
