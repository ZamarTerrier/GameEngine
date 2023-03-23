#include <particleSystem3D.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "tools.h"

#include "graphicsObject.h"

#include "camera.h"
#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

#include "transform.h"

#include "e_math.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void Particle3DFind(ParticleObject3D *particle){

    for(int i=0; i < particle->num_parts; i++)
    {
        if(particle->particles[i].life <= 0){

            for(int j=i; j < particle->num_parts; j++)
            {
                if(j != particle->num_parts - 1)
                    particle->particles[j] = particle->particles[j + 1];
            }

            particle->num_parts--;

            particle->particles = (Particle3D *) realloc(particle->particles, particle->num_parts * sizeof(Particle3D));

            i--;
        }
    }

}

void Particle3DDefaultUpdate(ParticleObject3D* particle){


    if(particle->go.graphObj.blueprints.descriptors == NULL)
        return;

    if(particle->num_parts == 0)
        return;

    for(int i=0; i < particle->num_parts; i++)
    {
        particle->particles[i].life -= 0.01;
        particle->particles[i].direction.y -= 0.1f * particle->particles[i].gravity * 10;
        particle->particles[i].position = v3_add(particle->particles[i].position, v3_muls(v3_divs(particle->particles[i].direction, 10), particle->particles[i].speed)) ;
        particle->particles[i].scale -= 0.001f * particle->particles[i].speed;
    }

    Particle3DFind(particle);

    if(particle->num_parts == 0)
        return;

    vertexParam *vParam = &particle->go.graphObj.shape.vParam;

    if(vParam->vertices != NULL)
        free(vParam->vertices);

    vParam->vertices = calloc(particle->num_parts, sizeof(ParticleVertex3D));

    ParticleVertex3D *verts = vParam->vertices;

    for(int i=0; i < particle->num_parts; i++)
    {
        verts[i].color = particle->particles[i].color;
        verts[i].position = v3_divs(particle->particles[i].position, 10);
        verts[i].size = particle->particles[i].scale;
    }

    BuffersUpdateVertex(vParam);

    Camera3D* cam = (Camera3D*) cam3D;

    ModelBuffer3D mbo = {};
    vec3 cameraUp = {0.0f,1.0f, 0.0f};

    mbo.model = m4_translate_mat_add(m4_mult(m4_scale_mat(particle->go.transform.scale), m4_rotation_matrix(particle->go.transform.rotation)), particle->go.transform.position);
    mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
    mbo.proj = m4_perspective(45.0f, 0.01f, MAX_CAMERA_VIEW_DISTANCE);
    mbo.proj.m[1][1] *= -1;

    DescriptorUpdate(particle->go.graphObj.blueprints.descriptors, 0, &mbo, sizeof(mbo));

}

void Particle3DDefaultDraw(GameObject3D* go){

    if(go->graphObj.shape.vParam.verticesSize == 0)
        return;

    for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){

        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelines[i].pipeline);

        PipelineSetting *settings = &go->graphObj.gItems.settings[i];

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vParam.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelines[i].layout, 0, 1, &go->graphObj.gItems.descriptors.descr_sets[imageIndex], 0, NULL);

        switch(settings->drawType){
            case 0:
                vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                break;
            case 1:
                vkCmdDraw(commandBuffers[imageIndex], go->graphObj.shape.vParam.verticesSize, 1, 0, 0);
                break;
        }

    }

}

void Particle3DInit(ParticleObject3D* particle, DrawParam dParam){

    GameObjectSetUpdateFunc(particle, (void *)Particle3DDefaultUpdate);
    GameObjectSetDrawFunc(particle, (void *)Particle3DDefaultDraw);
    GameObjectSetCleanFunc(particle, (void *)GameObject3DClean);
    GameObjectSetRecreateFunc(particle, (void *)GameObject3DRecreate);
    GameObjectSetDestroyFunc(particle, (void *)GameObject3DDestroy);

    particle->go.graphObj.blueprints.descriptors = (ShaderDescriptor *) calloc(MAX_UNIFORMS, sizeof(ShaderDescriptor));

    Transform3DInit(&particle->go.transform);
    GraphicsObjectInit(&particle->go.graphObj, ENGINE_VERTEX_TYPE_3D_PARTICLE);

    GraphicsObjectSetVertexSize(&particle->go.graphObj, sizeof(ParticleVertex3D), sizeof(uint32_t));
    GraphicsObjectSetVertex(&particle->go.graphObj, NULL, 0, NULL, 0);

    particle->go.graphObj.gItems.perspective = true;
    particle->go.enable_light = false;

    particle->go.graphObj.shape.vParam.vertices = calloc(particle->num_parts, sizeof(ParticleVertex3D));
    particle->particles = (Particle3D*) calloc(particle->num_parts, sizeof(Particle3D));

    BuffersAddUniformObject(&particle->go.graphObj.blueprints, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);

    particle->go.images = calloc(1, sizeof(GameObjectImage));

    if(strlen(dParam.diffuse) != 0)
    {
        int len = strlen(dParam.diffuse);
        particle->go.images->path = calloc(len + 1, sizeof(char));
        memcpy(particle->go.images->path, dParam.diffuse, len);
        particle->go.images->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    TextureImageAdd(&particle->go.graphObj.blueprints, particle->go.images);

    GraphicsObjectCreateDrawItems(&particle->go.graphObj, false);

    PipelineSetting setting;

    PipelineSettingSetDefault(&particle->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_particle_vert3D_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_particle_vert3D_spv_size);
        setting.fragShader = &_binary_shaders_particle_frag3D_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_particle_frag3D_spv_size);
        setting.fromFile = 0;
    }

    setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    setting.drawType = 1;

    GameObject3DAddSettingPipeline(particle, &setting);

    PipelineCreateGraphics(&particle->go.graphObj, false);

    particle->num_parts = 0;
}

void Particle3DAdd(ParticleObject3D* particle, vec3 position, vec3 direction, float speed, float gravity, float life){

    double time = EngineGetTime();

    srand((uint32_t)(time * 1000));

    particle->num_parts ++;

    particle->particles = (Particle3D*) realloc(particle->particles, particle->num_parts * sizeof(Particle3D));

    Particle3D part;
    part.position = position;
    part.direction = direction;
    part.life = life;
    part.speed = speed;
    part.gravity = gravity;

    float min_size = 0.3f;
    float max_size = 1.0f;

    part.scale = min_size + (((float)rand() / (float)RAND_MAX) * (max_size - min_size));

    part.color.x = min_size + (((float)rand() / (float)RAND_MAX) * (max_size - min_size));
    part.color.y = min_size + (((float)rand() / (float)RAND_MAX) * (max_size - min_size));
    part.color.z = min_size + (((float)rand() / (float)RAND_MAX) * (max_size - min_size));


    particle->particles[particle->num_parts - 1] = part;

}
