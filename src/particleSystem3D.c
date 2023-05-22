#include <particleSystem3D.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "e_tools.h"

#include "graphicsObject.h"

#include "e_camera.h"
#include "pipeline.h"
#include "e_buffer.h"
#include "e_texture.h"

#include "e_transform.h"

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

void Particle3DDefaultUpdate(ParticleObject3D* particle, BluePrintDescriptor *descriptor){

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

    vertexParam *vParam = &particle->go.graphObj.shapes[0].vParam;

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


    GameObject3DDescriptorModelUpdate(particle, descriptor);

}

void Particle3DDefaultDraw(GameObject3D* go){

    if(go->graphObj.shapes[0].vParam.verticesSize == 0)
        return;

    for(int i=0; i < go->graphObj.gItems.num_shader_packs;i++)
    {
        BluePrintPack *pack = &go->graphObj.blueprints.blue_print_packs[i];

        if(pack->render_point == current_render)
        {
            ShaderPack *pack = &go->graphObj.gItems.shader_packs[i];

            for(int j=0; j < pack->num_pipelines; j++){

                vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].pipeline);

                PipelineSetting *settings = &go->graphObj.blueprints.blue_print_packs[i].settings[j];

                vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
                vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

                VkBuffer vertexBuffers[] = {go->graphObj.shapes[settings->vert_indx].vParam.vertexBuffer};
                VkDeviceSize offsets[] = {0};

                vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

                vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pack->pipelines[j].layout, 0, 1, &pack->descriptor.descr_sets[imageIndex], 0, NULL);

                if(settings->flags & ENGINE_PIPELINE_FLAG_DRAW_INDEXED){
                    vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shapes[settings->vert_indx].iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shapes[settings->vert_indx].iParam.indexesSize, 1, 0, 0, 0);
                }else
                    vkCmdDraw(commandBuffers[imageIndex], go->graphObj.shapes[settings->vert_indx].vParam.verticesSize, 1, 0, 0);
            }
        }
    }
}

void Particle3DInit(ParticleObject3D* particle, DrawParam dParam){

    GameObjectSetUpdateFunc(particle, (void *)GameObject3DDefaultUpdate);
    GameObjectSetDrawFunc(particle, (void *)Particle3DDefaultDraw);
    GameObjectSetCleanFunc(particle, (void *)GameObject3DClean);
    GameObjectSetRecreateFunc(particle, (void *)GameObject3DRecreate);
    GameObjectSetDestroyFunc(particle, (void *)GameObject3DDestroy);

    Transform3DInit(&particle->go.transform);
    GraphicsObjectInit(&particle->go.graphObj, ENGINE_VERTEX_TYPE_3D_PARTICLE);

    GraphicsObjectSetVertex(&particle->go.graphObj, NULL, 0, sizeof(ParticleVertex3D), NULL, 0, sizeof(uint32_t));

    particle->go.graphObj.gItems.perspective = true;
    particle->go.self.flags = 0;

    particle->go.graphObj.shapes[0].vParam.vertices = calloc(particle->num_parts, sizeof(ParticleVertex3D));
    particle->go.graphObj.num_shapes = 1;
    particle->particles = (Particle3D*) calloc(particle->num_parts, sizeof(Particle3D));

    particle->go.images = calloc(1, sizeof(GameObjectImage));

    if(strlen(dParam.diffuse) != 0)
    {
        int len = strlen(dParam.diffuse);
        particle->go.images->path = calloc(len + 1, sizeof(char));
        memcpy(particle->go.images->path, dParam.diffuse, len);
        particle->go.images->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    particle->num_parts = 0;
}

void Particle3DAddDefault(ParticleObject3D* particle, void *render)
{
    uint32_t nums = particle->go.graphObj.blueprints.num_blue_print_packs;
    particle->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&particle->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)Particle3DDefaultUpdate, 0);

    BluePrintAddTextureImage(&particle->go.graphObj.blueprints, 0, particle->go.images, VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&particle->go.graphObj, &setting);

    if(strlen(setting.stages[0].some_shader) == 0 || strlen(setting.stages[1].some_shader) == 0)
    {
        PipelineSettingSetShader(&setting, &_binary_shaders_particle_vert3D_spv_start, (size_t)(&_binary_shaders_particle_vert3D_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
        PipelineSettingSetShader(&setting, &_binary_shaders_particle_frag3D_spv_start, (size_t)(&_binary_shaders_particle_frag3D_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

        setting.fromFile = 0;
    }

    setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);

    GameObject3DAddSettingPipeline(particle, nums, &setting);

    particle->go.graphObj.blueprints.num_blue_print_packs ++;
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
