#include "objLoader.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader.h"

#include "e_math.h"

typedef struct{
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes;
    tinyobj_material_t* materials;
    uint64_t num_shapes;
    uint64_t num_materials;
    char* warn;
    char* err;
} OBJStruct;

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

void ParseSomeStruct(ModelObject3D *mo, ModelVertex3D *vertexs){

    tinyobj_material_t* materials = NULL;
    size_t face_offset = 0;

    OBJStruct *obj = mo->obj;

    for(int i=0; i < obj->attrib.num_face_num_verts;i++){

        assert(obj->attrib.face_num_verts[i] % 3 ==
               0); /* assume all triangle faces. */

        for (int f = 0; f < (size_t)obj->attrib.face_num_verts[i] / 3; f++) {
              size_t k;
              float v[3][3];
              float n[3][3];
              float c[3];
              float uv[3][2];
              float len2;

              tinyobj_vertex_index_t idx0 = obj->attrib.faces[face_offset + 3 * f + 0];
              tinyobj_vertex_index_t idx1 = obj->attrib.faces[face_offset + 3 * f + 1];
              tinyobj_vertex_index_t idx2 = obj->attrib.faces[face_offset + 3 * f + 2];

              for (k = 0; k < 3; k++) {
                int f0 = idx0.v_idx;
                int f1 = idx1.v_idx;
                int f2 = idx2.v_idx;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                v[0][k] = obj->attrib.vertices[3 * (size_t)f0 + k];
                v[1][k] = obj->attrib.vertices[3 * (size_t)f1 + k];
                v[2][k] = obj->attrib.vertices[3 * (size_t)f2 + k];
              }

              {
              int f0 = idx0.vt_idx;
              int f1 = idx1.vt_idx;
              int f2 = idx2.vt_idx;
              assert(f0 >= 0);
              assert(f1 >= 0);
              assert(f2 >= 0);

              assert(f0 < obj->attrib.num_texcoords);
              assert(f1 < obj->attrib.num_texcoords);
              assert(f2 < obj->attrib.num_texcoords);

              uv[0][0] = obj->attrib.texcoords[2 * (size_t)f0 + 0];
              uv[1][0] = obj->attrib.texcoords[2 * (size_t)f1 + 0];
              uv[2][0] = obj->attrib.texcoords[2 * (size_t)f2 + 0];

              uv[0][1] = 1.0f - obj->attrib.texcoords[2 * (size_t)f0 + 1];
              uv[1][1] = 1.0f - obj->attrib.texcoords[2 * (size_t)f1 + 1];
              uv[2][1] = 1.0f - obj->attrib.texcoords[2 * (size_t)f2 + 1];
              }


              if (obj->attrib.num_normals > 0) {
                int f0 = idx0.vn_idx;
                int f1 = idx1.vn_idx;
                int f2 = idx2.vn_idx;
                if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
                  assert(f0 < (int)obj->attrib.num_normals);
                  assert(f1 < (int)obj->attrib.num_normals);
                  assert(f2 < (int)obj->attrib.num_normals);
                  for (k = 0; k < 3; k++) {
                    n[0][k] = obj->attrib.normals[3 * (size_t)f0 + k];
                    n[1][k] = obj->attrib.normals[3 * (size_t)f1 + k];
                    n[2][k] = obj->attrib.normals[3 * (size_t)f2 + k];
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

                /*if (obj->attrib.material_ids[i] >= 0) {
                  int matidx = obj->attrib.material_ids[i];
                  vertexs[3 * i + k].color.x = materials[matidx].diffuse[0];
                  vertexs[3 * i + k].color.y = materials[matidx].diffuse[1];
                  vertexs[3 * i + k].color.z = materials[matidx].diffuse[2];
                }else {
                    vertexs[3 * i + k].color.x = n[k][0];
                    vertexs[3 * i + k].color.y = n[k][1];
                    vertexs[3 * i + k].color.z = n[k][2];
                }*/


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
        face_offset += (size_t)obj->attrib.face_num_verts[i];
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

static void get_file_data(void* ctx, const char* filename, const int is_mtl, const char* obj_filename, char** data, size_t* len) {
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

void DestroyOBJModel(ModelObject3D *mo){

    OBJStruct *obj = mo->obj;

    tinyobj_attrib_free(&obj->attrib);
    tinyobj_shapes_free(obj->shapes,  obj->num_shapes);
    tinyobj_materials_free(obj->materials, obj->num_materials);
    free(mo->obj);

    GraphicsObjectDestroy(&mo->nodes[0].models[0].graphObj);

    if(mo->nodes[0].models->diffuse != NULL)
    {
        free(mo->nodes[0].models->diffuse->path);

        if(mo->nodes[0].models->diffuse->size > 0)
            free(mo->nodes[0].models->diffuse->buffer);

        free(mo->nodes[0].models->diffuse);
    }

    if(mo->nodes[0].models->specular != NULL)
    {
        free(mo->nodes[0].models->specular->path);

        if(mo->nodes[0].models->specular->size > 0)
            free(mo->nodes[0].models->specular->buffer);

        free(mo->nodes[0].models->specular);
    }

    if(mo->nodes[0].models->normal != NULL)
    {
        free(mo->nodes[0].models->normal->path);

        if(mo->nodes[0].models->normal->size > 0)
            free(mo->nodes[0].models->normal->buffer);

        free(mo->nodes[0].models->normal);
    }
}

void Load3DObjModel(ModelObject3D * mo, char *filepath, DrawParam *dParam){

  Transform3DInit(&mo->transform);

  GameObjectSetUpdateFunc(mo, (void *)ModelDefaultUpdate);
  GameObjectSetDrawFunc(mo, (void *)ModelDefaultDraw);
  GameObjectSetCleanFunc(mo, (void *)ModelClean);
  GameObjectSetRecreateFunc(mo, (void *)ModelRecreate);
  GameObjectSetDestroyFunc(mo, (void *)DestroyOBJModel);

  mo->obj = (OBJStruct *) calloc(1, sizeof(OBJStruct));

  OBJStruct *obj = mo->obj;

  unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
  tinyobj_parse_obj(&obj->attrib, &obj->shapes, &obj->num_materials, &obj->materials, &obj->num_materials, filepath, (void *)get_file_data, NULL, flags);

  mo->nodes = calloc(1, sizeof(ModelNode));
  mo->num_draw_nodes = 1;

  mo->nodes[0].models = calloc(1, sizeof(ModelStruct));
  mo->nodes[0].num_models = 1;

  ModelStruct *model = &mo->nodes[0].models[0];

  GraphicsObjectInit(&model->graphObj, ENGINE_VERTEX_TYPE_MODEL_OBJECT);

  model->graphObj.gItems.perspective = true;

  model->graphObj.shapes[0].vParam.vertices = (ModelVertex3D *) calloc(obj->attrib.num_face_num_verts * 3, sizeof(ModelVertex3D));
  model->graphObj.shapes[0].iParam.indices = (uint32_t *) calloc(obj->attrib.num_face_num_verts * 3, sizeof(uint32_t));

  ParseSomeStruct(mo, model->graphObj.shapes[0].vParam.vertices );

  model->graphObj.shapes[0].vParam.verticesSize = obj->attrib.num_face_num_verts * 3;
  model->graphObj.shapes[0].iParam.indexesSize = obj->attrib.num_face_num_verts * 3;

  for(int i=0; i < model->graphObj.shapes[0].iParam.indexesSize;i++)
      model->graphObj.shapes[0].iParam.indices[i] = i;

  GraphicsObjectSetVertex(&model->graphObj, model->graphObj.shapes[0].vParam.vertices, model->graphObj.shapes[0].vParam.verticesSize, sizeof(ModelVertex3D), model->graphObj.shapes[0].iParam.indices, model->graphObj.shapes[0].iParam.indexesSize, sizeof(uint32_t));


  if(dParam != NULL)
  {
      if(model->diffuse == NULL)
      {
          model->diffuse = calloc(1, sizeof(GameObjectImage));

          if(strlen(dParam->diffuse) != 0)
          {
              int len = strlen(dParam->diffuse);
              model->diffuse->path = calloc(len + 1, sizeof(char));
              memcpy(model->diffuse->path, dParam->diffuse, len);
              model->diffuse->path[len] = '\0';
          }
      }

      if(model->specular == NULL)
      {
          model->specular = calloc(1, sizeof(GameObjectImage));

          if(strlen(dParam->specular) != 0)
          {
              int len = strlen(dParam->specular);
              model->specular->path = calloc(len + 1, sizeof(char));
              memcpy(model->specular->path, dParam->specular, len);
              model->specular->path[len] = '\0';
          }
      }

      if(model->normal == NULL)
      {
          model->normal = calloc(1, sizeof(GameObjectImage));

          if(strlen(dParam->normal) != 0)
          {
              int len = strlen(dParam->normal);
              model->normal->path = calloc(len + 1, sizeof(char));
              memcpy(model->normal->path, dParam->normal, len);
              model->normal->path[len] = '\0';
          }
      }
  }

  ModelDefaultInit(model, dParam);
}


