#include "tools.h"

#include <vulkan/vulkan.h>

#include <math.h>
#include <string.h>

#include "e_math.h"

int p[512] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
    8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203,
    117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
    165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
    105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132,
    187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186,
    3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59,
    227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70,
    221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178,
    185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
    81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176,
    115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195,
    78, 66, 215, 61, 156, 180,

    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
    8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203,
    117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
    165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
    105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132,
    187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186,
    3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59,
    227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70,
    221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178,
    185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
    81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176,
    115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195,
    78, 66, 215, 61, 156, 180,
};

const double epsilon = 2.718281828182818281828;

void* beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(void* commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}


bool isComplete(QueueFamilyIndices self) {
        return self.graphicsFamily >= 0 && self.presentFamily >= 0;
}

QueueFamilyIndices findQueueFamilies(void* arg) {

    VkPhysicalDevice device = arg;

    QueueFamilyIndices indices;
    indices.graphicsFamily = 0;
    indices.presentFamily = 0;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*) malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);




    for (int i=0; i < queueFamilyCount; i++){

        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        bool presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (isComplete(indices)) {
            break;
        }
    }

    free(queueFamilies);

    return indices;
}

shader readFile(const char* filename) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("failed to open file : %s \n", filename);
        exit(1);
    }

    size_t size = 0;

    fseek(file, 0, SEEK_END );
    size = ftell(file);
    rewind(file);

    uint32_t* temp = (uint32_t*) calloc(size, sizeof(uint32_t));

    while(1)
    {
        fread(temp, sizeof(int),size, file);

        if(feof(file)) break;
    }

    fclose(file);

    shader shdr = {temp, size};

    return shdr;
}

void* createShaderModule(shader shdr) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shdr.size;
    createInfo.pCode = shdr.code;
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        printf("failed to create shader module!");
        exit(1);
    }
    return shaderModule;
}

void InitPlane3D(vertexParam *vParam, indexParam *iParam, int stackCount, int sectorCount){

    vec3 pos = {0 , 0, 0};
    vec3 col = {0.3 , 0.1, 0.11};

    int i, j;

    vParam->verticesSize = ((stackCount + 1) * (sectorCount + 1)) * 2;

    vParam->vertices = (Vertex3D *) calloc(vParam->verticesSize, sizeof(Vertex3D));

    int vIter = 0;

    Vertex3D *verts = vParam->vertices;

    for(i=0; i <= stackCount;i++){
        for(j=0; j <= sectorCount;j++){

            vIter = i * sectorCount + (i > 0 ? j + i : j);

            pos.x = 0.5 * i;
            pos.z = 0.5 * j;

            verts[vIter].position = pos;
            verts[vIter].color = (vec3){1.0f, 1.0f, 1.0f};
            verts[vIter].normal = (vec3){0,1,0};


            verts[vIter].texCoord = (vec2){(float)i / sectorCount, (float)j / stackCount};

        }
    }

    iParam->indexesSize = (stackCount * sectorCount) * 2 * 6 + 6;

    iParam->indices = (uint32_t *) calloc(iParam->indexesSize, sizeof(uint32_t));

    int k1, k2, it = 0, tt = 0;

    for(i=0; i < stackCount;i++){
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 +  sectorCount + 1 ;      // beginning of next stack
        for(j=0; j < sectorCount; ++j, ++k1, ++k2){

            iParam->indices[it] = k1 + 1;
            iParam->indices[it + 1] = k2;
            iParam->indices[it + 2] = k1;
            it +=3;

            iParam->indices[it] = k1 + 1;
            iParam->indices[it + 1] = k2 + 1;
            iParam->indices[it + 2] = k2;

            it +=3;


        }
    }
}

void InitTerrain3D(vertexParam *vParam, indexParam *iParam, int rows, int colmns, int cell_step){

    if(rows < 4)
        rows = 4;

    if(colmns < 4)
        colmns = 4;

    vec3 pos = {0 , 0, 0};
    vec3 col = {0.3 , 0.1, 0.11};

    int i, j;

    vParam->verticesSize = ((rows + 1) * (colmns + 1)) * 2;

    vParam->vertices = (Vertex3D *) calloc(vParam->verticesSize, sizeof(Vertex3D));

    int vIter = 0;

    Vertex3D *verts = vParam->vertices;

    for(i=0; i <= rows;i++){
        for(j=0; j <= colmns;j++){

            vIter = i * colmns + (i > 0 ? j + i : j);

            pos.x = 0.5 * i;
            pos.z = 0.5 * j;

            verts[vIter].position = pos;
            verts[vIter].color = (vec3){1.0f, 1.0f, 1.0f};
            verts[vIter].normal = (vec3){0,1,0};


            verts[vIter].texCoord = (vec2){(float)i / (colmns / cell_step), (float)j / (rows / cell_step)};

        }
    }

    iParam->indexesSize = (rows * rows) * 2 * 6 + 6;

    iParam->indices = (uint32_t *) calloc(iParam->indexesSize, sizeof(uint32_t));

    int k1, k2, it = 0, tt = 0;

    for(i=0; i < rows;i++){
        k1 = i * (rows + 1);     // beginning of current stack
        k2 = k1 +  rows + 1 ;      // beginning of next stack
        for(j=0; j < rows; ++j, ++k1, ++k2){

            iParam->indices[it] = k1 + 1;
            iParam->indices[it + 1] = k2;
            iParam->indices[it + 2] = k1;
            it +=3;

            iParam->indices[it] = k1 + 1;
            iParam->indices[it + 1] = k2 + 1;
            iParam->indices[it + 2] = k2;

            it +=3;


        }
    }
}

float* computeIcosahedronVertices(float radius)
{
    const float H_ANGLE = M_PI / 180 * 72;    // 72 degree = 360 / 5
    const float V_ANGLE = atanf(1.0f / 2);  // elevation = 26.565 degree

    float *vertices = (float *)calloc(12 * 3, sizeof(float));    // 12 vertices
    int i1, i2;                             // indices
    float z, xy;                            // coords
    float hAngle1 = -M_PI / 2 - H_ANGLE / 2;  // start from -126 deg at 2nd row
    float hAngle2 = -M_PI / 2;                // start from -90 deg at 3rd row

    // the first top vertex (0, 0, r)
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = radius;

    // 10 vertices at 2nd and 3rd rows
    for(int i = 1; i <= 5; ++i)
    {
        i1 = i * 3;         // for 2nd row
        i2 = (i + 5) * 3;   // for 3rd row

        z = radius * sinf(V_ANGLE);             // elevaton
        xy = radius * cosf(V_ANGLE);

        vertices[i1] = xy * cosf(hAngle1);      // x
        vertices[i2] = xy * cosf(hAngle2);
        vertices[i1 + 1] = xy * sinf(hAngle1);  // x
        vertices[i2 + 1] = xy * sinf(hAngle2);
        vertices[i1 + 2] = z;                   // z
        vertices[i2 + 2] = -z;

        // next horizontal angles
        hAngle1 += H_ANGLE;
        hAngle2 += H_ANGLE;
    }

    // the last bottom vertex (0, 0, -r)
    i1 = 11 * 3;
    vertices[i1] = 0;
    vertices[i1 + 1] = 0;
    vertices[i1 + 2] = -radius;

    return vertices;
}

void addVertices3(Vertex3D* verts, float v1[3], float v2[3], float v3[3], int current){
    verts[current + 0].position.x = v1[0];
    verts[current + 0].position.y = v1[1];
    verts[current + 0].position.z = v1[2];
    verts[current + 1].position.x = v2[0];
    verts[current + 1].position.y = v2[1];
    verts[current + 1].position.z = v2[2];
    verts[current + 2].position.x = v3[0];
    verts[current + 2].position.y = v3[1];
    verts[current + 2].position.z = v3[2];
}

void addIndices(uint32_t* indxs, int indx1, int indx2, int indx3, int current){

    indxs[current + 0] = indx1;
    indxs[current + 1] = indx2;
    indxs[current + 2] = indx3;
}

void addTexCoords3(Vertex3D* verts, float t1[2], float t2[2], float t3[2], int current){
    verts[current + 0].texCoord.x = t1[0];
    verts[current + 0].texCoord.y = t1[1];
    verts[current + 1].texCoord.x = t2[0];
    verts[current + 1].texCoord.y = t2[1];
    verts[current + 2].texCoord.x = t3[0];
    verts[current + 2].texCoord.y = t3[1];
}

float computeScaleForLength(const float v[3], float length)
{
    // and normalize the vector then re-scale to new radius
    return length / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void computeHalfVertex(const float v1[3], const float v2[3], float length, float newV[3])
{
    newV[0] = v1[0] + v2[0];
    newV[1] = v1[1] + v2[1];
    newV[2] = v1[2] + v2[2];
    float scale = computeScaleForLength(newV, length);
    newV[0] *= scale;
    newV[1] *= scale;
    newV[2] *= scale;
}

///////////////////////////////////////////////////////////////////////////////
// find middle texcoords of 2 tex coords and return new coord (3rd param)
///////////////////////////////////////////////////////////////////////////////
void computeHalfTexCoord(const float t1[2], const float t2[2], float newT[2])
{
    newT[0] = (t1[0] + t2[0]) * 0.5f;
    newT[1] = (t1[1] + t2[1]) * 0.5f;
}

void subdivideVerticesFlat(vertexParam *vParam, indexParam *iParam, int subdivision, float radius){
    Vertex3D * tmpVerts;
    uint32_t* tmpIndxs;
    int indexCount;
    const float *v1, *v2, *v3;          // ptr to original vertices of a triangle
    const float *t1, *t2, *t3;          // ptr to original texcoords of a triangle
    float newV1[3], newV2[3], newV3[3]; // new vertex positions
    float newT1[2], newT2[2], newT3[2]; // new texture coords
    float normal[3];                    // new face normal
    unsigned int index = 0, curr = 0;             // new index value
    int i, j;

    // iteration
    for(i = 1; i <= subdivision; ++i)
    {
        tmpVerts = (Vertex3D *)calloc(vParam->verticesSize, sizeof(Vertex3D));
        tmpIndxs = (uint32_t *)calloc(iParam->indexesSize, sizeof(uint32_t));
        memcpy(tmpVerts, vParam->vertices, vParam->verticesSize * sizeof(Vertex3D));
        memcpy(tmpIndxs, iParam->indices, iParam->indexesSize * sizeof(uint32_t));

        free(vParam->vertices);
        free(iParam->indices);

        vParam->vertices = (Vertex3D *)calloc(vParam->verticesSize * 4, sizeof(Vertex3D));
        iParam->indices = (uint32_t *)calloc(iParam->indexesSize * 4, sizeof(uint32_t));

        index = 0;
        curr = 0;
        indexCount = iParam->indexesSize;//(int)tmpIndices.size();

        for(j = 0; j < indexCount; j += 3)
        {
            // get 3 vertice and texcoords of a triangle
            v1 = &tmpVerts[tmpIndxs[j]].position;
            v2 = &tmpVerts[tmpIndxs[j + 1]].position;
            v3 = &tmpVerts[tmpIndxs[j + 2]].position;
            t1 = &tmpVerts[tmpIndxs[j]].texCoord;
            t2 = &tmpVerts[tmpIndxs[j + 1]].texCoord;
            t3 = &tmpVerts[tmpIndxs[j + 2]].texCoord;

            // get 3 new vertices by spliting half on each edge
            computeHalfVertex(v1, v2, radius, newV1);
            computeHalfVertex(v2, v3, radius, newV2);
            computeHalfVertex(v1, v3, radius, newV3);
            computeHalfTexCoord(t1, t2, newT1);
            computeHalfTexCoord(t2, t3, newT2);
            computeHalfTexCoord(t1, t3, newT3);

            // add 4 new triangles
            addVertices3(vParam->vertices, v1, newV1, newV3, curr);
            addTexCoords3(vParam->vertices, t1, newT1, newT3, curr);
            //computeFaceNormal(v1, newV1, newV3, normal);
            //addNormals(normal, normal, normal)
            addIndices(iParam->indices, index, index+1, index+2, curr);
            curr +=3;


            addVertices3(vParam->vertices, newV1, v2, newV2, curr);
            addTexCoords3(vParam->vertices, newT1, t2, newT2, curr);
            //computeFaceNormal(newV1, v2, newV2, normal);
            //addNormals(normal, normal, normal);
            addIndices(iParam->indices, index+3, index+4, index+5, curr);
            curr +=3;

            addVertices3(vParam->vertices, newV1, newV2, newV3, curr);
            addTexCoords3(vParam->vertices, newT1, newT2, newT3, curr);
            //computeFaceNormal(newV1, newV2, newV3, normal);
            //addNormals(normal, normal, normal);
            addIndices(iParam->indices, index+6, index+7, index+8, curr);
            curr +=3;

            addVertices3(vParam->vertices, newV3, newV2, v3, curr);
            addTexCoords3(vParam->vertices, newT3, newT2, t3, curr);
            //computeFaceNormal(newV3, newV2, v3, normal);
            //addNormals(normal, normal, normal);
            addIndices(iParam->indices, index+9, index+10, index+11, curr);
            curr +=3;

            // add new line indices per iteration
            //addSubLineIndices(index, index+1, index+4, index+5, index+11, index+9); //CCW

            // next index
            index += 12;
        }
        iParam->indexesSize = iParam->indexesSize * 4;
        vParam->verticesSize = vParam->verticesSize * 4;
        free(tmpIndxs);
        free(tmpVerts);
    }

    return;

}

int IcoSphereGenerator(vertexParam *vParam, indexParam *iParam,float radius)
{

    vParam->verticesSize = 60;
    vParam->vertices = (Vertex3D *) calloc(60, sizeof(Vertex3D));
    iParam->indexesSize = 60;
    iParam->indices = (uint32_t *) calloc(60, sizeof(uint32_t));

    const float S_STEP = 186 / 2048.0f;     // horizontal texture step
    const float T_STEP = 322 / 1024.0f;     // vertical texture step

    // compute 12 vertices of icosahedron
    float* tmpVertices = computeIcosahedronVertices(radius);

    // clear memory of prev arrays

    const float *v0, *v1, *v2, *v3, *v4, *v11;          // vertex positions
    float n[3];                                         // face normal
    float t0[2], t1[2], t2[2], t3[2], t4[2], t11[2], tt[2];    // texCoords
    unsigned int index = 0, curr =0;

    // compute and add 20 tiangles of icosahedron first
    v0 = &tmpVertices[0];       // 1st vertex
    v11 = &tmpVertices[11 * 3]; // 12th vertex
    for(int i = 1; i <= 5; ++i)
    {
        // 4 vertices in the 2nd row
        v1 = &tmpVertices[i * 3];
        if(i < 5)
            v2 = &tmpVertices[(i + 1) * 3];
        else
            v2 = &tmpVertices[3];

        v3 = &tmpVertices[(i + 5) * 3];
        if((i + 5) < 10)
            v4 = &tmpVertices[(i + 6) * 3];
        else
            v4 = &tmpVertices[6 * 3];

        // texture coords
        t0[0] = (2 * i - 1) * S_STEP;   t0[1] = 0;
        t1[0] = (2 * i - 2) * S_STEP;   t1[1] = T_STEP;
        t2[0] = (2 * i - 0) * S_STEP;   t2[1] = T_STEP;
        t3[0] = (2 * i - 1) * S_STEP;   t3[1] = T_STEP * 2;
        t4[0] = (2 * i + 1) * S_STEP;   t4[1] = T_STEP * 2;
        t11[0]= 2 * i * S_STEP;         t11[1]= T_STEP * 3;

        // add a triangle in 1st row
        //Icosphere::computeFaceNormal(v0, v1, v2, n);
        addVertices3(vParam->vertices, v0, v1, v2, curr);
        //addNormals(n, n, n);
        addTexCoords3(vParam->vertices, t0, t1, t2, curr);
        addIndices(iParam->indices, index, index+1, index+2, curr);
        curr +=3;

        // add 2 triangles in 2nd row
        //Icosphere::computeFaceNormal(v1, v3, v2, n);
        addVertices3(vParam->vertices, v1, v3, v2, curr);
        //addNormals(n, n, n);
        addTexCoords3(vParam->vertices, t1, t3, t2, curr);
        addIndices(iParam->indices, index+3, index+4, index+5, curr);
        curr +=3;

        //Icosphere::computeFaceNormal(v2, v3, v4, n);
        addVertices3(vParam->vertices, v2, v3, v4, curr);
        //addNormals(n, n, n);
        addTexCoords3(vParam->vertices, t2, t3, t4, curr);
        addIndices(iParam->indices, index+6, index+7, index+8, curr);
        curr +=3;

        // add a triangle in 3rd row
        //Icosphere::computeFaceNormal(v3, v11, v4, n);
        addVertices3(vParam->vertices, v3, v11, v4, curr);
        //addNormals(n, n, n);
        addTexCoords3(vParam->vertices, t3, t11, t4, curr);
        addIndices(iParam->indices, index+9, index+10, index+11, curr);
        curr +=3;

        /*// add 6 edge lines per iteration
        //  i
        //  /   /   /   /   /       : (i, i+1)                              //
        // /__ /__ /__ /__ /__                                              //
        // \  /\  /\  /\  /\  /     : (i+3, i+4), (i+3, i+5), (i+4, i+5)    //
        //  \/__\/__\/__\/__\/__                                            //
        //   \   \   \   \   \      : (i+9,i+10), (i+9, i+11)               //
        //    \   \   \   \   \                                             //
        lineIndices.push_back(index);       // (i, i+1)
        lineIndices.push_back(index+1);       // (i, i+1)
        lineIndices.push_back(index+3);     // (i+3, i+4)
        lineIndices.push_back(index+4);
        lineIndices.push_back(index+3);     // (i+3, i+5)
        lineIndices.push_back(index+5);
        lineIndices.push_back(index+4);     // (i+4, i+5)
        lineIndices.push_back(index+5);
        lineIndices.push_back(index+9);     // (i+9, i+10)
        lineIndices.push_back(index+10);
        lineIndices.push_back(index+9);     // (i+9, i+11)
        lineIndices.push_back(index+11);

        // next index*/
        index += 12;
    }

    subdivideVerticesFlat(vParam, iParam, 2, radius);
    //free(tmpVertices);

    return 1;
}

float* getUnitPositiveX(unsigned int pointsPerRow)
{
    const float DEG2RAD = M_PI / 180.0f;

    float* vertices = (float *)calloc(pointsPerRow * pointsPerRow * 3, sizeof(float));
    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along y-axis
    float a2;           // latitudinal angle
    float scale;

    int iter;
    // rotate latitudinal plane from 45 to -45 degrees along Z-axis
    for(unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis
        for(unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            scale = computeScaleForLength(v, 1);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            iter = (i * pointsPerRow + j) * 3;

            vertices[iter] = v[0];
            vertices[iter + 1] = v[1];
            vertices[iter + 2] = v[2];

            // DEBUG
            //std::cout << "vertex: (" << v[0] << ", " << v[1] << ", " << v[2] << "), "
            //          << sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) << std::endl;
        }
    }

    return vertices;
}

void scaleVertex(float v[3], float scale)
{
    v[0] *= scale;
    v[1] *= scale;
    v[2] *= scale;
}

void addVertices4(Vertex3D* verts, float v1[3], float v2[3], float v3[3], float v4[3], int current){
    verts[current + 0].position.x = v1[0];
    verts[current + 0].position.y = v1[1];
    verts[current + 0].position.z = v1[2];
    verts[current + 1].position.x = v2[0];
    verts[current + 1].position.y = v2[1];
    verts[current + 1].position.z = v2[2];
    verts[current + 2].position.x = v3[0];
    verts[current + 2].position.y = v3[1];
    verts[current + 2].position.z = v3[2];
    verts[current + 3].position.x = v4[0];
    verts[current + 3].position.y = v4[1];
    verts[current + 3].position.z = v4[2];
}

void addTexCoords4(Vertex3D* verts, float t1[2], float t2[2], float t3[2], float t4[2], int current){
    verts[current + 0].texCoord.x = t1[0];
    verts[current + 0].texCoord.y = t1[1];
    verts[current + 1].texCoord.x = t2[0];
    verts[current + 1].texCoord.y = t2[1];
    verts[current + 2].texCoord.x = t3[0];
    verts[current + 2].texCoord.y = t3[1];
    verts[current + 3].texCoord.x = t4[0];
    verts[current + 3].texCoord.y = t4[1];
}

void addTexCoords2(Vertex3D* verts, float t1[2], float t2[2], int current){
    verts[current + 0].texCoord.x = t1[0];
    verts[current + 0].texCoord.y = t1[1];
    verts[current + 1].texCoord.x = t2[0];
    verts[current + 1].texCoord.y = t2[1];
}

void addVertex(Vertex3D* verts, float x, float y, float z, int current)
{
    verts[current].position.x = x;
    verts[current].position.y = y;
    verts[current].position.z = z;
}

void addTexCoord(Vertex3D* verts, float s, float t, int current)
{
    verts[current].texCoord.x = s;
    verts[current].texCoord.y = t;
}

int Cubesphere(vertexParam *vParam, indexParam *iParam, float radius,int vertexCountPerRow){
    // generate unit-length verties in +X face
        float* unitVertices = getUnitPositiveX(vertexCountPerRow);

        vParam->verticesSize = ((vertexCountPerRow - 1) * (vertexCountPerRow - 1)) * 4;
        vParam->vertices = (Vertex3D *) calloc(vParam->verticesSize, sizeof(Vertex3D));
        iParam->indexesSize = ((vertexCountPerRow - 1) * (vertexCountPerRow - 1)) * 6;
        iParam->indices = (uint32_t *) calloc(iParam->indexesSize, sizeof(uint32_t));

        unsigned int k = 0, k1, k2, i1, i2, curr = 0, currI = 0; // indices
        float v1[3], v2[3], v3[3], v4[3];   // tmp vertices
        float t1[2], t2[2], t3[2], t4[2];   // texture coords
        float n[3];                         // normal vector

        // +X face
        for(unsigned int i = 0; i < vertexCountPerRow - 1; ++i)
        {
            k1 = i * vertexCountPerRow;              // index at curr row
            k2 = k1 + vertexCountPerRow;             // index at next row

            // vertical tex coords
            t1[1] = t3[1] = (float)i / (vertexCountPerRow - 1);
            t2[1] = t4[1] = (float)(i+1) / (vertexCountPerRow - 1);

            for(unsigned int j = 0; j < vertexCountPerRow - 1; ++j, ++k1, ++k2)
            {
                i1 = k1 * 3;
                i2 = k2 * 3;

                // 4 vertices of a quad
                // v1--v3
                // | / |
                // v2--v4
                v1[0] = unitVertices[i1];
                v1[1] = unitVertices[i1+1];
                v1[2] = unitVertices[i1+2];
                v2[0] = unitVertices[i2];
                v2[1] = unitVertices[i2+1];
                v2[2] = unitVertices[i2+2];
                v3[0] = unitVertices[i1+3];
                v3[1] = unitVertices[i1+4];
                v3[2] = unitVertices[i1+5];
                v4[0] = unitVertices[i2+3];
                v4[1] = unitVertices[i2+4];
                v4[2] = unitVertices[i2+5];

                // compute face nornal
                //Cubesphere::computeFaceNormal(v1, v2, v3, n);

                // resize vertices by sphere radius
                scaleVertex(v1, radius);
                scaleVertex(v2, radius);
                scaleVertex(v3, radius);
                scaleVertex(v4, radius);

                // compute horizontal tex coords
                t1[0] = t2[0] = (float)j / (vertexCountPerRow - 1);
                t3[0] = t4[0] = (float)(j+1) / (vertexCountPerRow - 1);

                // add 4 vertex attributes
                addVertices4(vParam->vertices, v1, v2, v3, v4, curr);
                //addNormals(n, n, n, n);
                addTexCoords4(vParam->vertices, t1, t2, t3, t4, curr);
                curr +=4;

                // add indices of 2 triangles
                addIndices(iParam->indices, k, k+1, k+2, currI);
                currI +=3;
                addIndices(iParam->indices, k+2, k+1, k+3, currI);
                currI +=3;

                // add line indices; top and left
                /*lineIndices.push_back(k);       // left
                lineIndices.push_back(k+1);
                lineIndices.push_back(k);       // top
                lineIndices.push_back(k+2);*/

                k += 4;     // next
            }
        }

        // array size and index for building next face
        unsigned int startIndex;                    // starting index for next face
        int vertexSize = (int)vParam->verticesSize;      // vertex array size of +X face
        int indexSize = (int)iParam->indexesSize;        // index array size of +X face
        //int lineIndexSize = (int)lineIndices.size(); // line index size of +X face

        // build -X face by negating x and z values
        startIndex = vParam->verticesSize;
        vParam->verticesSize = vParam->verticesSize + vertexSize;
        Vertex3D *verts = vParam->vertices = (Vertex3D *)realloc(vParam->vertices, vParam->verticesSize * sizeof(Vertex3D));
        memset(vParam->vertices + startIndex, +  0, sizeof(Vertex3D) * vertexSize);
        for(int i = 0; i < vertexSize; i ++)
        {
            addVertex(verts, -verts[i].position.x, verts[i].position.y, -verts[i].position.z, curr);
            addTexCoord(verts, verts[i].texCoord.x, verts[i].texCoord.y, curr);
            //addNormal(-normals[i], normals[i+1], -normals[i+2]);
            curr ++;
        }
        iParam->indexesSize = iParam->indexesSize + indexSize;
        iParam->indices = (uint32_t *)realloc(iParam->indices, iParam->indexesSize * sizeof(uint32_t));
        for(int i = 0; i < indexSize; ++i)
        {
            iParam->indices[currI] = startIndex + iParam->indices[i];
            currI++;
        }

        /*for(int i = 0; i < lineIndexSize; i += 4)
        {
            // left and bottom lines
            lineIndices.push_back(startIndex + i);      // left
            lineIndices.push_back(startIndex + i + 1);
            lineIndices.push_back(startIndex + i + 1);  // bottom
            lineIndices.push_back(startIndex + i + 3);
        }*/

        // build +Y face by swapping x=>y, y=>-z, z=>-x
        startIndex = vParam->verticesSize;
        vParam->verticesSize = vParam->verticesSize + vertexSize;
        verts = vParam->vertices = (Vertex3D *)realloc(vParam->vertices, vParam->verticesSize * sizeof(Vertex3D));
        memset(vParam->vertices + startIndex, +  0, sizeof(Vertex3D) * vertexSize);
        for(int i = 0; i < vertexSize; i ++)
        {
            addVertex(verts, -verts[i].position.z, verts[i].position.x, -verts[i].position.y, curr);
            addTexCoord(verts, verts[i].texCoord.x, verts[i].texCoord.y, curr);
            //addNormal(-normals[i+2], normals[i], -normals[i+1]);
            curr ++;
        }
        iParam->indexesSize = iParam->indexesSize + indexSize;
        iParam->indices = (uint32_t *)realloc(iParam->indices, iParam->indexesSize * sizeof(uint32_t));
        for(int i = 0; i < indexSize; ++i)
        {
            iParam->indices[currI] = startIndex + iParam->indices[i];
            currI++;
        }

        /*for(int i = 0; i < lineIndexSize; ++i)
        {
            // top and left lines (same as +X)
            lineIndices.push_back(startIndex + lineIndices[i]);
        }*/

        // build -Y face by swapping x=>-y, y=>z, z=>-x
        startIndex = vParam->verticesSize;
        vParam->verticesSize = vParam->verticesSize + vertexSize;
        verts = vParam->vertices = (Vertex3D *)realloc(vParam->vertices, vParam->verticesSize * sizeof(Vertex3D));
        memset(vParam->vertices + startIndex, +  0, sizeof(Vertex3D) * vertexSize);
        for(int i = 0; i < vertexSize; i ++)
        {
            addVertex(verts, -verts[i].position.z, -verts[i].position.x, verts[i].position.y, curr);
            addTexCoord(verts, verts[i].texCoord.x, verts[i].texCoord.y, curr);
            //addNormal(-normals[i+2], -normals[i], normals[i+1]);
            curr ++;
        }
        iParam->indexesSize = iParam->indexesSize + indexSize;
        iParam->indices = (uint32_t *)realloc(iParam->indices, iParam->indexesSize * sizeof(uint32_t));
        for(int i = 0; i < indexSize; ++i)
        {
            iParam->indices[currI] = startIndex + iParam->indices[i];
            currI++;
        }

        /*for(int i = 0; i < lineIndexSize; i += 4)
        {
            // top and right lines
            lineIndices.push_back(startIndex + i);      // top
            lineIndices.push_back(startIndex + i + 2);
            lineIndices.push_back(startIndex + 2 + i);  // right
            lineIndices.push_back(startIndex + 3 + i);
        }*/

        // build +Z face by swapping x=>z, z=>-x
        startIndex = vParam->verticesSize;
        vParam->verticesSize = vParam->verticesSize + vertexSize;
        verts = vParam->vertices = (Vertex3D *)realloc(vParam->vertices, vParam->verticesSize * sizeof(Vertex3D));
        memset(vParam->vertices + startIndex, +  0, sizeof(Vertex3D) * vertexSize);
        for(int i = 0; i < vertexSize; i ++)
        {
            addVertex(verts, -verts[i].position.z, verts[i].position.y, verts[i].position.x, curr);
            addTexCoord(verts, verts[i].texCoord.x, verts[i].texCoord.y, curr);
            //addNormal(-normals[i+2], normals[i+1], normals[i]);
            curr ++;
        }
        iParam->indexesSize = iParam->indexesSize + indexSize;
        iParam->indices = (uint32_t *)realloc(iParam->indices, iParam->indexesSize * sizeof(uint32_t));
        for(int i = 0; i < indexSize; ++i)
        {
            iParam->indices[currI] = startIndex + iParam->indices[i];
            currI++;
        }

        /*for(int i = 0; i < lineIndexSize; ++i)
        {
            // top and left lines (same as +X)
            lineIndices.push_back(startIndex + lineIndices[i]);
        }*/

        // build -Z face by swapping x=>-z, z=>x
        startIndex = vParam->verticesSize;
        vParam->verticesSize = vParam->verticesSize + vertexSize;
        verts = vParam->vertices = (Vertex3D *)realloc(vParam->vertices, vParam->verticesSize * sizeof(Vertex3D));
        memset(vParam->vertices + startIndex, +  0, sizeof(Vertex3D) * vertexSize);
        for(int i = 0; i < vertexSize; i ++)
        {
            addVertex(verts, verts[i].position.z, verts[i].position.y, -verts[i].position.x, curr);
            addTexCoord(verts, verts[i].texCoord.x, verts[i].texCoord.y, curr);
            //addNormal(normals[i+2], normals[i+1], -normals[i]);
            curr ++;
        }
        iParam->indexesSize = iParam->indexesSize + indexSize;
        iParam->indices = (uint32_t *)realloc(iParam->indices, iParam->indexesSize * sizeof(uint32_t));
        for(int i = 0; i < indexSize; ++i)
        {
            iParam->indices[currI] = startIndex + iParam->indices[i];
            currI++;
        }

        /*for(int i = 0; i < lineIndexSize; i += 4)
        {
            // left and bottom lines
            lineIndices.push_back(startIndex + i);      // left
            lineIndices.push_back(startIndex + i + 1);
            lineIndices.push_back(startIndex + i + 1);  // bottom
            lineIndices.push_back(startIndex + i + 3);
        }*/

        return 1;

}

int SphereGenerator3D(vertexParam *vParam, indexParam *iParam,float radius, int stackCount, int sectorCount){
    float x, y, z, xz;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    vParam->verticesSize = ((stackCount + 1) * (sectorCount + 1)) * 2;

    vParam->vertices = (Vertex3D *) calloc(vParam->verticesSize, sizeof(Vertex3D));

    int vIter = 0;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xz = radius * -cosf(stackAngle);             // r * cos(u)
        y = radius * -sinf(stackAngle);              // r * sin(u)

        Vertex3D *verts = vParam->vertices;

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            vIter = i * sectorCount + (i > 0 ? j + i : j);

            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
            z = xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
            verts[vIter].position = (vec3){x, y, z};

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            verts[vIter].normal = (vec3){nx, ny, nz};

            // vertex tex coord (s, t) range between [0, 1]

            s = (float)j / stackCount;
            t = (float)i / sectorCount;

            verts[vIter].texCoord = (vec2){s, t};

            verts[vIter].color = (vec3){1,1,1};
        }
    }

    iParam->indexesSize = (stackCount * sectorCount) * 2 * 6 + 6;

    iParam->indices = (uint32_t *) calloc(iParam->indexesSize, sizeof(uint32_t));

    int k1, k2;
    int it = 0, tt = 0;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1 ;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1

            iParam->indices[it] = k1;
            iParam->indices[it + 1] = k2;
            iParam->indices[it + 2] = k1 + 1;
            it +=3;

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                iParam->indices[it] = k1 + 1;
                iParam->indices[it + 1] = k2;
                iParam->indices[it + 2] = k2 + 1;
                it +=3;
            }
        }
    }

    return it;
}

void ConeGenerator(vertexParam *vParam, indexParam *iParam, const float height, int stackCount, int sectorCount) {

    vec3 pos = {0,0,0};

    float x, z;                              // vertex positio

    float angIncSector = 2 * M_PI / sectorCount;
    float angIncStack = 2 * M_PI / stackCount;
    float heigInc = height / stackCount;
    float sectorAngle, stackAngle;

    vParam->verticesSize = ((stackCount + 1) * (sectorCount + 1)) * 2;

    vParam->vertices = (Vertex3D *) calloc(vParam->verticesSize, sizeof(Vertex3D));

    Vertex3D *verts = vParam->vertices;

    int vIter = 0;
    int mulcount = (stackCount + 1) * (sectorCount + 1);
    // calculate points around directrix
    for (int i = 0; i <= stackCount; ++i) {

        stackAngle = i * angIncSector;        // starting from pi/2 to -pi/2

        for(int j = 0; j <= sectorCount; ++j)
        {
            vIter = i * sectorCount + (i > 0 ? j + i : j);

            sectorAngle = j * angIncStack;           // starting from 0 to 2pi

            x = cos(sectorAngle) * stackAngle;             // r * cos(u) * cos(v)
            z = sin(sectorAngle) * stackAngle;              // r * sin(u)

            vec3 posit;
            posit.x = x;
            posit.y = pos.y;
            posit.z = z;
            verts[vIter].position = posit;
            verts[vIter].texCoord = (vec2){(float)i / stackCount, (float)j / sectorCount};
            verts[vIter].color = (vec3){(float)i / stackCount, (float)j / sectorCount, (float)j / sectorCount};
        }
    }

    for (int i = 0; i <= stackCount; ++i) {

        stackAngle = i * angIncSector;        // starting from pi/2 to -pi/2

        for(int j = 0; j <= sectorCount; ++j)
        {
            vIter = mulcount + i * sectorCount + (i > 0 ? j + i : j);

            sectorAngle = j * angIncStack;           // starting from 0 to 2pi

            x = -cos(sectorAngle) * stackAngle;             // r * cos(u) * cos(v)
            z = -sin(sectorAngle) * stackAngle;              // r * sin(u)

            vec3 posit;
            posit.x = x;
            posit.y = pos.y + i * heigInc - height;
            posit.z = z;
            verts[vIter].position = posit;
            verts[vIter].texCoord = (vec2){(float)i / stackCount, (float)j / sectorCount};
            verts[vIter].color = (vec3){(float)i / stackCount, (float)j / sectorCount, (float)j / sectorCount};
        }
    }

    iParam->indexesSize = (stackCount * sectorCount) * 2 * 6;

    iParam->indices = (uint32_t *) calloc(iParam->indexesSize, sizeof(uint32_t));

    int k1, k2;
    int it = 0, tt = 0;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1 ;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1

            iParam->indices[it] = k1 + 1;
            iParam->indices[it + 1] = k2;
            iParam->indices[it + 2] = k1;
            it +=3;

            // k1+1 => k2 => k2+1
            iParam->indices[it] = k2 + 1;
            iParam->indices[it + 1] = k2;
            iParam->indices[it + 2] = k1 + 1;
            it +=3;
        }
    }

    int pl = mulcount;

    for(int i = 0; i < stackCount; ++i)
    {
        k1 = pl + i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1 ;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1

            iParam->indices[it] = k1;
            iParam->indices[it + 1] = k2;
            iParam->indices[it + 2] = k1 + 1;
            it +=3;

            // k1+1 => k2 => k2+1
            iParam->indices[it] = k1 + 1;
            iParam->indices[it + 1] = k2;
            iParam->indices[it + 2] = k2 + 1;
            it +=3;
        }
    }

}

float dot_grad(int hash, float xf){
        // In 1D case, the gradient may be either 1 or -1
        // The distance vector is the input offset (relative to the smallest bound)
        return (hash & 0x1) ? xf : -xf;
}

float dot_grad2(int hash, float xf, float yf){
    // In 2D case, the gradient may be any of 8 direction vectors pointing to the
    // edges of a unit-square. The distance vector is the input offset (relative to
    // the smallest bound)
    switch (hash & 0x7) {
        case 0x0: return  xf + yf;
        case 0x1: return  xf;
        case 0x2: return  xf - yf;
        case 0x3: return -yf;
        case 0x4: return -xf - yf;
        case 0x5: return -xf;
        case 0x6: return -xf + yf;
        case 0x7: return  yf;
        default:  return  0.0f;
    }
}

vec3 perp(const vec3 v) {
    float min = fabs(v.x);
    vec3 cardinalAxis = {1, 0, 0};

    if (fabs(v.y) < min) {
        min = fabsf(v.y);
        cardinalAxis = (vec3){0, 1, 0};
    }

    if (fabs(v.z) < min) {
        cardinalAxis = (vec3){0, 0, 1};
    }

    return v3_cross(v, cardinalAxis);
}

float lerp_noise(float a, float b, float t) {
    return a + t * (b - a);
}

float fade(float t){
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float noise1D( float x){
    // Left coordinate of the unit-line that contains the input
    int xi0 = floor(x);

    // Input location in the unit-line
    float xf0 = x - xi0;
    float xf1 = xf0 - 1.0;

    // Wrap to range 0-255
    int const xi = xi0 & 0xFF;

    // Apply the fade function to the location
    float const u = fade(xf0);

    // Generate hash values for each point of the unit-line
    int const h0 = p[xi + 0];
    int const h1 = p[xi + 1];

    // Linearly interpolate between dot products of each gradient with its distance to the input location
    return lerp_noise(dot_grad(h0, xf0), dot_grad(h1, xf1), u);
}

float noise2D(float x, float y)
{
    // Top-left coordinates of the unit-square
    int xi0 = (int)(floor(x)) & 0xFF;
    int yi0 = (int)(floor(y)) & 0xFF;

    // Input location in the unit-square
    float xf0 = x - xi0;
    float yf0 = y - yi0;
    float xf1 = xf0 - 1.0;
    float yf1 = yf0 - 1.0;

    // Wrap to range 0-255
    int const xi = xi0 & 0xFF;
    int const yi = yi0 & 0xFF;

    // Apply the fade function to the location
    float const u = fade(xf0);
    float const v = fade(yf0);

    // Generate hash values for each point of the unit-square
    int const h00 = p[p[xi + 0] + yi + 0];
    int const h01 = p[p[xi + 0] + yi + 1];
    int const h10 = p[p[xi + 1] + yi + 0];
    int const h11 = p[p[xi + 1] + yi + 1];

    // Linearly interpolate between dot products of each gradient with its distance to the input location
    float const x1 = lerp_noise(dot_grad2(h00, xf0, yf0), dot_grad2(h10, xf1, yf0), u);
    float const x2 = lerp_noise(dot_grad2(h01, xf0, yf1), dot_grad2(h11, xf1, yf1), u);
    return lerp_noise(x1, x2, v);
}

float sinWithRange(float value, float minV, float range){
    return ((fabs(sin(value)) + 1) / 2 * range) + minV;
}

float cosWithRange(float value, float minV, float range){
    return ((fabs(cos(value)) + 1) / 2 * range) + minV;
}

void ToolsAddStrings(char *out, int buff_size, char *s1, char *s2){

    char temp[buff_size];

    memcpy(temp, s1, buff_size);

    memset(out, 0, buff_size);

    int len = strlen(temp);
    int len2 = strlen(s2);

    memcpy(out, temp, len);
    memcpy(out + len, s2, len2);

    memset(out + len + len2, 0, 1);

}

bool ToolsCmpStrings(char *in, char *s1){

    char *cursor = in;

    int len = strlen(s1) - 1, iter = 0;;

    while(*cursor == s1[iter] && iter!=len)
    {
        cursor++;
        iter++;
    }

    if(iter == len)
        return true;

    return false;
}

int ToolsStr32BitLength(uint32_t *text)
{
    int size = 0;

    uint32_t *point = text;

    while(*point != 0)
    {
        size ++;
        point++;
    }

    return size;
}

void* ToolsLoadImageFromFile(size_t* len, char *filepath)
{
    FILE* fd;
    int size;

    fd = fopen(filepath, "r");
    if (fd == NULL) {
        printf("File Not Found!\n");
        return -1;
    }


    fseek(fd, 0L, SEEK_END);
    size = ftell(fd);

    char *buff = (char *)calloc(size, sizeof(char));

    fseek(fd, 0L, SEEK_SET);

    fread(buff, sizeof(char), size, fd);

    fflush(fd);

    fclose(fd);

    *len = size;

    return buff;
}

int ToolsStringToUInt32(uint32_t *dest, const char *src)
{
    uint32_t size = strlen(src);

    bool find = false;
    int iter = 0;
    for(int i=0; i < size; i++)
    {
        find = false;
        if(src[i] < 0)
        {
            for(int j=0; j < 66; j++)
            {
                char temp[2];
                temp[0] = src[i + 1];
                temp[1] = src[i];
                uint16_t t_value = *(uint16_t *)temp;
                if(fontIndexes[j].IndexLetter == t_value)
                {
                    dest[iter] = fontIndexes[j].FindLetter;
                    find = true;
                    break;
                }
            }

            i ++;
        }

        if(!find)
            dest[iter] = src[i];

        iter++;
    }

    dest[iter] = 0;

    return iter;
}
