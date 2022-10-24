#include "models.h"

#ifdef _WIN_BUILD_
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "buffers.h"
#include "texture.h"
#include "pipeline.h"

#include "ufbx.h"

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

    for(int i=0; i < mo->attrib.num_face_num_verts;i++){

        assert(mo->attrib.face_num_verts[i] % 3 ==
               0); /* assume all triangle faces. */

        for (int f = 0; f < (size_t)mo->attrib.face_num_verts[i] / 3; f++) {
              size_t k;
              float v[3][3];
              float n[3][3];
              float c[3];
              float uv[3][2];
              float len2;

              tinyobj_vertex_index_t idx0 = mo->attrib.faces[face_offset + 3 * f + 0];
              tinyobj_vertex_index_t idx1 = mo->attrib.faces[face_offset + 3 * f + 1];
              tinyobj_vertex_index_t idx2 = mo->attrib.faces[face_offset + 3 * f + 2];

              for (k = 0; k < 3; k++) {
                int f0 = idx0.v_idx;
                int f1 = idx1.v_idx;
                int f2 = idx2.v_idx;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                v[0][k] = mo->attrib.vertices[3 * (size_t)f0 + k];
                v[1][k] = mo->attrib.vertices[3 * (size_t)f1 + k];
                v[2][k] = mo->attrib.vertices[3 * (size_t)f2 + k];
              }

              {
              int f0 = idx0.vt_idx;
              int f1 = idx1.vt_idx;
              int f2 = idx2.vt_idx;
              assert(f0 >= 0);
              assert(f1 >= 0);
              assert(f2 >= 0);

              assert(f0 < mo->attrib.num_texcoords);
              assert(f1 < mo->attrib.num_texcoords);
              assert(f2 < mo->attrib.num_texcoords);

              uv[0][0] = mo->attrib.texcoords[2 * (size_t)f0 + 0];
              uv[1][0] = mo->attrib.texcoords[2 * (size_t)f1 + 0];
              uv[2][0] = mo->attrib.texcoords[2 * (size_t)f2 + 0];

              uv[0][1] = 1.0f - mo->attrib.texcoords[2 * (size_t)f0 + 1];
              uv[1][1] = 1.0f - mo->attrib.texcoords[2 * (size_t)f1 + 1];
              uv[2][1] = 1.0f - mo->attrib.texcoords[2 * (size_t)f2 + 1];
              }


              if (mo->attrib.num_normals > 0) {
                int f0 = idx0.vn_idx;
                int f1 = idx1.vn_idx;
                int f2 = idx2.vn_idx;
                if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
                  assert(f0 < (int)mo->attrib.num_normals);
                  assert(f1 < (int)mo->attrib.num_normals);
                  assert(f2 < (int)mo->attrib.num_normals);
                  for (k = 0; k < 3; k++) {
                    n[0][k] = mo->attrib.normals[3 * (size_t)f0 + k];
                    n[1][k] = mo->attrib.normals[3 * (size_t)f1 + k];
                    n[2][k] = mo->attrib.normals[3 * (size_t)f2 + k];
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

                if (mo->attrib.material_ids[i] >= 0) {
                  int matidx = mo->attrib.material_ids[i];
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
        face_offset += (size_t)mo->attrib.face_num_verts[i];
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

void DefaultModelInit(ModelObject3D * mo, DrawParam dParam){

    addUniformObject(&mo->go.graphObj.local, sizeof(ModelBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    addUniformObject(&mo->go.graphObj.local, sizeof(LightBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    if(strlen(dParam.filePath) != 0)
        addTexture(&mo->go.graphObj.local, dParam.filePath);

    GameObject3DCreateDrawItems(mo);

    PipelineSetting setting;

    PipelineSettingSetDefault(&mo->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_model_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_model_vert_spv_size);
        setting.fragShader = &_binary_shaders_model_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_model_frag_spv_size);
        setting.fromFile = 0;
    }
    else
        GraphicsObjectSetShadersPath(&mo->go.graphObj, dParam.vertShader, dParam.fragShader);

    GameObject3DAddSettingPipeline(&mo->go, setting);

    PipelineCreateGraphics(&mo->go.graphObj);
}

void Load3DObjModel(ModelObject3D * mo, char *filepath, DrawParam dParam){

    int vSize = 0, iSize = 0;

    GameObject3DInit(&mo->go);

    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    tinyobj_parse_obj(&mo->attrib, &mo->shapes, &mo->num_materials, &mo->materials, &mo->num_materials, filepath, (void *)get_file_data, NULL, flags);

    Vertex3D *verts = (Vertex3D *) calloc(mo->attrib.num_face_num_verts * 3, sizeof(Vertex3D));
    uint32_t *indxs = (uint32_t *) calloc(mo->attrib.num_face_num_verts * 3, sizeof(uint32_t));

    ParseSomeStruct(mo, verts);

    vSize = mo->attrib.num_face_num_verts * 3;
    iSize = mo->attrib.num_face_num_verts * 3;

    for(int i=0; i < iSize;i++)
        indxs[i] = i;

    GraphicsObject3DSetVertex(&mo->go.graphObj, verts, vSize, indxs, iSize);

    DefaultModelInit(mo, dParam);
}

void Load3DFBXModel(ModelObject3D * mo, char *filepath, DrawParam dParam)
{
    int vSize = 0, iSize = 0;

    GameObject3DInit(&mo->go);

    ufbx_error error; // Optional, pass NULL if you don't care about errors

    ufbx_scene *scene = ufbx_load_file(filepath, NULL, &error);

    if (!scene) {
        fprintf(stderr, "Failed to load: %s\n", error.description.data);
        exit(1);
    }

    // Use and inspect `scene`, it's just plain data!

    // Geometry is always stored in a consistent indexed format:
    ufbx_node *cube = ufbx_find_node(scene, "Bike");
    ufbx_mesh *mesh = cube->mesh;

    size_t num_tri_indices = mesh->max_face_triangles * 3;
    uint32_t *tri_indices = calloc(num_tri_indices, sizeof(uint32_t));

    for (size_t face_ix = 0; face_ix < mesh->num_faces; face_ix++) {
        ufbx_face face = mesh->faces.data[face_ix];
        size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

        for (size_t vertex_ix = 0; vertex_ix < num_tris * 3; vertex_ix++) {
            vSize++;
        }
    }

   iSize = vSize;

   Vertex3D *verts = (Vertex3D *) calloc(vSize, sizeof(Vertex3D));
   uint32_t *indxs = (uint32_t *) calloc(iSize, sizeof(uint32_t));

    int iter = 0;

    for (size_t face_ix = 0; face_ix < mesh->num_faces; face_ix++) {
        ufbx_face face = mesh->faces.data[face_ix];
        size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

        ufbx_vec2 default_uv = { 0 };

        // Iterate through every vertex of every triangle in the triangulated result
        for (size_t vi = 0; vi < num_tris * 3; vi++) {
            uint32_t ix = tri_indices[vi];
            Vertex3D *vert = &verts[iter];

            ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
            ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
            ufbx_vec2 uv = mesh->vertex_uv.exists ? ufbx_get_vertex_vec2(&mesh->vertex_uv, ix) : default_uv;

            vert->position = (vec3){pos.x, pos.y, pos.z};
            vert->normal = (vec3){normal.x, normal.y, normal.z};
            vert->texCoord = (vec2){uv.x, 1.0f - uv.y};
            vert->color = (vec3){0.1, 0.1, 0.1};

            // The skinning vertex stream is pre-calculated above so we just need to
            // copy the right one by the vertex index.
            /*if (skin) {
                skin_vertices[num_indices] = mesh_skin_vertices[mesh->vertex_indices.data[ix]];
            }*/

            indxs[iter] = iter;
            iter++;
       }

    }

    GraphicsObject3DSetVertex(&mo->go.graphObj, verts, vSize, indxs, iSize);

    DefaultModelInit(mo, dParam);
}

void DestroyOBJModel(ModelObject3D *model){
    tinyobj_attrib_free(&model->attrib);
    tinyobj_shapes_free(model->shapes,  model->num_shapes);
    tinyobj_materials_free(model->materials, model->num_materials);
    GameObject3DDestroy(&model->go);
}

void DestroyFBXModel(ModelObject3D *model){
    tinyobj_attrib_free(&model->attrib);
    tinyobj_shapes_free(model->shapes,  model->num_shapes);
    tinyobj_materials_free(model->materials, model->num_materials);
    GameObject3DDestroy(&model->go);
}
