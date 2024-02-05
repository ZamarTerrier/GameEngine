#include "tree_object.h"

#include <vulkan/vulkan.h>

#include "pipeline.h"

#include "e_buffer.h"

#include "e_blue_print.h"

#include "e_math.h"

#include "e_resource_shapes.h"
#include "e_resource_export.h"
#include "e_resource_descriptors.h"

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

typedef struct{
    float radius;
    uint32_t point_indx[3];
    vec3 point_vec[3];
    vec3 rotation;
    vec3 position;
    vec3 tree_color;
    vec3 leaf_color;
} TreeLast;

typedef struct{
    uint32_t sectors;
    uint32_t height;
    vec3 position;
    vec3 rotation;
    vec3 color;
    float radius;
    float grad;
} PipeMeshParams;

void TreeMakeLeaf(vertexParam *vParam, indexParam *iParam, vec3 pos, vec3 rot, vec3 color, float size, VertextIterator *vi)
{

    TreeVertex3D *verts = vParam->vertices;

    mat3 rot_mat = m3_rotation_matrix(v3_muls(rot, 360));

    uint32_t last_v_indx = vi->v_index;
    verts[vi->v_index].position = v3_add(m3_v3_mult(rot_mat, vec3_f(-size, 0, 0)), pos);
    verts[vi->v_index].color = color;
    verts[vi->v_index].texCoord = vec2_f(0, 0);
    verts[vi->v_index + 1].position = v3_add(m3_v3_mult(rot_mat, vec3_f(-size, size * 2.0f, 0)), pos);
    verts[vi->v_index + 1].color = color;
    verts[vi->v_index + 1].texCoord = vec2_f(0, 1);
    verts[vi->v_index + 2].position = v3_add(m3_v3_mult(rot_mat, vec3_f(size, size * 2.0f, 0)), pos);
    verts[vi->v_index + 2].color = color;
    verts[vi->v_index + 2].texCoord = vec2_f(1, 1);
    verts[vi->v_index + 3].position = v3_add(m3_v3_mult(rot_mat, vec3_f(size, 0, 0)), pos);
    verts[vi->v_index + 3].color = color; //;
    verts[vi->v_index + 3].texCoord = vec2_f(1, 0);
    vi->v_index +=4;

    iParam->indices[vi->i_index] = last_v_indx;
    iParam->indices[vi->i_index + 1] = last_v_indx + 1;
    iParam->indices[vi->i_index + 2] = last_v_indx + 2;
    vi->i_index +=3;

    iParam->indices[vi->i_index] = last_v_indx + 2;
    iParam->indices[vi->i_index + 1] = last_v_indx + 3;
    iParam->indices[vi->i_index + 2] = last_v_indx;
    vi->i_index +=3;

}

float TreeMakePipe(vertexParam *vParam, indexParam *iParam, void *args, VertextIterator *vi)
{
    PipeMeshParams *params = args;

    float step = M_PI * 2 / params->sectors;

    TreeVertex3D *verts = vParam->vertices;

    mat3 rot_mat = m3_rotation_matrix(params->rotation);

    vec3 temp;

    uint32_t last_index = vi->v_index;

    float radius = params->radius;

    for(int y = 0; y <= params->height; y ++)
    {
        for(int i = 0; i <= params->sectors; i ++)
        {
            temp.x = cos(step * i) * radius;
            temp.z = sin(step * i) * radius;
            temp.y = y;
            temp = m3_v3_mult(rot_mat, temp);

            verts[vi->v_index].position = v3_add(temp, params->position);
            verts[vi->v_index].texCoord = vec2_f((float)(y) / params->height, (float)(i) / params->sectors);
            verts[vi->v_index].color = params->color;//;
            vi->v_index++;

            radius -= params->grad;
        }
    }

    int k1, k2;
    for(int i = 0; i < params->height; ++i)
    {
        k1 = i * (params->sectors + 1) + last_index;     // beginning of current stack
        k2 = k1 + params->sectors + 1 ;      // beginning of next stack

        for(int j = 0; j < params->sectors; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1

            iParam->indices[vi->i_index] = k1 + 1;
            iParam->indices[vi->i_index + 1] = k2;
            iParam->indices[vi->i_index + 2] = k1;
            vi->i_index +=3;

            iParam->indices[vi->i_index] = k2 + 1;
            iParam->indices[vi->i_index + 1] = k2;
            iParam->indices[vi->i_index + 2] = k1 + 1;
            vi->i_index +=3;
        }
    }

    return radius;
}

void TreePopulateLeafs(vertexParam *vParam, indexParam *iParam, TreeLast tLast, uint32_t sectors, uint32_t length, VertextIterator *vi)
{
    TreeVertex3D *verts = vParam->vertices;

    uint32_t indx = vi->v_index;

    vec3 pos;
    vec3 dir;
    for(int i=0;i < length;i++)
        for(int j=0;j < sectors;j+=2){
            pos = verts[indx - (i * sectors) - j - 1].position;
            dir = v3_sub(tLast.position, pos);
            TreeMakeLeaf(vParam, iParam, pos, v3_norm(dir), tLast.leaf_color, 2.0f, vi);
        }
}

TreeLast TreeMake3DPeace(vertexParam *vParam, indexParam *iParam, vec3 pos, vec3 rotation, vec3 color, uint32_t length, float radius, float grad, VertextIterator *vi)
{
    if(radius < 1.0f)
        radius = 1.0f;

    PipeMeshParams params = {
        .sectors = 6,
        .height = length,
        .position = vec3_f( 0, 0, 0),
        .rotation = vec3_f( 0, 0, 0),
        .radius = radius,
        .grad = grad,
        .color = color
    };

    TreeVertex3D *verts = vParam->vertices;

    uint32_t indx = vi->v_index;
    TreeLast treeLast;
    treeLast.radius = TreeMakePipe(vParam, iParam, &params, vi);
    uint32_t indx1 = vi->v_index - params.sectors;

    vec3 orig_pos = params.position;
    vec3 orig_rot = params.rotation;

    params.position = orig_pos;
    params.position.y = 3;
    params.position = m3_v3_mult(m3_rotation_matrix(vec3_f(0, 0, 45)), params.position);
    params.position.y = length + treeLast.radius;
    params.radius = treeLast.radius - 0.1f;
    params.rotation.z = orig_rot.z + 45;
    params.height = 2;
    params.grad = grad / 100;
    treeLast.point_vec[1] = params.position;

    uint32_t indx2 = vi->v_index + 1;
    TreeMakePipe(vParam, iParam, &params, vi);

    params.position = orig_pos;
    params.position.y = 3;
    params.position = m3_v3_mult(m3_rotation_matrix(vec3_f(0, 0, -45)), params.position);
    params.position.y = length + treeLast.radius;
    params.rotation.z = orig_rot.z - 45.0;
    treeLast.point_vec[2] = params.position;

    uint32_t indx3 = vi->v_index + 1;
    treeLast.radius = TreeMakePipe(vParam, iParam, &params, vi);

    uint32_t indx4 = vi->v_index;
    mat4 m4_rot = m4_rotation_matrix(rotation);
    vec4 s_vec;
    for(int i = indx; i < indx4; i++)
    {
        s_vec = vec4_f(verts[i].position.x, verts[i].position.y, verts[i].position.z, 1.0);
        s_vec = m4_v4_mult(m4_rot, s_vec);
        s_vec = v4_div(s_vec, s_vec.w);
        verts[i].position = v3_add(vec3_f(s_vec.x, s_vec.y, s_vec.z), pos);
    }

    treeLast.rotation = rotation;

    treeLast.point_vec[0] = pos;
    treeLast.point_vec[1].x -= 3.5f;
    treeLast.point_vec[1].y += 3.5f;
    treeLast.point_vec[2].x += 3.5f;
    treeLast.point_vec[2].y += 3.5f;
    s_vec = vec4_f(treeLast.point_vec[1].x, treeLast.point_vec[1].y, treeLast.point_vec[1].z, 1.0);
    s_vec = m4_v4_mult(m4_rot, s_vec);
    s_vec = v4_div(s_vec, s_vec.w);
    treeLast.point_vec[1] = v3_add(vec3_f(s_vec.x, s_vec.y, s_vec.z), pos);
    s_vec = vec4_f(treeLast.point_vec[2].x, treeLast.point_vec[2].y, treeLast.point_vec[2].z, 1.0);
    s_vec = m4_v4_mult(m4_rot, s_vec);
    s_vec = v4_div(s_vec, s_vec.w);
    treeLast.point_vec[2] = v3_add(vec3_f(s_vec.x, s_vec.y, s_vec.z), pos);

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

    treeLast.point_indx[0] = indx;
    treeLast.point_indx[1] = indx3 - params.sectors - 1;
    treeLast.point_indx[2] = indx4 - params.sectors - 1;

    treeLast.position = pos;

    return treeLast;
}

void TreeWebLastPoints(indexParam *iParam, uint32_t sectors, uint32_t t_last_1, uint32_t t_last_2, VertextIterator *vi)
{
    uint32_t k1 = t_last_1 - 1, k2 = t_last_2;

    for(int i=0; i < sectors;i++)
    {
        iParam->indices[vi->i_index] = k1 + 1;
        iParam->indices[vi->i_index + 1] = k2;
        iParam->indices[vi->i_index + 2] = k1;
        vi->i_index += 3;

        iParam->indices[vi->i_index] = k2 + 1;
        iParam->indices[vi->i_index + 1] = k2;
        iParam->indices[vi->i_index + 2] = k1 + 1;
        vi->i_index +=3;

        k1 ++;
        k2 ++;
    }
}

void TreeMakePeace(vertexParam *vParam, indexParam *iParam, TreeVerts *tree_verts, vec3 pos, float angle, VertextIterator *vi)
{
    TreeVertex3D *verts = vParam->vertices;

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

vec3 TreeSummRotation(vec3 rotation, vec3 summ)
{
    vec3 result;

    if(rotation.x > 0)
        result.x = rotation.x + summ.x;
    else
        result.x = rotation.x - summ.x;

    if(rotation.y > 0)
        result.y = rotation.y + summ.y;
    else
        result.y = rotation.y - summ.y;

    if(rotation.z > 0)
        result.z = rotation.z + summ.z;
    else
        result.z = rotation.z - summ.z;

    return result;
}

void TreePopulateBranch(vertexParam *vParam, indexParam *iParam, TreeLast root, uint32_t iter, uint32_t max_count, vec3 angle, VertextIterator *vi)
{
    TreeLast branch_res1, branch_res2;

    vec3 branch1_rot = v3_add(root.rotation, angle);

    if(iter % 2)
        branch1_rot.y *= -1;

    branch_res1 = TreeMake3DPeace(vParam, iParam, root.point_vec[1], branch1_rot, root.tree_color, 3, root.radius, 0.01f, vi);
    branch_res1.rotation = branch1_rot;
    branch_res1.leaf_color = root.leaf_color;
    branch_res1.tree_color = root.tree_color;

    if(iter > 1)
        TreePopulateLeafs(vParam, iParam, branch_res1, 6, 9, vi);

    TreeWebLastPoints(iParam, 6, root.point_indx[1], branch_res1.point_indx[0], vi);

    iter++;

    if(iter >= max_count)
        return;

    TreePopulateBranch(vParam, iParam, branch_res1, iter, max_count, angle, vi);

    vec3 branch2_rot = v3_add(root.rotation, v3_muls(angle, -1));

    if(iter % 2)
        branch2_rot.y *= -1;

    branch_res2 = TreeMake3DPeace(vParam, iParam, root.point_vec[2], branch2_rot, root.tree_color, 3, root.radius, 0.01f, vi);
    branch_res2.rotation = branch2_rot;
    branch_res2.leaf_color = root.leaf_color;
    branch_res2.tree_color = root.tree_color;

    if(iter > 1)
        TreePopulateLeafs(vParam, iParam, branch_res2, 6, 9, vi);

    TreeWebLastPoints(iParam, 6, root.point_indx[2], branch_res2.point_indx[0], vi);


    TreePopulateBranch(vParam, iParam, branch_res2, iter, max_count, angle, vi);

}

void TreeObjectInit(TreeObject *to, uint32_t type, DrawParam *dParam, void *arg)
{

    TreeParams *params = arg;

    GameObject3DInit(to);

    to->go.graphObj.shapes[0].bindingDescription = &BindTree3DDescription;
    to->go.graphObj.shapes[0].attr = treeAttributeDescription;
    to->go.graphObj.shapes[0].countAttr = 4;

    if(type == ENGINE_TREE_OBJECT_TYPE_VERTEX)
    {
        vertexParam vParam;
        memset(&vParam, 0, sizeof(vertexParam));
        indexParam iParam;
        memset(&iParam, 0, sizeof(indexParam));
        VertextIterator vi;
        memset(&vi, 0, sizeof(VertextIterator));

        uint32_t buff_size = UINT16_MAX;

        vParam.vertices = calloc(buff_size, sizeof(TreeVertex3D));
        iParam.indices = calloc(buff_size * 3, sizeof(uint32_t));

        //InitTreeVertices(&vParam, &iParam, &vi);

        TreeLast root = TreeMake3DPeace(&vParam, &iParam, vec3_f(0, 0, 0), params->start_rotation, params->tree_color, params->height, params->radius, params->grad, &vi);
        root.tree_color = params->tree_color;
        root.leaf_color = params->leaf_color;

        TreePopulateBranch(&vParam, &iParam, root, 0, params->max_branches, params->mutable_rotation, &vi);

        GraphicsObjectSetVertex(&to->go.graphObj, vParam.vertices, vi.v_index, sizeof(TreeVertex3D), iParam.indices, vi.i_index, sizeof(uint32_t));

        free(vParam.vertices);
        free(iParam.indices);
    }else{
        GraphicsObjectSetVertex(&to->go.graphObj, cubeVert, 24, sizeof(TreeVertex3D), cubeIndx, 36, sizeof(uint32_t));
    }
}

void TreeObjectInitInstances(GameObject3D *go){

    VkDeviceSize bufferSize;

    GraphicsObjectInit(&go->graphObj, ENGINE_VERTEX_TYPE_TREE_INSTANCE);

    memset(go->instances, 0, sizeof(VertexInstance3D) * MAX_INSTANCES);

    bufferSize = sizeof(VertexInstance3D) * MAX_INSTANCES;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &go->buffer.buffer, &go->buffer.buffer_memory, ENGINE_BUFFER_ALLOCATE_VERTEX);

}

void TreeObjectSetInstanceDescriptor(TreeObject *to, DrawParam *dParam){

    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_tree_instance_vert_spv_start, (size_t)(&_binary_shaders_tree_instance_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_tree_instance_frag_spv_start, (size_t)(&_binary_shaders_tree_instance_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_NONE;
    setting.poligonMode = VK_POLYGON_MODE_FILL;
    //setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    //setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;
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
    setting.poligonMode = VK_POLYGON_MODE_FILL;
    //setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    //setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;
}

void TreeObjectInitDefault(TreeObject *to, uint32_t type, DrawParam *dParam, void *arg)
{
    TreeObjectInit(to, type, dParam, arg);

    TreeObjectSetDefaultDescriptor(to, type, dParam);

    GameObject3DInitDraw(to);
}
