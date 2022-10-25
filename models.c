#include "models.h"

#ifdef _WIN_BUILD_
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "buffers.h"
#include "texture.h"
#include "pipeline.h"

#include "camera.h"
#include "gameObject.h"
#include "graphicsObject.h"
#include "transform.h"
#include "buffers.h"

#include "e_math.h"

#include "ufbx.h"

size_t clamp_sz(size_t a, size_t min_a, size_t max_a) { return min(max(a, min_a), max_a); }

mat4 ufbx_to_mat4(ufbx_matrix m) {
    return mat4_f(
        (float)m.m00, (float)m.m01, (float)m.m02, (float)m.m03,
        (float)m.m10, (float)m.m11, (float)m.m12, (float)m.m13,
        (float)m.m20, (float)m.m21, (float)m.m22, (float)m.m23,
        0, 0, 0, 1
    );
}

static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
  float v10[3];
  float v20[3];
  float len2;

  v10[0] = v1[0] - v0[0];
  v10[1] = v1[1] - v0[1];
  v10[2] = v1[2] - v0[2];

  v20[0] = v2[0] - v0[0];
  v20[1] = v2[1] - v0[1];
  v20[2] = v2[2] - v0[2];

  N[0] = v20[1] * v10[2] - v20[2] * v10[1];
  N[1] = v20[2] * v10[0] - v20[0] * v10[2];
  N[2] = v20[0] * v10[1] - v20[1] * v10[0];

  len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
  if (len2 > 0.0f) {
    float len = (float)sqrt((double)len2);

    N[0] /= len;
    N[1] /= len;
  }
}

void ParseSomeStruct(ModelObject3D *mo, Vertex3D *vertexs){

    tinyobj_material_t* materials = NULL;
    size_t face_offset = 0;

    for(int i=0; i < mo->obj->attrib.num_face_num_verts;i++){

        assert(mo->obj->attrib.face_num_verts[i] % 3 ==
               0); /* assume all triangle faces. */

        for (int f = 0; f < (size_t)mo->obj->attrib.face_num_verts[i] / 3; f++) {
              size_t k;
              float v[3][3];
              float n[3][3];
              float c[3];
              float uv[3][2];
              float len2;

              tinyobj_vertex_index_t idx0 = mo->obj->attrib.faces[face_offset + 3 * f + 0];
              tinyobj_vertex_index_t idx1 = mo->obj->attrib.faces[face_offset + 3 * f + 1];
              tinyobj_vertex_index_t idx2 = mo->obj->attrib.faces[face_offset + 3 * f + 2];

              for (k = 0; k < 3; k++) {
                int f0 = idx0.v_idx;
                int f1 = idx1.v_idx;
                int f2 = idx2.v_idx;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                v[0][k] = mo->obj->attrib.vertices[3 * (size_t)f0 + k];
                v[1][k] = mo->obj->attrib.vertices[3 * (size_t)f1 + k];
                v[2][k] = mo->obj->attrib.vertices[3 * (size_t)f2 + k];
              }

              {
              int f0 = idx0.vt_idx;
              int f1 = idx1.vt_idx;
              int f2 = idx2.vt_idx;
              assert(f0 >= 0);
              assert(f1 >= 0);
              assert(f2 >= 0);

              assert(f0 < mo->obj->attrib.num_texcoords);
              assert(f1 < mo->obj->attrib.num_texcoords);
              assert(f2 < mo->obj->attrib.num_texcoords);

              uv[0][0] = mo->obj->attrib.texcoords[2 * (size_t)f0 + 0];
              uv[1][0] = mo->obj->attrib.texcoords[2 * (size_t)f1 + 0];
              uv[2][0] = mo->obj->attrib.texcoords[2 * (size_t)f2 + 0];

              uv[0][1] = 1.0f - mo->obj->attrib.texcoords[2 * (size_t)f0 + 1];
              uv[1][1] = 1.0f - mo->obj->attrib.texcoords[2 * (size_t)f1 + 1];
              uv[2][1] = 1.0f - mo->obj->attrib.texcoords[2 * (size_t)f2 + 1];
              }


              if (mo->obj->attrib.num_normals > 0) {
                int f0 = idx0.vn_idx;
                int f1 = idx1.vn_idx;
                int f2 = idx2.vn_idx;
                if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
                  assert(f0 < (int)mo->obj->attrib.num_normals);
                  assert(f1 < (int)mo->obj->attrib.num_normals);
                  assert(f2 < (int)mo->obj->attrib.num_normals);
                  for (k = 0; k < 3; k++) {
                    n[0][k] = mo->obj->attrib.normals[3 * (size_t)f0 + k];
                    n[1][k] = mo->obj->attrib.normals[3 * (size_t)f1 + k];
                    n[2][k] = mo->obj->attrib.normals[3 * (size_t)f2 + k];
                  }
                } else { /* normal index is not defined for this face */
                  /* compute geometric normal */
                  CalcNormal(n[0], v[0], v[1], v[2]);
                  n[1][0] = n[0][0];
                  n[1][1] = n[0][1];
                  n[1][2] = n[0][2];
                  n[2][0] = n[0][0];
                  n[2][1] = n[0][1];
                  n[2][2] = n[0][2];
                }
              } else {
                /* compute geometric normal */
                CalcNormal(n[0], v[0], v[1], v[2]);
                n[1][0] = n[0][0];
                n[1][1] = n[0][1];
                n[1][2] = n[0][2];
                n[2][0] = n[0][0];
                n[2][1] = n[0][1];
                n[2][2] = n[0][2];
              }

              for (k = 0; k < 3; k++) {
                vertexs[3 * i + k].position.x = v[k][0];
                vertexs[3 * i + k].position.y = v[k][1];
                vertexs[3 * i + k].position.z = v[k][2];
                vertexs[3 * i + k].texCoord.x = uv[k][0];
                vertexs[3 * i + k].texCoord.y = uv[k][1];
                vertexs[3 * i + k].normal.x = n[k][0];
                vertexs[3 * i + k].normal.y = n[k][1];
                vertexs[3 * i + k].normal.z = n[k][2];

                if (mo->obj->attrib.material_ids[i] >= 0) {
                  int matidx = mo->obj->attrib.material_ids[i];
                  vertexs[3 * i + k].color.x = materials[matidx].diffuse[0];
                  vertexs[3 * i + k].color.y = materials[matidx].diffuse[1];
                  vertexs[3 * i + k].color.z = materials[matidx].diffuse[2];
                }else {
                    /*vertexs[3 * i + k].color.x = n[k][0];
                    vertexs[3 * i + k].color.y = n[k][1];
                    vertexs[3 * i + k].color.z = n[k][2];*/
                }

                /**/

                /*
                c[0] = n[k][0];
                c[1] = n[k][1];
                c[2] = n[k][2];
                len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                if (len2 > 0.0f) {
                  float len = (float)sqrt((double)len2);

                  c[0] /= len;
                  c[1] /= len;
                  c[2] /= len;
                }

                vb[(3 * i + k) * stride + 6] = (c[0] * 0.5f + 0.5f);
                vb[(3 * i + k) * stride + 7] = (c[1] * 0.5f + 0.5f);
                vb[(3 * i + k) * stride + 8] = (c[2] * 0.5f + 0.5f);

                if (attrib.material_ids[i] >= 0) {
                      int matidx = attrib.material_ids[i];
                      vb[(3 * i + k) * stride + 9] = materials[matidx].diffuse[0];
                      vb[(3 * i + k) * stride + 10] = materials[matidx].diffuse[1];
                      vb[(3 * i + k) * stride + 11] = materials[matidx].diffuse[2];
                } else {
                      vb[(3 * i + k) * stride + 9] = vb[(3 * i + k) * stride + 6];
                      vb[(3 * i + k) * stride + 10] = vb[(3 * i + k) * stride + 7];
                      vb[(3 * i + k) * stride + 11] = vb[(3 * i + k) * stride + 8];
                }*/
              }
        }
        face_offset += (size_t)mo->obj->attrib.face_num_verts[i];
    }

    return;
}

char* mmap_file(size_t* len, const char* filename) {

    FILE* fd;
    int size;

    fd = fopen(filename, "r");
    if (fd == NULL) {
        printf("File Not Found!\n");
        return -1;
    }


    fseek(fd, 0L, SEEK_END);
    size = ftell(fd);

    char *buff = (char *)calloc(size, sizeof(char));

    fseek(fd, 0L, SEEK_SET);

    fread(buff, sizeof(char), size, fd);

    fclose(fd);

    *len = size;

    return buff;
}

static void get_file_data(void* ctx, const char* filename, const int is_mtl,
                          const char* obj_filename, char** data, size_t* len) {
  // NOTE: If you allocate the buffer with malloc(),
  // You can define your own memory management struct and pass it through `ctx`
  // to store the pointer and free memories at clean up stage(when you quit an
  // app)
  // This example uses mmap(), so no free() required.
  (void)ctx;

  if (!filename) {
    fprintf(stderr, "null filename\n");
    (*data) = NULL;
    (*len) = 0;
    return;
  }

  size_t data_len = 0;

  *data = mmap_file(&data_len, filename);
  (*len) = data_len;
}

void ModelFBXDefaultDraw(ModelObject3D* mo){

    for(int i=0; i < mo->num_models;i++)
    {
        for(int j=0; j < mo->models[i].graphObj.gItems.pipelineCount; j++){

            vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, mo->models[i].graphObj.gItems.graphicsPipeline[j]);

            PipelineSetting *settings = &mo->models[i].graphObj.gItems.settings[j];

            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

            VkBuffer vertexBuffers[] = {mo->models[i].graphObj.shape.vParam.vertexBuffer};
            VkDeviceSize offsets[] = {0};

            vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

            vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, mo->models[i].graphObj.gItems.pipelineLayout[j], 0, 1, &mo->models[i].graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

            switch(settings->drawType){
                case 0:
                    vkCmdBindIndexBuffer(commandBuffers[imageIndex], mo->models[i].graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(commandBuffers[imageIndex], mo->models[i].graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                    break;
                case 1:
                    vkCmdDraw(commandBuffers[imageIndex], mo->models[i].graphObj.shape.vParam.verticesSize, 1, 0, 0);
                    break;
            }

        }
    }
}

void update_animation(ModelObject3D *vs, engine_anim *va, float time)
{
    float frame_time = (time - va->time_begin) * va->framerate;
    size_t f0 = min((size_t)frame_time + 0, va->num_frames - 1);
    size_t f1 = min((size_t)frame_time + 1, va->num_frames - 1);
    float t = min(frame_time - (float)f0, 1.0f);

    for (size_t i = 0; i < vs->fbx->num_nodes; i++) {
        engine_node *vn = &vs->fbx->nodes[i];
        engine_node_anim *vna = &va->nodes[i];

        vec3 p1 = {-vna->pos[f0].x, vna->pos[f0].z, -vna->pos[f0].y};
        vec3 p2 = {-vna->pos[f1].x, vna->pos[f1].z, -vna->pos[f1].y};

        vec4 rot = vna->rot ? v4_lerp(vna->rot[f0], vna->rot[f1], t) : vna->const_rot;
        vec3 pos = vna->pos ? v3_lerp(p1, p2, t) : vna->const_pos;
        vec3 scale = vna->scale ? v3_lerp(vna->scale[f0], vna->scale[f1], t) : vna->const_scale;

        vn->node_to_parent = m4_translate(m4_mult(m4_scale_mat(scale), m4_rotation_matrix((vec3){rot.x, rot.y,rot.z})), pos);
    }

    for (size_t i = 0; i < vs->fbx->num_blend_channels; i++) {
        engine_blend_channel *vbc = &vs->fbx->blend_channels[i];
        engine_blend_channel_anim *vbca = &va->blend_channels[i];

        vbc->weight = vbca->weight ? lerp(vbca->weight[f0], vbca->weight[f1], t) : vbca->const_weight;
    }
}


void update_hierarchy(ModelObject3D* mo)
{
    for (size_t i = 0; i < mo->fbx->num_nodes; i++) {
         engine_node *vn = &mo->fbx->nodes[i];

        // ufbx stores nodes in order where parent nodes always precede child nodes so we can
        // evaluate the transform hierarchy with a flat loop.
        if (vn->parent_index >= 0) {
            vn->node_to_world = m4_mult(mo->fbx->nodes[vn->parent_index].node_to_world, vn->node_to_parent);
        } else {
            vn->node_to_world = vn->node_to_parent;
        }
        vn->geometry_to_world = m4_mult(vn->node_to_world, vn->geometry_to_node);
        //vn->normal_to_world = um_mat_transpose(um_mat_inverse(vn->geometry_to_world));
    }
}

void ModelFBXDefaultUpdate(ModelObject3D* mo){

    engine_anim *anim = mo->fbx->num_animations > 0 ? &mo->fbx->animations[0] : NULL;

    if (anim) {
        mo->fbx->anim_time += 0.01f;
        if (mo->fbx->anim_time >= anim->time_end) {
            mo->fbx->anim_time -= anim->time_end - anim->time_begin;
        }
        update_animation(mo, anim, mo->fbx->anim_time);
    }

    update_hierarchy(mo);

    for(int i=0; i < mo->num_models;i++)
    {
        if(mo->models[i].graphObj.local.descriptors == NULL)
            return;

        Camera3D* cam = (Camera3D*) cam3D;
        void* data;

        ModelBuffer3D mbo = {};
        vec3 cameraUp = {0.0f,1.0f, 0.0f};
        mat4 edenMat = mat4_f(1,0,0,0,
                                                0,1,0,0,
                                                0,0,1,0,
                                                0,0,0,1);

        int node_id = mo->fbx->meshes[i].instance_node_indices[0];

        if(node_id){

            mat4 rotation_matrix = m4_m4_rotation_matrix(mo->fbx->nodes[node_id].geometry_to_world, mo->models[i].transform.rotation);

            mo->models[i].transform.model = m4_translate(m4_mult(m4_scale_mat(mo->models[i].transform.scale), rotation_matrix), mo->models[i].transform.position);
        }else{
            mo->models[i].transform.model = m4_translate(m4_mult(m4_scale_mat(mo->models[i].transform.scale), m4_rotation_matrix(mo->models[i].transform.rotation)), mo->models[i].transform.position);
        }
        mbo.model = mo->models[i].transform.model;
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

void ModelFBXClean(ModelObject3D* mo){

    for(int i=0; i < mo->num_models;i++)
    {
        GraphicsObjectClean(&mo->models[i].graphObj);
    }
}

void ModelFBXRecreate(ModelObject3D* mo){

    for(int i=0; i < mo->num_models;i++)
    {
        PipelineSetting *settings = (PipelineSetting *)mo->models[i].graphObj.gItems.settings;

        for(int i=0; i < mo->models[i].graphObj.gItems.settingsCount;i++)
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

        recreateUniformBuffers(&mo->models[i].graphObj.local);
        GameObject3DCreateDrawItems(&mo->models[i].graphObj);
        PipelineCreateGraphics(&mo->models[i].graphObj);
    }
}

void ModelFBXDestroy(ModelObject3D* mo){

    for(int i=0; i < mo->num_models;i++)
    {
         GraphicsObjectDestroy(&mo->models[i].graphObj);
    }
}

void ModelAddSettingPipeline(ModelStruct* model, PipelineSetting setting){
    PipelineSetting* settings;

    model->graphObj.gItems.settingsCount++;
    model->graphObj.gItems.settings = realloc(model->graphObj.gItems.settings, model->graphObj.gItems.settingsCount * sizeof(PipelineSetting));

    settings = (PipelineSetting *) model->graphObj.gItems.settings;

    memcpy(&settings[model->graphObj.gItems.settingsCount - 1], &setting, sizeof(PipelineSetting));
}

void ModelDefaultInit(ModelStruct *model, DrawParam dParam){

    addUniformObject(&model->graphObj.local, sizeof(ModelBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    addUniformObject(&model->graphObj.local, sizeof(LightBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    if(strlen(dParam.filePath) != 0)
        addTexture(&model->graphObj.local, dParam.filePath);

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
}

void Load3DObjModel(ModelObject3D * mo, char *filepath, DrawParam dParam){

    GameObjectSetUpdateFunc(mo, (void *)ModelFBXDefaultUpdate);
    GameObjectSetDrawFunc(mo, (void *)ModelFBXDefaultDraw);
    GameObjectSetCleanFunc(mo, (void *)ModelFBXClean);
    GameObjectSetRecreateFunc(mo, (void *)ModelFBXRecreate);
    GameObjectSetDestroyFunc(mo, (void *)ModelFBXDestroy);

    mo->obj = (OBJStruct *) calloc(1, sizeof(OBJStruct));
    mo->num_models = 1;

    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;

    tinyobj_parse_obj(&mo->obj->attrib, &mo->obj->shapes, &mo->obj->num_materials, &mo->obj->materials, &mo->obj->num_materials, filepath, (void *)get_file_data, NULL, flags);

    mo->models = calloc(1, sizeof(ModelStruct));

    mo->models->graphObj.local.descriptors = (ShaderBuffer *) calloc(0, sizeof(ShaderBuffer));

    Transform3DInit(&mo->models->transform);
    GraphicsObject3DInit(&mo->models->graphObj);

    mo->models->graphObj.gItems.perspective = true;

    mo->models->graphObj.shape.vParam.vertices = (Vertex3D *) calloc(mo->obj->attrib.num_face_num_verts * 3, sizeof(Vertex3D));
    mo->models->graphObj.shape.iParam.indices = (uint32_t *) calloc(mo->obj->attrib.num_face_num_verts * 3, sizeof(uint32_t));

    ParseSomeStruct(mo, mo->models[0].graphObj.shape.vParam.vertices );

    mo->models->graphObj.shape.vParam.verticesSize = mo->obj->attrib.num_face_num_verts * 3;
    mo->models->graphObj.shape.iParam.indexesSize = mo->obj->attrib.num_face_num_verts * 3;

    for(int i=0; i < mo->models->graphObj.shape.iParam.indexesSize;i++)
        mo->models->graphObj.shape.iParam.indices[i] = i;

    if(mo->models->graphObj.shape.vParam.verticesSize > 0){
        createVertexBuffer3D(&mo->models->graphObj.shape.vParam);
    }

    if(mo->models->graphObj.shape.iParam.indexesSize > 0){
        createIndexBuffer(&mo->models->graphObj.shape.iParam);
    }

    ModelDefaultInit(mo->models, dParam);
}

void read_node(engine_node *vnode, ufbx_node *node)
{
    vnode->parent_index = node->parent ? node->parent->typed_id : -1;
    vnode->node_to_parent = ufbx_to_mat4(node->node_to_parent);
    vnode->node_to_world = ufbx_to_mat4(node->node_to_world);
    vnode->geometry_to_node = ufbx_to_mat4(node->geometry_to_node);
    vnode->geometry_to_world = ufbx_to_mat4(node->geometry_to_world);
    vnode->normal_to_world = ufbx_to_mat4(ufbx_matrix_for_normals(&node->geometry_to_world));
}

void read_mesh(engine_mesh *vBuffer, ufbx_mesh *mesh)
{
    size_t max_parts = 0;
    size_t max_triangles = 0;

    for (size_t pi = 0; pi < mesh->materials.count; pi++) {
        ufbx_mesh_material *mesh_mat = &mesh->materials.data[pi];
        if (mesh_mat->num_triangles == 0) continue;
        max_parts += 1;
        max_triangles = max(max_triangles, mesh_mat->num_triangles);
    }

    vBuffer->num_indices =  vBuffer->num_verts = max_triangles * 3;

    size_t num_tri_indices = mesh->max_face_triangles * 3;
    uint32_t *tri_indices = calloc(num_tri_indices, sizeof(uint32_t));
    vBuffer->verts = (Vertex3D *) calloc(  vBuffer->num_verts, sizeof(Vertex3D));
    //skin_vertex *skin_vertices = calloc(max_triangles * 3, sizeof(skin_vertex));
    //skin_vertex *mesh_skin_vertices = calloc(mesh->num_vertices, sizeof(skin_vertex));
    vBuffer->indices = (uint32_t *) calloc( vBuffer->num_indices, sizeof(uint32_t));

    // In FBX files a single mesh can be instanced by multiple nodes. ufbx handles the connection
    // in two ways: (1) `ufbx_node.mesh/light/camera/etc` contains pointer to the data "attribute"
    // that node uses and (2) each element that can be connected to a node contains a list of
    // `ufbx_node*` instances eg. `ufbx_mesh.instances`.
    vBuffer->num_instances = mesh->instances.count;
    vBuffer->instance_node_indices = calloc(mesh->instances.count, sizeof(int32_t));

    for (size_t i = 0; i < mesh->instances.count; i++) {
        vBuffer->instance_node_indices[i] = (int32_t)mesh->instances.data[i]->typed_id;
    }

    size_t num_bones = 0;
    ufbx_skin_deformer *skin = NULL;

    /*if (mesh->skin_deformers.count > 0) {
        vBuffer->skinned = true;

        // Having multiple skin deformers attached at once is exceedingly rare so we can just
        // pick the first one without having to worry too much about it.
        skin = mesh->skin_deformers.data[0];

        // NOTE: A proper implementation would split meshes with too many bones to chunks but
        // for simplicity we're going to just pick the first `MAX_BONES` ones.
        for (size_t ci = 0; ci < skin->clusters.count; ci++) {
            ufbx_skin_cluster *cluster = skin->clusters.data[ci];
            if (num_bones < MAX_BONES) {
                vBuffer->bone_indices[num_bones] = (int32_t)cluster->bone_node->typed_id;
                vBuffer->bone_matrices[num_bones] = ufbx_to_mat4(cluster->geometry_to_bone);
                num_bones++;
            }
        }
        vBuffer->num_bones = num_bones;

        // Pre-calculate the skinned vertex bones/weights for each vertex as they will probably
        // be shared by multiple indices.
        for (size_t vi = 0; vi < mesh->num_vertices; vi++) {
            size_t num_weights = 0;
            float total_weight = 0.0f;
            float weights[4] = { 0.0f };
            uint8_t clusters[4] = { 0 };

            // `ufbx_skin_vertex` contains the offset and number of weights that deform the vertex
            // in a descending weight order so we can pick the first N weights to use and get a
            // reasonable approximation of the skinning.
            ufbx_skin_vertex vertex_weights = skin->vertices.data[vi];
            for (size_t wi = 0; wi < vertex_weights.num_weights; wi++) {
                if (num_weights >= 4) break;
                ufbx_skin_weight weight = skin->weights.data[vertex_weights.weight_begin + wi];

                // Since we only support a fixed amount of bones up to `MAX_BONES` and we take the
                // first N ones we need to ignore weights with too high `cluster_index`.
                if (weight.cluster_index < MAX_BONES) {
                    total_weight += (float)weight.weight;
                    clusters[num_weights] = (uint8_t)weight.cluster_index;
                    weights[num_weights] = (float)weight.weight;
                    num_weights++;
                }
            }

            // Normalize and quantize the weights to 8 bits. We need to be a bit careful to make
            // sure the _quantized_ sum is normalized ie. all 8-bit values sum to 255.
            if (total_weight > 0.0f) {
                skin_vertex *skin_vert = &mesh_skin_vertices[vi];
                uint32_t quantized_sum = 0;
                for (size_t i = 0; i < 4; i++) {
                    uint8_t quantized_weight = (uint8_t)((float)weights[i] / total_weight * 255.0f);
                    quantized_sum += quantized_weight;
                    skin_vert->bone_index[i] = clusters[i];
                    skin_vert->bone_weight[i] = quantized_weight;
                }
                skin_vert->bone_weight[0] += 255 - quantized_sum;
            }
        }
    }*/
    int iter = 0;

    for (size_t face_ix = 0; face_ix < mesh->num_faces; face_ix++) {
        ufbx_face face = mesh->faces.data[face_ix];
        size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

        ufbx_vec2 default_uv = { 0 };

        // Iterate through every vertex of every triangle in the triangulated result
        for (size_t vi = 0; vi < num_tris * 3; vi++) {
            uint32_t ix = tri_indices[vi];
            Vertex3D *vert = &vBuffer->verts[iter];

            ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
            ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
            ufbx_vec2 uv = mesh->vertex_uv.exists ? ufbx_get_vertex_vec2(&mesh->vertex_uv, ix) : default_uv;

            vert->position = (vec3){-pos.x, pos.z, -pos.y};
            vert->normal = (vec3){normal.x, normal.z, normal.y};
            vert->texCoord = (vec2){uv.x, 1.0f - uv.y};
            vert->color = (vec3){1.0f, 1.0f, 1.0f};

            // The skinning vertex stream is pre-calculated above so we just need to
            // copy the right one by the vertex index.
            /*if (skin) {
                skin_vertices[iter] = mesh_skin_vertices[mesh->vertex_indices.data[ix]];
            }*/

            vBuffer->indices[iter] =vBuffer->num_indices - iter - 1;
            iter++;
       }

    }

   // free(mesh_skin_vertices);
}

void read_blend_channel(engine_blend_channel *vchan, ufbx_blend_channel *chan)
{
    vchan->weight = (float)chan->weight;
}

void read_node_anim(engine_anim *va, engine_node_anim *vna, ufbx_anim_stack *stack, ufbx_node *node)
{
    vna->rot = calloc(va->num_frames, sizeof(vec4));
    vna->pos = calloc(va->num_frames, sizeof(vec3));
    vna->scale = calloc(va->num_frames, sizeof(vec3));

    bool const_rot = true, const_pos = true, const_scale = true;

    // Sample the node's transform evenly for the whole animation stack duration
    for (size_t i = 0; i < va->num_frames; i++) {
        double time = stack->time_begin + (double)i / va->framerate;

        ufbx_transform transform = ufbx_evaluate_transform(&stack->anim, node, time);
        vna->rot[i] = (vec4){transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w};
        vna->pos[i] = (vec3){transform.translation.x, transform.translation.y, transform.translation.z};
        vna->scale[i] = (vec3){transform.scale.x, transform.scale.y, transform.scale.z};

        if (i > 0) {
            // Negated quaternions are equivalent, but interpolating between ones of different
            // polarity takes a the longer path, so flip the quaternion if necessary.
            if (v4_dot(vna->rot[i], vna->rot[i - 1]) < 0.0f) {
                vna->rot[i] = v4_neg(vna->rot[i]);
            }

            // Keep track of which channels are constant for the whole animation as an optimization
            if (!v4_equal(vna->rot[i - 1], vna->rot[i])) const_rot = false;
            if (!v3_equal(vna->pos[i - 1], vna->pos[i])) const_pos = false;
            if (!v3_equal(vna->scale[i - 1], vna->scale[i])) const_scale = false;
        }
    }

    /*if (const_rot) { vna->const_rot = vna->rot[0]; free(vna->rot); vna->rot = NULL; }
    if (const_pos) { vna->const_pos = vna->pos[0]; free(vna->pos); vna->pos = NULL; }
    if (const_scale) { vna->const_scale = vna->scale[0]; free(vna->scale); vna->scale = NULL; }*/
}

void read_blend_channel_anim(engine_anim *va, engine_blend_channel_anim *vbca, ufbx_anim_stack *stack, ufbx_blend_channel *chan)
{
    vbca->weight = calloc( va->num_frames, sizeof(float));

    bool const_weight = true;

    // Sample the blend weight evenly for the whole animation stack duration
    for (size_t i = 0; i < va->num_frames; i++) {
        double time = stack->time_begin + (double)i / va->framerate;

        ufbx_real weight = ufbx_evaluate_blend_weight(&stack->anim, chan, time);
        vbca->weight[i] = (float)weight;

        // Keep track of which channels are constant for the whole animation as an optimization
        if (i > 0) {
            if (vbca->weight[i - 1] != vbca->weight[i]) const_weight = false;
        }
    }

    if (const_weight) { vbca->const_weight = vbca->weight[0]; free(vbca->weight); vbca->weight = NULL; }
}

void read_anim_stack(engine_anim *va, ufbx_anim_stack *stack, ufbx_scene *scene)
{
    const float target_framerate = 30.0f;
    const size_t max_frames = 4096;

    // Sample the animation evenly at `target_framerate` if possible while limiting the maximum
    // number of frames to `max_frames` by potentially dropping FPS.
    float duration = (float)stack->time_end - (float)stack->time_begin;
    size_t num_frames = clamp_sz((size_t)(duration * target_framerate), 2, max_frames);
    float framerate = (float)(num_frames - 1) / duration;

    va->name = calloc(stack->name.length + 1, sizeof(char));
    memcpy(va->name, stack->name.data, stack->name.length + 1);
    va->time_begin = (float)stack->time_begin;
    va->time_end = (float)stack->time_end;
    va->framerate = framerate;
    va->num_frames = num_frames;

    // Sample the animations of all nodes and blend channels in the stack
    va->nodes = calloc(scene->nodes.count, sizeof(engine_node_anim));
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node *node = scene->nodes.data[i];
        read_node_anim(va, &va->nodes[i], stack, node);
    }

    va->blend_channels = calloc(scene->blend_channels.count, sizeof(engine_blend_channel_anim));
    for (size_t i = 0; i < scene->blend_channels.count; i++) {
        ufbx_blend_channel *chan = scene->blend_channels.data[i];
        read_blend_channel_anim(va, &va->blend_channels[i], stack, chan);
    }
}

void read_scene(ModelObject3D *mo, ufbx_scene *scene)
{
    mo->fbx->num_nodes = scene->nodes.count;
    mo->fbx->nodes = calloc(mo->fbx->num_nodes, sizeof(engine_node));
    for (size_t i = 0; i < mo->fbx->num_nodes; i++) {
        read_node(&mo->fbx->nodes[i], scene->nodes.data[i]);
    }

    mo->fbx->num_meshes = scene->meshes.count;
    mo->fbx->meshes = (engine_mesh *)calloc(mo->fbx->num_meshes, sizeof(engine_mesh));
    for (size_t i = 0; i < mo->fbx->num_meshes; i++) {
        read_mesh(&mo->fbx->meshes[i], scene->meshes.data[i]);
    }

    mo->fbx->num_blend_channels = scene->blend_channels.count;
    mo->fbx->blend_channels = calloc(mo->fbx->num_blend_channels, sizeof(engine_blend_channel));
    for (size_t i = 0; i < mo->fbx->num_blend_channels; i++) {
        read_blend_channel(&mo->fbx->blend_channels[i], scene->blend_channels.data[i]);
    }

    mo->fbx->num_animations = scene->anim_stacks.count;
    mo->fbx->animations = calloc(mo->fbx->num_animations, sizeof(engine_anim));
    for (size_t i = 0; i < mo->fbx->num_animations; i++) {
        read_anim_stack(&mo->fbx->animations[i], scene->anim_stacks.data[i], scene);
    }
}

void Load3DFBXModel(ModelObject3D * mo, char *filepath, DrawParam dParam)
{
    GameObjectSetUpdateFunc(mo, (void *)ModelFBXDefaultUpdate);
    GameObjectSetDrawFunc(mo, (void *)ModelFBXDefaultDraw);
    GameObjectSetCleanFunc(mo, (void *)ModelFBXClean);
    GameObjectSetRecreateFunc(mo, (void *)ModelFBXRecreate);
    GameObjectSetDestroyFunc(mo, (void *)ModelFBXDestroy);

    int vSize = 0, iSize = 0;

    mo->fbx = (FBXStruct *) calloc(1, sizeof(FBXStruct));

    ufbx_error error; // Optional, pass NULL if you don't care about errors

    ufbx_scene *scene = ufbx_load_file(filepath, NULL, &error);

    if (!scene) {
        fprintf(stderr, "Failed to load: %s\n", error.description.data);
        exit(1);
    }

    read_scene( mo, scene);

    mo->models = (ModelStruct *) calloc(mo->fbx->num_meshes, sizeof(ModelStruct));
    mo->num_models = mo->fbx->num_meshes;

    for(int i=0; i < mo->fbx->num_meshes;i++)
    {

        mo->models[i].graphObj.local.descriptors = (ShaderBuffer *) calloc(0, sizeof(ShaderBuffer));

        Transform3DInit(&mo->models[i].transform);
        GraphicsObject3DInit(&mo->models[i].graphObj);

        mo->models[i].graphObj.gItems.perspective = true;

        GraphicsObject3DSetVertex(&mo->models[i].graphObj, mo->fbx->meshes[i].verts, mo->fbx->meshes[i].num_verts, mo->fbx->meshes[i].indices, mo->fbx->meshes[i].num_indices);

        if(mo->fbx->meshes[i].num_verts > 0){
            createVertexBuffer3D(&mo->models[i].graphObj.shape.vParam);
        }

        if(mo->fbx->meshes[i].num_indices > 0){
            createIndexBuffer(&mo->models[i].graphObj.shape.iParam);
        }

        ModelDefaultInit(&mo->models[i], dParam);

    }
}

void DestroyOBJModel(ModelObject3D *model){
    tinyobj_attrib_free(&model->obj->attrib);
    tinyobj_shapes_free(model->obj->shapes,  model->obj->num_shapes);
    tinyobj_materials_free(model->obj->materials, model->obj->num_materials);
    GraphicsObjectDestroy(&model->models[0].graphObj);
}

void DestroyFBXModel(ModelObject3D *mo){

    for(int i=0; i < mo->fbx->num_meshes;i++)
    {
        free(mo->fbx->meshes[i].verts);
        free(mo->fbx->meshes[i].indices);

        GraphicsObjectDestroy(&mo->models[i].graphObj);
    }

    free(mo->fbx->meshes);

    free(mo->fbx);

}
