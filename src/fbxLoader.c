#include "fbxLoader.h"

#include "e_camera.h"
#include "gameObject.h"
#include "graphicsObject.h"
#include "e_transform.h"
#include "e_buffer.h"
#include "e_texture.h"
#include "pipeline.h"

#include "e_math.h"

#include "ufbx.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

size_t clamp_sz(size_t a, size_t min_a, size_t max_a) { return e_min(e_max(a, min_a), max_a); }

mat4 ufbx_to_mat4(ufbx_matrix m) {
    return mat4_colmnsf(
        (float)m.m00, (float)m.m01, (float)m.m02, 0,
        (float)m.m10, (float)m.m11, (float)m.m12, 0,
        (float)m.m20, (float)m.m21, (float)m.m22, 0,
        (float)m.m03, (float)m.m13, (float)m.m23, 1
    );
}


void update_animation(ModelObject3D *vs, engine_fbx_anim *va, float time)
{
    FBXStruct *fbx = vs->obj;

    float frame_time = (time - va->time_begin) * va->framerate;
    size_t f0 = e_min((size_t)frame_time + 0, va->num_frames - 1);
    size_t f1 = e_min((size_t)frame_time + 1, va->num_frames - 1);
    float t = e_min(frame_time - (float)f0, 1.0f);

    for (size_t i = 0; i < fbx->num_nodes; i++) {
        engine_fbx_node *vn = &fbx->nodes[i];
        engine_fbx_node_anim *vna = &va->nodes[i];

        vec4 rot = vna->rot ? v4_lerp(vna->rot[f0], vna->rot[f1], t) : vna->const_rot;
        vec3 pos = vna->pos ? v3_lerp(vna->pos[f0], vna->pos[f1], t) : vna->const_pos;
        vec3 scale = vna->scale ? v3_lerp(vna->scale[f0], vna->scale[f1], t) : vna->const_scale;

        vn->node_to_parent = m4_transform_quaternion(pos, scale, rot);

    }

    for (size_t i = 0; i < fbx->num_blend_channels; i++) {
        engine_fbx_blend_channel *vbc = &fbx->blend_channels[i];
        engine_fbx_blend_channel_anim *vbca = &va->blend_channels[i];

        vbc->weight = vbca->weight ? lerp(vbca->weight[f0], vbca->weight[f1], t) : vbca->const_weight;
    }
}

void update_hierarchy(ModelObject3D* mo)
{
    FBXStruct *fbx = mo->obj;

    for (size_t i = 0; i < fbx->num_nodes; i++) {
         engine_fbx_node *vn = &fbx->nodes[i];

        // ufbx stores nodes in order where parent nodes always precede child nodes so we can
        // evaluate the transform hierarchy with a flat loop.
        if (vn->parent_index >= 0) {
            vn->node_to_world = m4_mult(fbx->nodes[vn->parent_index].node_to_world, vn->node_to_parent);
        } else {
            vn->node_to_world = vn->node_to_parent;
        }
        vn->geometry_to_world = m4_mult(vn->node_to_world, vn->geometry_to_node);
        //vn->normal_to_world = um_mat_transpose(um_mat_inverse(vn->geometry_to_world));
    }
}

void Load3DFBXNextFrame(ModelObject3D *mo)
{
  FBXStruct *fbx = mo->obj;

  if(fbx != NULL)
  {
      engine_fbx_anim *anim = fbx->num_animations > 0 ? &fbx->animations[0] : NULL;

      if (anim) {
          fbx->anim_time += 0.01f;
          if (fbx->anim_time >= anim->time_end) {
              fbx->anim_time -= anim->time_end - anim->time_begin;
          }
          update_animation(mo, anim, fbx->anim_time);
      }

      update_hierarchy(mo);
  }
}

void read_node(engine_fbx_node *vnode, ufbx_node *node)
{
    vnode->parent_index = node->parent ? node->parent->typed_id : -1;
    vnode->node_to_parent = ufbx_to_mat4(node->node_to_parent);
    vnode->node_to_world = ufbx_to_mat4(node->node_to_world);
    vnode->geometry_to_node = ufbx_to_mat4(node->geometry_to_node);
    vnode->geometry_to_world = ufbx_to_mat4(node->geometry_to_world);
    vnode->normal_to_world = ufbx_to_mat4(ufbx_matrix_for_normals(&node->geometry_to_world));
}

void read_mesh(engine_model_mesh *vBuffer, ufbx_mesh *mesh)
{
    size_t max_parts = 0;
    size_t max_triangles = 0;

    size_t num_tri_indices = mesh->max_face_triangles * 3;
    uint32_t *tri_indices = calloc(num_tri_indices, sizeof(uint32_t));

    for (size_t face_ix = 0; face_ix < mesh->num_faces; face_ix++) {
        ufbx_face face = mesh->faces.data[face_ix];
        max_triangles += ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);
    }

    vBuffer->num_indices =  vBuffer->num_verts = max_triangles * 3;

    vBuffer->verts = (ModelVertex3D *) calloc(  vBuffer->num_verts, sizeof(ModelVertex3D));
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


    int iter = 0;

    for (size_t face_ix = 0; face_ix < mesh->num_faces; face_ix++) {
        ufbx_face face = mesh->faces.data[face_ix];
        size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

        ufbx_vec2 default_uv = { 0 };

        // Iterate through every vertex of every triangle in the triangulated result
        for (size_t vi = 0; vi < num_tris * 3; vi++) {
            uint32_t ix = tri_indices[vi];
            ModelVertex3D *vert = &vBuffer->verts[iter];
            uint32_t *indx = vBuffer->indices;

            ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
            ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
            ufbx_vec2 uv = mesh->vertex_uv.exists ? ufbx_get_vertex_vec2(&mesh->vertex_uv, ix) : default_uv;

            vert->position = (vec3){pos.x, pos.y, pos.z};
            vert->normal = (vec3){normal.x, normal.y, normal.z};
            vert->texCoord = (vec2){uv.x, 1.0f - uv.y};

            // The skinning vertex stream is pre-calculated above so we just need to
            // copy the right one by the vertex index.
            /*if (skin) {
                skin_vertices[iter] = mesh_skin_vertices[mesh->vertex_indices.data[ix]];
            }*/

            indx[iter] = iter;
            iter++;
       }

    }

    free(tri_indices);
}

void read_blend_channel(engine_fbx_blend_channel *vchan, ufbx_blend_channel *chan)
{
    vchan->weight = (float)chan->weight;
}

void read_node_anim(engine_fbx_anim *va, engine_fbx_node_anim *vna, ufbx_anim_stack *stack, ufbx_node *node)
{
    vna->rot = calloc(va->num_frames, sizeof(vec4));
    vna->pos = calloc(va->num_frames, sizeof(vec3));
    vna->scale = calloc(va->num_frames, sizeof(vec3));

    bool const_rot = true, const_pos = true, const_scale = true;

    // Sample the node's transform evenly for the whole animation stack duration
    for (size_t i = 0; i < va->num_frames; i++) {
        double time = stack->time_begin + (double)i / va->framerate;

        ufbx_transform transform = ufbx_evaluate_transform(&stack->anim, node, time);
        vna->pos[i] = (vec3){transform.translation.x, transform.translation.y, transform.translation.z};
        vna->rot[i] = (vec4){transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w};

        /*vec4 axis = v4_to_v3_axis(vna->rot[i]);

        vna->rot[i] = v3_to_v4_quaternion((vec3){-axis.x, axis.z, -axis.y}, axis.w);*/
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

void read_blend_channel_anim(engine_fbx_anim *va, engine_fbx_blend_channel_anim *vbca, ufbx_anim_stack *stack, ufbx_blend_channel *chan)
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

void read_anim_stack(engine_fbx_anim *va, ufbx_anim_stack *stack, ufbx_scene *scene)
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
    va->nodes = calloc(scene->nodes.count, sizeof(engine_fbx_node_anim));
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node *node = scene->nodes.data[i];
        read_node_anim(va, &va->nodes[i], stack, node);
    }

    va->blend_channels = calloc(scene->blend_channels.count, sizeof(engine_fbx_blend_channel_anim));
    for (size_t i = 0; i < scene->blend_channels.count; i++) {
        ufbx_blend_channel *chan = scene->blend_channels.data[i];
        read_blend_channel_anim(va, &va->blend_channels[i], stack, chan);
    }
}

void read_scene(ModelObject3D *mo, ufbx_scene *scene)
{
    FBXStruct *fbx = mo->obj;

    fbx->num_nodes = scene->nodes.count;
    fbx->nodes = calloc(fbx->num_nodes, sizeof(engine_fbx_node));
    for (size_t i = 0; i < fbx->num_nodes; i++) {
        read_node(&fbx->nodes[i], scene->nodes.data[i]);
    }

    fbx->num_meshes = scene->meshes.count;
    fbx->meshes = (engine_model_mesh *)calloc(fbx->num_meshes, sizeof(engine_model_mesh));
    for (size_t i = 0; i < fbx->num_meshes; i++) {
        read_mesh(&fbx->meshes[i], scene->meshes.data[i]);
    }

    fbx->num_blend_channels = scene->blend_channels.count;
    fbx->blend_channels = calloc(fbx->num_blend_channels, sizeof(engine_fbx_blend_channel));
    for (size_t i = 0; i < fbx->num_blend_channels; i++) {
        read_blend_channel(&fbx->blend_channels[i], scene->blend_channels.data[i]);
    }

    fbx->num_animations = scene->anim_stacks.count;
    fbx->animations = calloc(fbx->num_animations, sizeof(engine_fbx_anim));
    for (size_t i = 0; i < fbx->num_animations; i++) {
        read_anim_stack(&fbx->animations[i], scene->anim_stacks.data[i], scene);
    }
}

// Не корректно
void DefaultFBXUpdate(ModelObject3D *mo)
{
  FBXStruct *fbx = mo->obj;

  for(int i=0; i < mo->num_draw_nodes;i++)
  {
      for(int j=0;j < mo->nodes[i].num_models;j++)
      {
          for(int l=0; l < mo->nodes[i].models[j].graphObj.blueprints.num_blue_print_packs; l++)
          {
              BluePrintPack *pack = &mo->nodes[i].models[j].graphObj.blueprints.blue_print_packs[l];

              if(pack->render_point == current_render)
              {

                  /*if(pack->descriptors == NULL)
                      return;

                  Camera3D* cam = (Camera3D*) cam3D;
                  void* data;

                  ModelBuffer3D mbo = {};
                  vec3 cameraUp = {0.0f,1.0f, 0.0f};

                  int node_id = fbx->meshes[i].instance_node_indices[0];

                  mo->nodes[i].model = fbx->nodes[node_id].geometry_to_world;//

                  mbo.model = mat4_mult_transform(mo->nodes[i].model, m4_transform(mo->transform.position, mo->transform.scale, mo->transform.rotation));
                  mbo.view = m4_look_at(cam->position, v3_add(cam->position, cam->rotation), cameraUp);
                  mbo.proj = edenMat;//m4_perspective(cam->view_angle, cam->view_near, cam->view_distance);
                  mbo.proj.m[1][1] *= -1;

                  vkMapMemory(e_device, pack->descriptors[0].uniform.uniformBuffersMemory[imageIndex], 0, sizeof(mbo), 0, &data);
                  memcpy(data, &mbo, sizeof(mbo));
                  vkUnmapMemory(e_device, pack->descriptors[0].uniform.uniformBuffersMemory[imageIndex]);

                  InvMatrixsBuffer imb = {};
                  memset(&imb, 0, sizeof(InvMatrixsBuffer));

                  vkMapMemory(e_device, pack->descriptors[1].uniform.uniformBuffersMemory[imageIndex], 0, sizeof(InvMatrixsBuffer), 0, &data);
                  memcpy(data, &imb, sizeof(InvMatrixsBuffer));
                  vkUnmapMemory(e_device, pack->descriptors[1].uniform.uniformBuffersMemory[imageIndex]);

                  LightBuffer3D lbo = {};
                  lbo.lights[0].position.x = 0;
                  lbo.lights[0].position.y = 0;
                  lbo.lights[0].position.z = 9.5f;

                  lbo.status.num_points = 0;
                  lbo.status.num_spots = 0;

                  vkMapMemory(e_device, pack->descriptors[2].uniform.uniformBuffersMemory[imageIndex], 0, sizeof(lbo), 0, &data);
                  memcpy(data, &lbo, sizeof(lbo));
                  vkUnmapMemory(e_device, pack->descriptors[2].uniform.uniformBuffersMemory[imageIndex]);*/

              }
          }


      }
  }
}

void ModelFBXDestroy(ModelObject3D *mo){

    FBXStruct *fbx = mo->obj;

    for(int i=0; i < mo->num_draw_nodes;i++)
    {
        free(fbx->meshes[i].verts);
        free(fbx->meshes[i].indices);

        for(int j=0;j < mo->nodes[i].num_models;j++)
        {
            GraphicsObjectDestroy(&mo->nodes[i].models[j].graphObj);
        }
    }

    free(fbx->meshes);

    free(fbx);

}

void Load3DFBXModel(ModelObject3D * mo, char *filepath, DrawParam *dParam)
{

  Transform3DInit(&mo->transform);

  GameObjectSetUpdateFunc(mo, (void *)DefaultFBXUpdate);
  GameObjectSetDrawFunc(mo, (void *)ModelDefaultDraw);
  GameObjectSetCleanFunc(mo, (void *)ModelClean);
  GameObjectSetRecreateFunc(mo, (void *)ModelRecreate);
  GameObjectSetDestroyFunc(mo, (void *)ModelFBXDestroy);

  int vSize = 0, iSize = 0;

  mo->obj = (FBXStruct *) calloc(1, sizeof(FBXStruct));

  ufbx_error error; // Optional, pass NULL if you don't care about errors

  ufbx_scene *scene = ufbx_load_file(filepath, NULL, &error);

  if (!scene) {
      fprintf(stderr, "Failed to load: %s\n", error.description.data);
      exit(1);
  }

  read_scene( mo, scene);

  FBXStruct *fbx = mo->obj;

  mo->nodes = (ModelNode *) calloc(fbx->num_meshes, sizeof(ModelNode));
  mo->num_draw_nodes = fbx->num_meshes;

  if(mo->num_draw_nodes > 0)
  {
      for(int i=0; i < fbx->num_meshes;i++)
      {

          mo->nodes[i].models = calloc(1, sizeof(ModelStruct));
          mo->nodes[i].num_models = 1;

          GraphicsObjectInit(&mo->nodes[i].models->graphObj, ENGINE_VERTEX_TYPE_MODEL_OBJECT);

          mo->nodes[i].models->graphObj.gItems.perspective = true;

          GraphicsObjectSetVertex(&mo->nodes[i].models->graphObj, fbx->meshes[i].verts, fbx->meshes[i].num_verts, sizeof(ModelVertex3D), fbx->meshes[i].indices, fbx->meshes[i].num_indices, sizeof(uint32_t));

          ModelDefaultInit(mo->nodes[i].models, dParam);

      }
  }
}
