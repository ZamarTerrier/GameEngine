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
    {0.5f, 5, 0},
    {-0.5f, 5, 0},
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
    {1.0, 0.5f, 0},
    {1.0, 0.0f, 0},

    {2.0f, 0.5f, 0},
    {2.0f, 1.0f, 0},
    {3.5f, 1.0f, 0},
    {3.5f, 0.5f, 0},

    {4.5, 1.0f, 0},
    {4.5, 1.5f, 0},
    {6.0f, 1.5f, 0},
    {6.0f, 1.0f, 0},
};

uint32_t type_04_i[30] = {
    0, 3, 2, 2, 1, 0,
    3, 4, 5, 5, 2, 3,
    4, 7, 6, 6, 5, 4,
    7, 8, 9, 9, 6, 7,
    8, 11, 10, 10, 9, 8,
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

float TreeMake3DPeace(vertexParam *vParam, indexParam *iParam, vec3 pos, vec3 rotation, uint32_t length, float radius, VertextIterator *vi)
{
    PipeMeshParams params = {
        .sectors = 6,
        .height = length,
        .position = vec3_f( 0, 0, 0),
        .rotation = vec3_f( 0, 0, 0),
        .radius = radius
    };

    uint32_t indx = vi->v_index;
    float last_radius = ToolsMakePipe(vParam, iParam, &params, vi);
    uint32_t indx1 = vi->v_index - params.sectors;

    vec3  orig_rot = params.rotation;

    params.position = pos;
    params.position.x = - last_radius;
    params.position.y = length + last_radius; // v3_add(pos, v3_muls(orig_dir, 25));
    params.height = 5;
    params.radius = last_radius - 0.1f;
    params.rotation.z = orig_rot.z + 45.0;

    uint32_t indx2 = vi->v_index + 1;
    ToolsMakePipe(vParam, iParam, &params, vi);

    params.position.x =  last_radius;
    params.rotation.z = orig_rot.z - 45.0;

    uint32_t indx3 = vi->v_index + 1;
    last_radius = ToolsMakePipe(vParam, iParam, &params, vi);


    uint32_t indx4 = vi->v_index;
    Vertex3D *verts = vParam->vertices;
    for(int i = indx; i < indx4; i++)
    {
        verts[i].position = v3_add(m3_v3_mult(m3_rotation_matrix(rotation), verts[i].position), pos);
    }

    //--------------------------------------
    iParam->indices[vi->i_index] = indx2 + 1;
    iParam->indices[vi->i_index + 1] = indx1;
    iParam->indices[vi->i_index + 2] = indx1 + 1;
    vi->i_index +=3;

    iParam->indices[vi->i_index] = indx3;
    iParam->indices[vi->i_index + 1] = indx2 + 1;
    iParam->indices[vi->i_index + 2] = indx1;
    vi->i_index +=3;

    //--------------------------------------

    iParam->indices[vi->i_index] = indx3;
    iParam->indices[vi->i_index + 1] = indx2 + 1;
    iParam->indices[vi->i_index + 2] = indx3 + 1;
    vi->i_index +=3;

    iParam->indices[vi->i_index] = indx2;
    iParam->indices[vi->i_index + 1] = indx3 + 1;
    iParam->indices[vi->i_index + 2] = indx2 + 1;
    vi->i_index +=3;

    for(uint32_t i=1;i < 4;i++)
    {

        iParam->indices[vi->i_index] = indx3 + i;
        iParam->indices[vi->i_index + 1] = indx2 + params.sectors - i;
        iParam->indices[vi->i_index + 2] = i > 1 ? indx2 + (params.sectors - i + 1) : indx2;
        vi->i_index +=3;

        iParam->indices[vi->i_index] = indx2 + params.sectors - i;
        iParam->indices[vi->i_index + 1] = indx3 + i;
        iParam->indices[vi->i_index + 2] = indx3 + i + 1;
        vi->i_index +=3;
    }

    //--------------------------------------
    iParam->indices[vi->i_index] = indx3 + 4;
    iParam->indices[vi->i_index + 1] = indx1 + 3;
    iParam->indices[vi->i_index + 2] = indx2 + params.sectors - 3;
    vi->i_index +=3;

    iParam->indices[vi->i_index] = indx3 + 4;
    iParam->indices[vi->i_index + 1] = indx1 + 3;
    iParam->indices[vi->i_index + 2] = indx1 + 4;
    vi->i_index +=3;

    //--------------------------------------

    for(uint32_t i=0;i < 2;i++)
    {
        iParam->indices[vi->i_index] = indx2 + params.sectors - 3 - i;
        iParam->indices[vi->i_index + 1] = indx1 + 2 - i;
        iParam->indices[vi->i_index + 2] = indx1 + 3 - i;
        vi->i_index +=3;

        iParam->indices[vi->i_index] = indx2 + params.sectors - 3 - i;
        iParam->indices[vi->i_index + 1] = indx2 + params.sectors - 4 - i;
        iParam->indices[vi->i_index + 2] = indx1 + 2 - i;
        vi->i_index +=3;
    }

    //--------------------------------------

    for(uint32_t i=0;i < 2;i++)
    {
        iParam->indices[vi->i_index] = indx3 + 4 + i;
        iParam->indices[vi->i_index + 1] = indx1 + 4 + i;
        iParam->indices[vi->i_index + 2] = i > 0 ? indx1 + i - 1 : indx1 + params.sectors - 1;
        vi->i_index +=3;

        iParam->indices[vi->i_index] = indx3 + 4 + i;
        iParam->indices[vi->i_index + 1] = i > 0 ? indx3 + i - 1 : indx3 + params.sectors - 1;
        iParam->indices[vi->i_index + 2] = i > 0 ? indx1 + i - 1 : indx1 + params.sectors - 1;
        vi->i_index +=3;
    }

    iParam->indices[vi->i_index] = indx3 + 4;
    iParam->indices[vi->i_index + 1] = indx1 + 4;
    iParam->indices[vi->i_index + 2] = indx1 + 5;
    vi->i_index +=3;

    iParam->indices[vi->i_index] = indx3 + 4;
    iParam->indices[vi->i_index + 1] = indx3 + 5;
    iParam->indices[vi->i_index + 2] = indx1 + 5;
    vi->i_index +=3;

    iParam->indices[vi->i_index] = indx3 + 5;
    iParam->indices[vi->i_index + 1] = indx1 + 5;
    iParam->indices[vi->i_index + 2] = indx1;
    vi->i_index +=3;

    iParam->indices[vi->i_index] = indx3 + 5;
    iParam->indices[vi->i_index + 1] = indx3;
    iParam->indices[vi->i_index + 2] = indx1;
    vi->i_index +=3;

    return last_radius;
}

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

void InitTreeVertices(vertexParam *vParam, indexParam *iParam, VertextIterator *vi)
{
    memset(&t_peaces, 0, sizeof(TreePeace) * 20);

    AddNewBranch(vec3_f(0.0, 0, 0), 2, 0);
    AddNewBranch(vec3_f(0, 5, 0), 0, 0);
    AddNewBranch(vec3_f(0, 8, 0), 1, 0);
    AddNewBranch(vec3_f(1.0, 11, 0), 1, 0);
    AddNewBranch(vec3_f(-1.0, 11, 0), 2, 0);
    AddNewBranch(vec3_f(1.0, 6.5f, 0), 3, 0);
    AddNewBranch(vec3_f(-1.0, 6.5f, 0), 4, 0);

    for(int i=0;i < num_peaces;i++){
        switch(t_peaces[i].type){
            case 0:
                TreeMakePeace(vParam, iParam, &type_01, t_peaces[i].orig, t_peaces[i].angle, vi);
                break;
            case 1:
                TreeMakePeace(vParam, iParam, &type_02, t_peaces[i].orig, t_peaces[i].angle, vi);
                break;
            case 2:
                TreeMakePeace(vParam, iParam, &type_03, t_peaces[i].orig, t_peaces[i].angle, vi);
                break;
            case 3:
                TreeMakePeace(vParam, iParam, &type_04, t_peaces[i].orig, t_peaces[i].angle, vi);
                break;
            case 4:
                TreeMakePeace(vParam, iParam, &type_05, t_peaces[i].orig, t_peaces[i].angle, vi);
                break;
        }
    }

    for(int i=0;i < iParam->indexesSize;i++)
    {
        if(iParam->indices[i] == 0)
            continue;
    }
}

void TreeObjectInit(TreeObject *to, uint32_t type, DrawParam *dParam)
{
    GameObject3DInit(to);

    if(type == ENGINE_TREE_OBJECT_TYPE_VERTEX)
    {
        vertexParam vParam;
        memset(&vParam, 0, sizeof(vertexParam));
        indexParam iParam;
        memset(&iParam, 0, sizeof(indexParam));
        VertextIterator vi;
        memset(&vi, 0, sizeof(VertextIterator));

        uint32_t buff_size = UINT16_MAX;

        vParam.vertices = calloc(buff_size, sizeof(Vertex3D));
        iParam.indices = calloc(buff_size * 3, sizeof(uint32_t));

        //InitTreeVertices(&vParam, &iParam, &vi);

        float last_radius = TreeMake3DPeace(&vParam, &iParam, vec3_f(0, 0, 0), vec3_f(0, 0, 0), 20, 2, &vi);
        TreeMake3DPeace(&vParam, &iParam, vec3_f(-7, 27, 0), vec3_f( 0, 0, 45), 10, last_radius, &vi);
        last_radius = TreeMake3DPeace(&vParam, &iParam, vec3_f(7, 27, 0), vec3_f(0, 0, -45), 10, last_radius, &vi);
        TreeMake3DPeace(&vParam, &iParam, vec3_f(14, 43, 0), vec3_f(0, 0, 0), 10, last_radius, &vi);
        TreeMake3DPeace(&vParam, &iParam, vec3_f(-14, 43, 0), vec3_f(0, 0, 0), 10, last_radius, &vi);

        TreeMake3DPeace(&vParam, &iParam, vec3_f(-23, 34, 0), vec3_f(0, 0, 90), 10, last_radius, &vi);
        TreeMake3DPeace(&vParam, &iParam, vec3_f(23, 34, 0), vec3_f(0, 0, -90), 10, last_radius, &vi);


        GraphicsObjectSetVertex(&to->go.graphObj, vParam.vertices, vi.v_index, sizeof(Vertex3D), iParam.indices, vi.i_index, sizeof(uint32_t));

        free(vParam.vertices);
        free(iParam.indices);
    }else{
        GraphicsObjectSetVertex(&to->go.graphObj, cubeVert, 24, sizeof(Vertex3D), cubeIndx, 36, sizeof(uint32_t));
    }
}

void TreeObjectSetDefaultDescriptor(TreeObject *to, uint32_t type, DrawParam *dParam)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    switch(type)
    {
        case ENGINE_TREE_OBJECT_TYPE_VERTEX:
            PipelineSettingSetShader(&setting, &_binary_shaders_tree_vert_spv_start, (size_t)(&_binary_shaders_tree_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
            PipelineSettingSetShader(&setting, &_binary_shaders_tree_frag_spv_start, (size_t)(&_binary_shaders_tree_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
        case ENGINE_TREE_OBJECT_TYPE_SDF:
            BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(SDFBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DSDFBufferUpdate, 0);

            PipelineSettingSetShader(&setting, &_binary_shaders_tree_sdf_vert_spv_start, (size_t)(&_binary_shaders_tree_sdf_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
            PipelineSettingSetShader(&setting, &_binary_shaders_tree_sdf_frag_spv_start, (size_t)(&_binary_shaders_tree_sdf_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
    }


    setting.fromFile = 0;
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_NONE;
    setting.poligonMode = VK_POLYGON_MODE_LINE;
    //setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    //setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;
}

void TreeObjectInitDefault(TreeObject *to, uint32_t type, DrawParam *dParam)
{
    TreeObjectInit(to, type, dParam);

    TreeObjectSetDefaultDescriptor(to, type, dParam);

    GameObject3DInitDraw(to);
}
