
#include "graphicsObject.h"

#include <vulkan/vulkan.h>

#include "e_descriptor.h"
#include "pipeline.h"
#include "e_buffer.h"
#include "e_texture.h"

#include "e_resource_data.h"
#include "e_resource_descriptors.h"
#include "e_resource_engine.h"

void GraphicsObjectInit(GraphicsObject* graphObj, uint32_t type)
{
    graphObj->blueprints.num_blue_print_packs = 0;

    graphObj->blueprints.isShadow = false;

    if(graphObj->num_shapes == 0)
        memset(&graphObj->shapes, 0, sizeof(Shape) * MAX_SHAPES);

    switch(type)
    {
        case ENGINE_VERTEX_TYPE_2D_OBJECT:
            graphObj->shapes[0].bindingDescription = &Bind2DDescription;
            graphObj->shapes[0].attr = planeAttributeDescription;
            graphObj->shapes[0].countAttr = 3;
            graphObj->shapes[0].countBind = 1;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_3D_OBJECT:
            graphObj->shapes[0].bindingDescription = &Bind3DDescription;
            graphObj->shapes[0].attr = cubeAttributeDescription;
            graphObj->shapes[0].countAttr = 3;
            graphObj->shapes[0].countBind = 1;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_3D_INSTANCE:
            graphObj->shapes[0].bindingDescription = calloc(2, sizeof(EIVertexInputBindingDescription));
            graphObj->shapes[0].bindingDescription[0] = Bind3DDescription;
            graphObj->shapes[0].bindingDescription[1] = Bind3DInstanceDescription;
            graphObj->shapes[0].attr = instanceAttributeDescription;
            graphObj->shapes[0].countAttr = 6;
            graphObj->shapes[0].countBind = 2;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_TREE_INSTANCE:
            graphObj->shapes[0].bindingDescription = calloc(2, sizeof(EIVertexInputBindingDescription));
            graphObj->shapes[0].bindingDescription[0] = BindTree3DDescription;
            graphObj->shapes[0].bindingDescription[1] = Bind3DInstanceDescription;
            graphObj->shapes[0].attr = treeInstanceAttributeDescription;
            graphObj->shapes[0].countAttr = 7;
            graphObj->shapes[0].countBind = 2;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_MODEL_OBJECT:
            graphObj->shapes[0].bindingDescription = &BindModel3DDescription;
            graphObj->shapes[0].attr = modelAttributeDescription;
            graphObj->shapes[0].countAttr = 5;
            graphObj->shapes[0].countBind = 1;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_2D_PARTICLE:
            graphObj->shapes[0].bindingDescription = &BindParticle2DDescription;
            graphObj->shapes[0].attr = particle2DAttributeDescription;
            graphObj->shapes[0].countAttr = 3;
            graphObj->shapes[0].countBind = 1;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_3D_PARTICLE:
            graphObj->shapes[0].bindingDescription = &BindParticle3DDescription;
            graphObj->shapes[0].attr = particle3DAttributeDescription;
            graphObj->shapes[0].countAttr = 3;
            graphObj->shapes[0].countBind = 1;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_TERRAIN:
            graphObj->shapes[0].bindingDescription = &BindTerrainDescription;
            graphObj->shapes[0].attr = TerrainAttributeDescription;
            graphObj->shapes[0].countAttr = 3;
            graphObj->shapes[0].countBind = 1;
            graphObj->shapes[0].type = type;
            break;
        case ENGINE_VERTEX_TYPE_SKY:
            graphObj->shapes[0].bindingDescription = &BindSkyDescription;
            graphObj->shapes[0].attr = SkyAttributeDescription;
            graphObj->shapes[0].countAttr = 2;
            graphObj->shapes[0].countBind = 1;
            graphObj->shapes[0].type = type;
            break;
        default:
            graphObj->shapes[0].type = 0;
            break;
    }
}

void GraphicsObjectSetVertex(GraphicsObject* graphObj, void *vert, int vertCount, uint32_t type_v_size, uint32_t *inx, int indxCount, uint32_t type_i_size){

    uint32_t num = graphObj->num_shapes;

    graphObj->shapes[num].vParam.typeSize = type_v_size;
    graphObj->shapes[num].iParam.typeSize = type_i_size;

    int res = 0;

    if(vert != NULL)
    {
        graphObj->shapes[num].vParam.vertices = calloc(vertCount, graphObj->shapes[num].vParam.typeSize);
        memcpy(graphObj->shapes[num].vParam.vertices, vert, graphObj->shapes[num].vParam.typeSize * vertCount);
        graphObj->shapes[num].vParam.verticesSize = vertCount;
    }

    if(inx != NULL)
    {
        graphObj->shapes[num].iParam.indices = calloc(indxCount, graphObj->shapes[num].iParam.typeSize);
        memcpy(graphObj->shapes[num].iParam.indices, inx, graphObj->shapes[num].iParam.typeSize * indxCount);
        graphObj->shapes[num].iParam.indexesSize = indxCount;
    }

    if(!graphObj->shapes[num].init)
    {
        res = BuffersCreateVertex(&graphObj->shapes[num].vParam);
        if(res)
            return;

        res = BuffersCreateIndex(&graphObj->shapes[num].iParam);
        if(res)
            return;

        graphObj->shapes[num].init = true;
    }

    if(graphObj->shapes[num].vParam.verticesSize > 0)
        BuffersUpdateVertex(&graphObj->shapes[num].vParam);

    if(graphObj->shapes[num].iParam.indexesSize > 0)
        BuffersUpdateIndex(&graphObj->shapes[num].iParam);

    graphObj->num_shapes ++;
}

void GraphicsObjectCreateDrawItems(GraphicsObject* graphObj){

    memset(graphObj->gItems.shader_packs, 0, sizeof(ShaderPack) * MAX_BLUE_PRINTS);


    for(int i=0;i< graphObj->blueprints.num_blue_print_packs;i++)
    {

        BluePrintPack *pack = &graphObj->blueprints.blue_print_packs[i];

        DescriptorCreate(&graphObj->gItems.shader_packs[i].descriptor, pack->descriptors, &graphObj->blueprints, pack->num_descriptors, imagesCount);

    }

}

void GraphicsObjectClean(GraphicsObject *graphObj)
{
    for(int i=0;i < graphObj->gItems.num_shader_packs;i++)
    {
        PipelineDestroy(&graphObj->gItems.shader_packs[i]);

        DescriptorDestroy(&graphObj->gItems.shader_packs[i].descriptor);
    }

    for(int i=0;i < graphObj->blueprints.num_blue_print_packs;i++)
    {
        for(int j=0;j < graphObj->blueprints.blue_print_packs[i].num_descriptors;j++)
        {
            BluePrintDescriptor *descriptor = &graphObj->blueprints.blue_print_packs[i].descriptors[j];

            if(!(descriptor->flags & ENGINE_BLUE_PRINT_FLAG_LINKED_UNIFORM))
            {
                if(descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
                    for (int j = 0; j < imagesCount; j++) {
                        BuffersDestroyBuffer(descriptor->uniform->uniformBuffers[j]);
                    }
                    free(descriptor->uniform->uniformBuffers);
                    descriptor->uniform->uniformBuffers = NULL;
                    free(descriptor->uniform->uniformBuffersMemory);
                    descriptor->uniform->uniformBuffersMemory = NULL;
                }
            }
        }
    }
}

void GraphicsObjectDestroy(GraphicsObject* graphObj){


    for(int i=0;i < graphObj->gItems.num_shader_packs;i++)
    {
        PipelineDestroy(&graphObj->gItems.shader_packs[i]);

        DescriptorDestroy(&graphObj->gItems.shader_packs[i].descriptor);
    }

    for(int i=0;i < graphObj->blueprints.num_blue_print_packs;i++)
    {
        for(int j=0;j < graphObj->blueprints.blue_print_packs[i].num_descriptors;j++)
        {
            BluePrintDescriptor *descriptor = &graphObj->blueprints.blue_print_packs[i].descriptors[j];

            if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || descriptor->descrType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE){

                if(descriptor->flags & ENGINE_BLUE_PRINT_FLAG_LINKED_TEXTURE)
                    continue;

                if((descriptor->flags & ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE) && (descriptor->flags & ENGINE_BLUE_PRINT_FLAG_ARRAY_IMAGE))
                {
                    Texture2D **textures = descriptor->textures;

                    for(int i=0;i < descriptor->size;i++)
                    {
                        if(textures[0][i].flags & ENGINE_TEXTURE2D_FLAG_GENERATED)
                        {
                            ImageDestroyTexture(&textures[0][i]);
                        }else if(textures[0][i].flags & ENGINE_TEXTURE2D_FLAG_VIEW){
                            vkDestroyImageView(e_device, textures[0][i].textureImageView, NULL);
                        }
                    }

                    free(descriptor->textures);

                }else if((descriptor->flags & ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE)){
                    Texture2D **textures = descriptor->textures;

                    if(textures[0]->flags & ENGINE_TEXTURE2D_FLAG_GENERATED)
                    {
                        ImageDestroyTexture(textures[0]);
                        free(textures[0]);
                    }else if(textures[0]->flags & ENGINE_TEXTURE2D_FLAG_VIEW){
                        vkDestroyImageView(e_device, textures[0]->textureImageView, NULL);
                    }

                    free(descriptor->textures);
                }else{
                    Texture2D **textures = descriptor->textures;

                    for(int i=0;i < imagesCount;i++)
                    {
                        for(int j=0;j < descriptor->size;j++)
                        {
                            if(textures[i][j].flags & ENGINE_TEXTURE2D_FLAG_GENERATED)
                            {
                                ImageDestroyTexture(&textures[i][j]);
                            }else if(textures[i][j].flags & ENGINE_TEXTURE2D_FLAG_VIEW){
                                vkDestroyImageView(e_device, textures[i][j].textureImageView, NULL);
                            }
                        }
                        free(textures[i]);
                    }

                    free(descriptor->textures);
                }

                descriptor->textures = NULL;
            }else{
                if(descriptor->flags & ENGINE_BLUE_PRINT_FLAG_LINKED_UNIFORM)
                    continue;

                for (int k = 0; k < imagesCount; k++) {
                    BuffersDestroyBuffer(descriptor->uniform->uniformBuffers[k]);
                }

                free(descriptor->uniform->uniformBuffers);
                descriptor->uniform->uniformBuffers = NULL;
                free(descriptor->uniform->uniformBuffersMemory);
                descriptor->uniform->uniformBuffersMemory = NULL;
            }
        }
    }

    for(int i=0;i < graphObj->num_shapes;i++)
    {
        if(graphObj->shapes[i].iParam.indexesSize > 0)
            BuffersDestroyBuffer(graphObj->shapes[i].iParam.indexBuffer);

        graphObj->shapes[i].iParam.indexBuffer = NULL;
        graphObj->shapes[i].iParam.indexBufferMemory = NULL;

        BuffersDestroyBuffer(graphObj->shapes[i].vParam.vertexBuffer);
        graphObj->shapes[i].vParam.vertexBuffer = NULL;
        graphObj->shapes[i].vParam.vertexBufferMemory = NULL;
    }
}

void GraphicsObjectSetShadersPath(GraphicsObject *graphObj, const char *vert, const char *frag)
{

    int len = strlen(vert);
    memset(graphObj->aShader.vertShader, 0, 256);
    memcpy(graphObj->aShader.vertShader, vert, len);


    len = strlen(frag);
    memset(graphObj->aShader.fragShader, 0, 256);
    memcpy(graphObj->aShader.fragShader, frag, len);
}
