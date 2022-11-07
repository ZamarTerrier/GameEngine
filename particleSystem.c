#include <particleSystem.h>

#include "tools.h"

#include "camera.h"
#include "pipeline.h"
#include "e_resource.h"
#include "buffers.h"
#include "texture.h"

#include "e_math.h"

Vertex3D* partVert;
Particle* particles;
int partCount = 0;


void FindParticle3D(){

    Particle* temp;

    for(int i=0; i < partCount; i++)
    {
        if(particles[i].life <= 0){

            for(int j=i; j < partCount; j++)
            {
                if(j != partCount - 1)
                    particles[j] = particles[j + 1];
            }

            partCount--;

            particles = (Particle *) realloc(particles, partCount * sizeof(Particle));

            i--;
        }
    }

}


void UpdateParticles(GameObject3D* go){


    if(go->graphObj.local.descriptors == NULL)
        return;

    if(partCount == 0)
        return;

    for(int i=0; i < partCount; i++)
    {
        particles[i].life -= 0.01;
        particles[i].position = v3_add(particles[i].position, v3_muls(v3_divs(particles[i].direction, 10), particles[i].speed)) ;
    }

    FindParticle3D();

    Camera3D* cam = (Camera3D*) cam3D;
    void* data;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    mbo.model = m4_translate_mat_add(m4_mult(m4_scale_mat(go->transform.scale), m4_rotation_matrix(go->transform.rotation)), go->transform.position);
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.01f, 100.0f);
    mbo.proj.m[1][1] *= -1;

    vkMapMemory(device, go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
    memcpy(data, &mbo, sizeof(mbo));
    vkUnmapMemory(device,  go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

    DataBuffer dbo = {};
    dbo.camRot = cam->rotation;
    dbo.camPos = cam->position;

    double xpos, ypos;

    dbo.time = glfwGetTime();
    EngineGetCursorPos(&xpos, &ypos);

    dbo.mouse.x = xpos / (WIDTH);
    dbo.mouse.y = ypos / (HEIGHT);

    vkMapMemory(device,  go->graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(dbo), 0, &data);
    memcpy(data, &dbo, sizeof(dbo));
    vkUnmapMemory(device,  go->graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);

}

void ParticleDraw(GameObject3D* go){

    if(partCount == 0)
        return;

    if(partCount != go->graphObj.shape.vParam.verticesSize)
        partVert = (Vertex3D *) realloc(partVert, partCount * sizeof(Vertex3D));

    for(int i=0; i < partCount; i++)
    {
        partVert[i].position = v3_divs(particles[i].position, 10) ;
    }

    if(partCount != 0)
        GraphicsObjectSetVertex(&go->graphObj, partVert, partCount, NULL, 0);

    for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){

        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.graphicsPipeline[i]);

        PipelineSetting *settings = &go->graphObj.gItems.settings[i];

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vParam.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelineLayout[i], 0, 1, &go->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

        switch(settings->drawType){
            case 0:
                vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
                vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                break;
            case 1:
                vkCmdDraw(commandBuffers[imageIndex], go->graphObj.shape.vParam.verticesSize, 1, 0, 0);
                break;
        }

    }

}

void InitParticle3D(GameObject3D* particle, vec3 position, const char* texturePath, const char* vertShader, const char* fragShader){

    GameObject3DInit(particle);

    GameObjectSetUpdateFunc(particle, (void *)UpdateParticles);
    GameObjectSetDrawFunc(particle, (void *)ParticleDraw);

    partVert = (Vertex3D*) calloc(partCount, sizeof(Vertex3D));
    particles = (Particle*) calloc(partCount, sizeof(Particle));

    BuffersAddUniformObject(&particle->graphObj.local, sizeof(ModelBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&particle->graphObj.local, sizeof(DataBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    particle->image = calloc(1, sizeof(ImageStruct));

    if(strlen(texturePath) != 0)
    {
        particle->image->path = texturePath;
        particle->image->buffer = ToolsLoadImageFromFile(&particle->image->size, texturePath);

        ImageAddTexture(&particle->graphObj.local, particle->image);
    }


    GameObject3DCreateDrawItems(particle);

    PipelineSetting setting;

    setting.poligonMode = VK_POLYGON_MODE_FILL;
    setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    memcpy(setting.vertShader, vertShader, 256);
    memcpy(setting.fragShader, fragShader, 256);
    setting.drawType = 1;
    setting.fromFile = 1;
    GameObject3DAddSettingPipeline(particle, setting);
    PipelineCreateGraphics(&particle->graphObj);

    Transform3DSetScale(particle, (vec3){1,1,1});
    Transform3DSetPosition(particle, position);
}

void AddParticle3D(GameObject3D* particle, vec3 position, vec3 direction, float life,float speed){

    partCount ++;

    particles = (Particle*) realloc(particles, partCount * sizeof(Particle));

    Particle part = { position, direction, life, speed};

    particles[partCount - 1] = part;

}
