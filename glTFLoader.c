#include "glTFLoader.h"

#include "models.h"

#include "camera.h"
#include "gameObject.h"
#include "graphicsObject.h"
#include "transform.h"
#include "buffers.h"

#include "e_math.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

int frameCurr = 0;

void QuatToAngleAxis(vec4 q, float *outAngleDegrees, vec3 *axis) {

  float angleRadians = 2 * acos(q.w);
  if (angleRadians == 0.0) {
    *outAngleDegrees = 0.0;
    axis->x = 0.0;
    axis->y = 0.0;
    axis->z = 1.0;
    return;
  }

  float denom = sqrt(1-q.w*q.w);
  *outAngleDegrees = angleRadians * 180.0 / M_PI;
  axis->x = q.x / denom;
  axis->y = q.y / denom;
  axis->z = q.z / denom;
}

size_t ComponentTypeByteSize(int type) {
  switch (type) {
    case cgltf_component_type_r_8u:
    case cgltf_component_type_r_8:
      return sizeof(char);
    case cgltf_component_type_r_16u:
    case cgltf_component_type_r_16:
      return sizeof(short);
    case cgltf_component_type_r_32u:
      return sizeof(int);
    case cgltf_component_type_r_32f:
      return sizeof(float);
    case cgltf_component_type_max_enum:
      return sizeof(double);
    default:
      return 0;
  }
}

mat4 getLocalTransform(cgltf_node *node){

    cgltf_float matrix[16];

    cgltf_node_transform_local(node, matrix);

    mat4 mat = mat4_colmnsf(
                        matrix[0],  matrix[1],  matrix[2],  matrix[3],
                        matrix[4],  matrix[5],  matrix[6],  matrix[7],
                        matrix[8],  matrix[9],  matrix[10], matrix[11],
                        matrix[12], matrix[13], matrix[14], matrix[15]
                            );

    return mat;
}


mat4 getFullTransform(cgltf_node *node){

    mat4 mat = getLocalTransform(node);

    const cgltf_node* parent = node->parent;

    while (parent)
    {
        mat4 pm = getLocalTransform(parent);

        mat = mat4_mult_transform(mat, pm);

        parent = parent->parent;
    }

    return mat;
}


void readKeyframeValuesV3(anim_keyframe_struct *keyframes, cgltf_animation_channel *chanel)
{
    vec3 temp;
    vec3 *fdata = chanel->sampler->input->buffer_view->data + chanel->sampler->input->buffer_view->offset;
    for (size_t i = 0; i < chanel->sampler->output->count; i++)
    {
        temp = fdata[i];
        keyframes[i].vector3 = fdata[i];
    }
}

void readKeyframeValuesV4(anim_keyframe_struct *keyframes, cgltf_animation_channel *chanel)
{
    vec4 *fdata = chanel->sampler->input->buffer_view->data + chanel->sampler->input->buffer_view->offset;
    for (size_t i = 0; i < chanel->sampler->output->count; i++)
        keyframes[i].vector4 = fdata[i];
}

void readKeyframeValues(anim_channel *a_channel, cgltf_animation_channel *chanel)
{
    if(chanel->sampler->output->type == cgltf_type_vec3)
    {
        a_channel->type = chanel->target_path;
        readKeyframeValuesV3(a_channel->keyframes, chanel);
    }else if(chanel->sampler->output->type == cgltf_type_vec4)
    {
        a_channel->type = chanel->target_path;
        readKeyframeValuesV4(a_channel->keyframes, chanel);
    }
}

void readKeyframeTimes(anim_channel *a_channel, cgltf_animation_channel *chanel)
{

    float *fdata = chanel->sampler->input->buffer_view->data + chanel->sampler->input->buffer_view->offset;
    for (size_t i = 0; i < chanel->sampler->input->count; i++)
        a_channel->keyframes[i].time = fdata[i];
}

vec3 getValueV3(anim_channel *channel, int curr_key,float time)
{
    if(channel->num_keyframes <= 0)
        return (vec3){0,0,0};


    if(curr_key == 0)
        return channel->keyframes[0].vector3;

    if(curr_key == channel->num_keyframes - 1)
        return channel->keyframes[0].vector3;

    anim_keyframe_struct * now = &channel->keyframes[curr_key + 0];
    anim_keyframe_struct * next = &channel->keyframes[curr_key + 1];
    anim_keyframe_struct * before = &channel->keyframes[curr_key + 2];


    if (time <= channel->keyframes[curr_key + 1].time)
        return channel->keyframes[curr_key + 1].vector3;
    if (time >= channel->keyframes[curr_key - 1].time)
        return channel->keyframes[curr_key - 1].vector3;

    size_t toFrameIndex = 0;
    for (size_t i = 0; i < channel->num_keyframes; ++i) {
        if (time <= channel->keyframes[i].time) {
            toFrameIndex = i;
            break;
        }
    }

    if(toFrameIndex > 0)
    {
        anim_keyframe_struct fromFrame = channel->keyframes[toFrameIndex - 1];
        anim_keyframe_struct toFrame = channel->keyframes[toFrameIndex];
        vec3 val1 = channel->keyframes[toFrameIndex - 1].vector3;
        vec3 val2 = channel->keyframes[toFrameIndex].vector3;
        // time <= toFrame.time, time > fromFrame.time
        uint32_t alpha = (time - fromFrame.time) / (toFrame.time - fromFrame.time);

        return v3_lerp(val1, val2, alpha);
    }

    return (vec3){0,0,0};
}

vec4 getValueV4(anim_channel *channel, int curr_key,float time)
{
    if(channel->num_keyframes <= 0)
        return;

    if(curr_key == 0)
        return channel->keyframes[curr_key].vector4;

    if(curr_key == channel->num_keyframes - 1)
        return channel->keyframes[0].vector4;

    if (time <= channel->keyframes[curr_key + 1].time)
        return channel->keyframes[curr_key + 1].vector4;
    if (time >= channel->keyframes[curr_key - 1].time)
        return channel->keyframes[curr_key - 1].vector4;

    size_t toFrameIndex = 0;
    for (size_t i = 0; i < channel->num_keyframes; ++i) {
        if (time <= channel->keyframes[i].time) {
            toFrameIndex = i;
            break;
        }
    }

    if(toFrameIndex > 0)
    {
        anim_keyframe_struct fromFrame = channel->keyframes[toFrameIndex - 1];
        anim_keyframe_struct toFrame = channel->keyframes[toFrameIndex];
        vec4 val1 = channel->keyframes[toFrameIndex - 1].vector4;
        vec4 val2 = channel->keyframes[toFrameIndex].vector4;
        // time <= toFrame.time, time > fromFrame.time
        uint32_t alpha = (time - fromFrame.time) / (toFrame.time - fromFrame.time);

        return v4_lerp(val1, val2, alpha);
    }

    return (vec4){0,0,0,0};
}

float getDuration(anim_channel *channels, uint32_t num_channels)
{
    if(!frameCurr)
        return 0;

    float dur = 0;
    for (int i=0;i < num_channels;i++) {
        float channelDur = channels[i].keyframes[frameCurr - 1].time;
        if (channelDur > dur)
            dur = channelDur;
    }

    return dur;
}

void update_frame(anim_array * animation, glTFStruct *glTF, uint32_t node_id, float time)
{

    float duration = getDuration(animation->channels, animation->num_channels);

    float wrappedTime = fmod(time, duration);

    for(int i=0;i< animation->num_channels;i++)
    {
        anim_channel *channel = &animation->channels[i];
        if(channel->node_id == node_id)
        {
                gltf_node *node = NULL;

                for(int j=0;j < glTF->num_nodes; j++)
                    if(glTF->nodes[j].id_node == node_id)
                        node = &glTF->nodes[j];

                if(node == NULL)
                    continue;

                vec3 vector3;
                vec4 vector4;
                mat4 transform;

                if(frameCurr >= channel->num_keyframes)
                    frameCurr = 0;

                switch(channel->type)
                {
                    case cgltf_animation_path_type_translation:
                        vector3 = getValueV3(channel, frameCurr, wrappedTime);
                        transform = m4_translate_mat(vector3);
                        node->global_matrix.m[3][0] = vector3.x;//mat4_mult_transform(node->global_matrix, transform);
                        node->global_matrix.m[3][1] = vector3.y;
                        node->global_matrix.m[3][2] = vector3.z;
                        break;
                    /*case cgltf_animation_path_type_rotation:
                        vector4 = getValueV4(channel, frameCurr, time);
                        transform = m4_rotation_quaternion(vector4);
                        node->global_matrix = mat4_mult_transform(node->global_matrix, transform);
                        break;
                    case cgltf_animation_path_type_scale:
                        vector3 = getValueV3(channel, frameCurr, time);
                        transform = m4_translate_mat(vector3);
                        node->global_matrix = mat4_mult_transform(node->global_matrix, transform);
                        break;*/
                }
        }
    }

    frameCurr++;
}

void SetupMeshState(glTFStruct *glTF, cgltf_data *model) {

    glTF->num_nodes = model->nodes_count;
    glTF->nodes = calloc(glTF->num_nodes, sizeof(gltf_node));

    glTF->num_anims = model->animations_count;
    glTF->animations = calloc(model->animations_count, sizeof(anim_array));

    for(int i=0;i < glTF->num_anims;i++ )
    {
        cgltf_animation* anim_gltf = &model->animations[i];
        anim_array *animation = &glTF->animations[i];

        animation->channels = calloc( anim_gltf->channels_count, sizeof(anim_channel));
        animation->num_channels = anim_gltf->channels_count;

        for(int j=0; j < anim_gltf->channels_count; j++)
        {
            cgltf_animation_channel *channel = &anim_gltf->channels[j];
            cgltf_animation_sampler *sampler = channel->sampler;

            anim_channel *a_channel = &animation->channels[j];

            a_channel->keyframes = calloc(sampler->output->count, sizeof(anim_keyframe_struct));
            a_channel->num_keyframes = sampler->output->count;

            readKeyframeTimes(a_channel, channel);
            readKeyframeValues(a_channel, channel);

        }
    }

    int iter = 0;
    for(int i = 0; i < model->nodes_count; i++)
    {
        cgltf_node *node = &model->nodes[i];

        glTF->nodes[i].id_node = i;

        if(node->mesh != NULL)
        {
            glTF->nodes[i].mesh = calloc(1, sizeof(engine_mesh));
            glTF->nodes[i].isModel = true;
            cgltf_mesh* mesh = node->mesh;
            engine_mesh *g_mesh = glTF->nodes[i].mesh;

            if (mesh->primitives_count > 1) {
                continue;
            }

            cgltf_primitive *primitive = &mesh->primitives[0];

            g_mesh->num_indices = primitive->indices->count;
            g_mesh->indices = (uint32_t *)calloc(g_mesh->num_indices, sizeof(uint32_t));

            g_mesh->num_verts = primitive->attributes[0].data->count;
            g_mesh->verts = (Vertex3D *)calloc(g_mesh->num_verts, sizeof(Vertex3D));

            for(int j=0; j < primitive->attributes_count; j++)
            {
                cgltf_attribute *attribute = &primitive->attributes[j];

                vec3 temp;

                vec2* v2_point;
                vec3* v3_point;

                v3_point = attribute->data->buffer_view->buffer->data + attribute->data->buffer_view->offset;
                v2_point = attribute->data->buffer_view->buffer->data + attribute->data->buffer_view->offset;

                for(int v=0;v < g_mesh->num_verts; v++)
                {
                    g_mesh->verts[v].color = (vec3){1,1,1};
                }

                for(int v=0;v < g_mesh->num_verts; v++)
                {
                    switch(attribute->type)
                    {
                        case cgltf_attribute_type_position:
                            temp = v3_point[v];
                            g_mesh->verts[v].position = v3_point[v];
                            break;
                        case cgltf_attribute_type_normal:
                            g_mesh->verts[v].normal = v3_point[v];
                            break;
                        case cgltf_attribute_type_texcoord:
                            g_mesh->verts[v].texCoord = v2_point[v];
                            break;
                        case cgltf_attribute_type_color:
                            g_mesh->verts[v].color = v3_point[v];
                            break;
                        default:
                            break;
                    }
                }
            }

            uint16_t * ind_point = primitive->indices->buffer_view->buffer->data + primitive->indices->buffer_view->offset;

            for(int j=0; j < g_mesh->num_indices; j++)
            {
                g_mesh->indices[j] = ind_point[j];
            }


            for(int j=0;j < model->animations_count;j++ )
            {
                cgltf_animation* anim = &model->animations[j];

                for(int k=0; k < anim->channels_count; k++)
                {
                    cgltf_animation_channel *channel = &anim->channels[k];
                    if(channel->target_node == node)
                    {
                        glTF->animations[j].channels[k].node_id = i;
                    }
                }
            }

            glTF->nodes[i].local_matrix = getLocalTransform(node);
            glTF->nodes[i].global_matrix = getFullTransform(node);

            iter++;
        }
        else
            glTF->nodes[i].isModel = false;
    }

    glTF->num_meshes = iter;

}

void DefaultglTFUpdate(ModelObject3D *mo)
{
    for(int i=0; i < mo->num_models;i++)
    {
        anim_array *anim = mo->glTF->animations > 0 ? &mo->glTF->animations[0] : NULL;

        if (anim) {
            mo->glTF->anim_time += 0.001f;

            update_frame(anim, mo->glTF, mo->models[i].id_node, mo->glTF->anim_time);
        }


        if(mo->models[i].graphObj.local.descriptors == NULL)
            return;

        Camera3D* cam = (Camera3D*) cam3D;
        void* data;

        ModelBuffer3D mbo = {};
        vec3 cameraUp = {0.0f,1.0f, 0.0f};

        mo->models[i].transform.model = mo->glTF->nodes[mo->models[i].id_node].global_matrix;

        mbo.model =  mo->models[i].transform.model;
        mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
        mbo.proj = m4_perspective(45.0f, 0.01f, 1000.0f);
        mbo.proj.m[1][1] *= -1;

        vkMapMemory(device, mo->models[i].graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
        memcpy(data, &mbo, sizeof(mbo));
        vkUnmapMemory(device, mo->models[i].graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

        LightBuffer3D lbo = {};
        lbo.lights[0].position.x = 0;
        lbo.lights[0].position.y = 0;
        lbo.lights[0].position.z = 9.5f;
        lbo.lights[0].color.x = 0.0f;
        lbo.lights[0].color.y = 0.0f;
        lbo.lights[0].color.z = 0.0f;

        lbo.size = 0;

        vkMapMemory(device, mo->models[i].graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(lbo), 0, &data);
        memcpy(data, &lbo, sizeof(lbo));
        vkUnmapMemory(device, mo->models[i].graphObj.local.descriptors[1].uniform->uniformBuffersMemory[imageIndex]);
    }
}

void Load3DglTFModel(void *ptr, char *ascii, char *binary, DrawParam dParam){

    ModelObject3D *mo = ptr;

    GameObjectSetUpdateFunc(mo, (void *)DefaultglTFUpdate);
    GameObjectSetDrawFunc(mo, (void *)ModelDefaultDraw);
    GameObjectSetCleanFunc(mo, (void *)ModelClean);
    GameObjectSetRecreateFunc(mo, (void *)ModelRecreate);
    GameObjectSetDestroyFunc(mo, (void *)ModelDestroy);

    mo->glTF = (glTFStruct *) calloc(1, sizeof(glTFStruct));

    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, ascii, &data);
    if (result == cgltf_result_success)
    {
        result = cgltf_load_buffers(&options, data, binary);

        if (result == cgltf_result_success)
        {
            SetupMeshState(mo->glTF, data);

            mo->models = (ModelStruct *) calloc(mo->glTF->num_meshes, sizeof(ModelStruct));
            mo->num_models = mo->glTF->num_meshes;

            int iter = 0;
            for(int i=0; i < mo->glTF->num_nodes;i++)
            {

                gltf_node *node = &mo->glTF->nodes[i];

                if(node->isModel)
                {
                    mo->models[iter].id_node = node->id_node;
                    mo->models[iter].graphObj.local.descriptors = (ShaderBuffer *) calloc(0, sizeof(ShaderBuffer));

                    Transform3DInit(&mo->models[iter].transform);
                    GraphicsObject3DInit(&mo->models[iter].graphObj);

                    mo->models[iter].graphObj.gItems.perspective = true;

                    GraphicsObject3DSetVertex(&mo->models[iter].graphObj, node->mesh->verts, node->mesh->num_verts, node->mesh->indices, node->mesh->num_indices);

                    if(node->mesh->num_verts > 0){
                        createVertexBuffer3D(&mo->models[iter].graphObj.shape.vParam);
                    }

                    if(node->mesh->num_indices > 0){
                        createIndexBuffer(&mo->models[iter].graphObj.shape.iParam);
                    }


                    ModelDefaultInit(&mo->models[iter], dParam);
                    iter++;
                }

            }
        }

        /* TODO make awesome stuff */
        cgltf_free(data);
    }
}
