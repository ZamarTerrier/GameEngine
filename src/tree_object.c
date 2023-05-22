#include "tree_object.h"

#include <vulkan/vulkan.h>

#include "pipeline.h"

#include "e_blue_print.h"

#include "e_math.h"

#include "e_resource_shapes.h"
#include "e_resource_export.h"

vec3 type_01_v[12] = {
    {0.5f, 0, 0},
    {0.7f, 1, 0},
    {-0.7f, 1, 0},
    {-0.5f, 0, 0},
    {0.7f, 2, 0},
    {-0.7f, 2, 0},
    {1.5f, 1.5f, 0},
    {1.5f, 2.0f, 0},
    {0.5f, 3, 0},
    {-0.5f, 3, 0},
    {-1.5f, 2.0f, 0},
    {-1.5f, 1.5f, 0},
};

uint32_t type_01_i[30] = {
    0, 1, 2, 2, 3, 0,
    1, 4, 5, 5, 2, 1,
    1, 6, 7, 7, 4, 1,
    4, 8, 9, 9, 5, 4,
    5, 10, 11, 11, 2, 5,
};

TreeVerts type_01 = {
    .positions = type_01_v,
    .num_positions = 12,
    .indexes = type_01_i,
    .num_indexes = 30
};

vec3 type_02_v[10] = {
    {0.5f, 0, 0},
    {0.7f, 1, 0},
    {-0.7f, 1, 0},
    {-0.5f, 0, 0},
    {0.2f, 2.0f, 0},
    {-0.2f, 2.0f, 0},
    {2.5f, 3.0f, 0},
    {1.5f, 3.0f, 0},
    {-1.5f, 3.0f, 0},
    {-2.5f, 3.0f, 0},
};

uint32_t type_02_i[24] = {
    0, 1, 2, 2, 3, 0,
    1, 4, 5, 5, 2, 1,
    1, 6, 7, 7, 4, 1,
    5, 8, 9, 9, 2, 5,
};

TreeVerts type_02 = {
    .positions = type_02_v,
    .num_positions = 10,
    .indexes = type_02_i,
    .num_indexes = 24
};

vec3 type_03_v[12] = {
    {0.5f, 0, 0},
    {0.7f, 1, 0},
    {-0.7f, 1, 0},
    {-0.5f, 0, 0},
    {0.5f, 2, 0},
    {0.7f, 3, 0},
    {-0.7f, 3, 0},
    {-0.5f, 2, 0},
    {0.5f, 4, 0},
    {0.7f, 5, 0},
    {-0.7f, 5, 0},
    {-0.5f, 4, 0},
};

uint32_t type_03_i[30] = {
    0, 1, 2, 2, 3, 0,
    1, 4, 7, 7, 2, 1,
    4, 5, 6, 6, 7, 4,
    5, 8, 11, 11, 6, 5,
    8, 9, 10, 10, 11, 8,
};

TreeVerts type_03 = {
    .positions = type_03_v,
    .num_positions = 12,
    .indexes = type_03_i,
    .num_indexes = 30
};

vec3 type_04_v[12] = {
    {-0.5f, 0, 0},
    {-0.5f, 0.5f, 0},
    {1.0, 1.0f, 0},
    {1.0, 0.5f, 0},

    {2.0, 1.0f, 0},
    {2.0, 1.5f, 0},
    {3.0, 1.5f, 0},
    {3.0, 1.0f, 0},

    {4.0, 1.5f, 0},
    {4.0, 2.0f, 0},
    {5.0, 2.0f, 0},
    {5.0, 1.5f, 0},
};

uint32_t type_04_i[30] = {
    0, 1, 2, 2, 3, 0,
    3, 2, 5, 5, 4, 3,
    4, 5, 6, 6, 7, 4,
    7, 6, 9, 9, 8, 7,
    8, 9, 10, 10, 11, 8,
};

TreeVerts type_04 = {
    .positions = type_04_v,
    .num_positions = 12,
    .indexes = type_04_i,
    .num_indexes = 30
};

vec3 type_05_v[12] = {
    {0.5f, 0, 0},
    {0.5f, 0.5f, 0},
    {-1.0, 1.0f, 0},
    {-1.0, 0.5f, 0},

    {-2.0, 1.0f, 0},
    {-2.0, 1.5f, 0},
    {-3.0, 1.5f, 0},
    {-3.0, 1.0f, 0},

    {-4.0, 1.5f, 0},
    {-4.0, 2.0f, 0},
    {-5.0, 2.0f, 0},
    {-5.0, 1.5f, 0},
};

uint32_t type_05_i[30] = {
    0, 1, 2, 2, 3, 0,
    3, 2, 5, 5, 4, 3,
    4, 5, 6, 6, 7, 4,
    7, 6, 9, 9, 8, 7,
    8, 9, 10, 10, 11, 8,
};

TreeVerts type_05 = {
    .positions = type_05_v,
    .num_positions = 12,
    .indexes = type_05_i,
    .num_indexes = 30
};

void TreeMakePeace(vertexParam *vParam, indexParam *iParam, TreeVerts *tree_verts, vec3 pos, float angle, VertextIterator *vi)
{
    Vertex3D *verts = vParam->vertices;

    uint32_t last_vertex = vi->v_index;

    float t_angle = M_PI / 180 * angle;

    vec3 dir;
    dir.x = cos(t_angle) + sin(t_angle);
    dir.y = -sin(t_angle) + cos(t_angle);

    vec3 temp;

    float radius = 0.5f;
    for(int i=0; i < tree_verts->num_positions;i++)
    {

        temp = vec3_f(tree_verts->positions[i].x * radius, tree_verts->positions[i].y, tree_verts->positions[i].z * radius);
        temp.x = temp.x * cos(t_angle) - temp.y * sin(t_angle);
        temp.y = temp.x * sin(t_angle) + temp.y * cos(t_angle);
        temp = v3_add(pos, temp);

        verts[vi->v_index].position = temp;
        vi->v_index ++;
    }

    for(int i=0; i < tree_verts->num_indexes;i++)
    {
        iParam->indices[vi->i_index] = tree_verts->indexes[i] + last_vertex;
        vi->i_index ++;
    }
}

uint32_t num_peaces = 0;
TreePeace t_peaces[20];
void AddNewBranch(vec3 point, uint32_t type, float angle)
{
    t_peaces[num_peaces].type = type;
    t_peaces[num_peaces].orig = point;
    t_peaces[num_peaces].angle = angle;

    num_peaces ++;
}

void InitTreeVertices(vertexParam *vParam, indexParam *iParam)
{
    memset(&t_peaces, 0, sizeof(TreePeace) * 20);

    AddNewBranch(vec3_f(0.0, -5, 0), 2, 0);
    AddNewBranch(vec3_f(0, 0, 0), 0, 0);
    AddNewBranch(vec3_f(0, 3, 0), 1, 0);
    AddNewBranch(vec3_f(1.0, 6, 0), 1, 0);
    AddNewBranch(vec3_f(-1.0, 6, 0), 2, 0);
    AddNewBranch(vec3_f(1.0, 1.5f, 0), 3, 0);
    AddNewBranch(vec3_f(-1.0, 1.5f, 0), 4, 0);

    VertextIterator vi;
    memset(&vi, 0, sizeof(VertextIterator));

    uint32_t indx_points = 0;
    uint32_t vert_points = 0;

    for(int i=0;i < num_peaces;i++){
        switch(t_peaces[i].type){
            case 0:
                indx_points += type_01.num_indexes;
                vert_points += type_01.num_positions;
                break;
            case 1:
                indx_points += type_02.num_indexes;
                vert_points += type_02.num_positions;
                break;
            case 2:
                indx_points += type_03.num_indexes;
                vert_points += type_03.num_positions;
                break;
            case 3:
                indx_points += type_04.num_indexes;
                vert_points += type_04.num_positions;
                break;
            case 4:
                indx_points += type_05.num_indexes;
                vert_points += type_05.num_positions;
                break;
        }
    }

    vParam->verticesSize = vert_points;
    iParam->indexesSize = indx_points;

    vParam->vertices = (Vertex3D *) calloc(vParam->verticesSize, sizeof(Vertex3D));
    iParam->indices = (uint32_t *) calloc(iParam->indexesSize, sizeof(uint32_t));


    for(int i=0;i < num_peaces;i++){
        switch(t_peaces[i].type){
            case 0:
                TreeMakePeace(vParam, iParam, &type_01, t_peaces[i].orig, t_peaces[i].angle, &vi);
                break;
            case 1:
                TreeMakePeace(vParam, iParam, &type_02, t_peaces[i].orig, t_peaces[i].angle, &vi);
                break;
            case 2:
                TreeMakePeace(vParam, iParam, &type_03, t_peaces[i].orig, t_peaces[i].angle, &vi);
                break;
            case 3:
                TreeMakePeace(vParam, iParam, &type_04, t_peaces[i].orig, t_peaces[i].angle, &vi);
                break;
            case 4:
                TreeMakePeace(vParam, iParam, &type_05, t_peaces[i].orig, t_peaces[i].angle, &vi);
                break;
        }
    }

    for(int i=0;i < iParam->indexesSize;i++)
    {
        if(iParam->indices[i] == 0)
            continue;
    }
}

void TreeObjectInit(TreeObject *to, DrawParam *dParam)
{
    GameObject3DInit(to);

    vertexParam vParam;
    memset(&vParam, 0, sizeof(vertexParam));
    indexParam iParam;
    memset(&iParam, 0, sizeof(indexParam));

    InitTreeVertices(&vParam, &iParam);

    GraphicsObjectSetVertex(&to->go.graphObj, vParam.vertices, vParam.verticesSize, sizeof(Vertex3D), iParam.indices, iParam.indexesSize, sizeof(uint32_t));
}

void TreeObjectSetDefaultDescriptor(TreeObject *to, DrawParam *dParam)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_tree_vert_spv_start, (size_t)(&_binary_shaders_tree_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_tree_frag_spv_start, (size_t)(&_binary_shaders_tree_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_NONE;
    setting.poligonMode = VK_POLYGON_MODE_LINE;
    //setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    //setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;
}

void TreeObjectInitDefault(TreeObject *to, DrawParam *dParam)
{
    TreeObjectInit(to, dParam);

    TreeObjectSetDefaultDescriptor(to, dParam);

    GameObject3DInitDraw(to);
}
