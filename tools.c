#include "tools.h"

#include <math.h>
#include <string.h>

mat4 edenMat= {1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 1, 0,
               0, 0, 0, 1};

const double epsilon = 2.718281828182818281828;

VkCommandBuffer beginSingleTimeCommands() {
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

void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
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

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    indices.graphicsFamily = 0;
    indices.presentFamily = 0;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*) malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);




    for (i=0; i < queueFamilyCount; i++){

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


VkShaderModule createShaderModule(shader shdr) {
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

mat4 m4_rotation_matrix(mat4 matrix, vec3 degrees)
{
  degrees.x *= M_PI / 180;  // convert to radians
  degrees.y *= M_PI / 180;
  degrees.z *= M_PI / 180;

  float sin_x = sin(degrees.x);
  float cos_x = cos(degrees.x);
  float sin_y = sin(degrees.y);
  float cos_y = cos(degrees.y);
  float sin_z = sin(degrees.z);
  float cos_z = cos(degrees.z);

  mat4 result;

  result.m[0][0] = cos_z * cos_y;
  result.m[1][0] = cos_z * sin_y * sin_x + sin_z * cos_x;
  result.m[2][0] = -cos_z * sin_y * cos_x + sin_z * sin_x;
  result.m[3][0] = 0;

  result.m[0][1] = -sin_z * cos_y;
  result.m[1][1] = -sin_z * sin_y * sin_x + cos_z * cos_x;
  result.m[2][1] = sin_z * sin_y * cos_x + cos_z * sin_x;
  result.m[3][1] = 0;

  result.m[0][2] = sin_y;
  result.m[1][2] = -cos_y * sin_x;
  result.m[2][2] = cos_y * cos_x;
  result.m[3][2] = 0;

  result.m[0][3] = 0;
  result.m[1][3] = 0;
  result.m[2][3] = 0;
  result.m[3][3] = 1;

  return result;
}

mat4 m4_look_at(vec3 from, vec3 to, vec3 up) {
    vec3 z = v3_muls(v3_norm(v3_sub(to, from)), -1);
    vec3 x = v3_norm(v3_cross(up, z));
    vec3 y = v3_cross(z, x);

    return mat4_f(
        x.x, x.y, x.z, -v3_dot(from, x),
        y.x, y.y, y.z, -v3_dot(from, y),
        z.x, z.y, z.z, -v3_dot(from, z),
        0,   0,   0,    1
    );
}

mat4 m4_translate(mat4 mat, vec3 pos){

    mat.m[3][0] += pos.x;
    mat.m[3][1] += pos.y;
    mat.m[3][2] += pos.z;

    return mat;
}

mat4 m4_mult(mat4 m1, mat4 m2){
    mat4 result;

    result.m[0][0] = (m1.m[0][0] * m2.m[0][0]) + (m1.m[0][1] * m2.m[1][0]) + (m1.m[0][2] * m2.m[2][0]) + (m1.m[0][3] * m2.m[3][0]);
    result.m[1][0] = (m1.m[0][0] * m2.m[0][1]) + (m1.m[0][1] * m2.m[1][1]) + (m1.m[0][2] * m2.m[2][1]) + (m1.m[0][3] * m2.m[3][1]);
    result.m[2][0] = (m1.m[0][0] * m2.m[0][2]) + (m1.m[0][1] * m2.m[1][2]) + (m1.m[0][2] * m2.m[2][2]) + (m1.m[0][3] * m2.m[3][2]);
    result.m[3][0] = (m1.m[0][0] * m2.m[0][3]) + (m1.m[0][1] * m2.m[1][3]) + (m1.m[0][2] * m2.m[2][3]) + (m1.m[0][3] * m2.m[3][3]);

    result.m[0][1] = (m1.m[1][0] * m2.m[0][0]) + (m1.m[1][1] * m2.m[1][0]) + (m1.m[1][2] * m2.m[2][0]) + (m1.m[1][3] * m2.m[3][0]);
    result.m[1][1] = (m1.m[1][0] * m2.m[0][1]) + (m1.m[1][1] * m2.m[1][1]) + (m1.m[1][2] * m2.m[2][1]) + (m1.m[1][3] * m2.m[3][1]);
    result.m[2][1] = (m1.m[1][0] * m2.m[0][2]) + (m1.m[1][1] * m2.m[1][2]) + (m1.m[1][2] * m2.m[2][2]) + (m1.m[1][3] * m2.m[3][2]);
    result.m[3][1] = (m1.m[1][0] * m2.m[0][3]) + (m1.m[1][1] * m2.m[1][3]) + (m1.m[1][2] * m2.m[2][3]) + (m1.m[1][3] * m2.m[3][3]);

    result.m[0][2] = (m1.m[2][0] * m2.m[2][0]) + (m1.m[2][1] * m2.m[1][0]) + (m1.m[2][2] * m2.m[2][0]) + (m1.m[2][3] * m2.m[3][0]);
    result.m[1][2] = (m1.m[2][0] * m2.m[0][1]) + (m1.m[2][1] * m2.m[1][1]) + (m1.m[2][2] * m2.m[2][1]) + (m1.m[2][3] * m2.m[3][1]);
    result.m[2][2] = (m1.m[2][0] * m2.m[0][2]) + (m1.m[2][1] * m2.m[1][2]) + (m1.m[2][2] * m2.m[2][2]) + (m1.m[2][3] * m2.m[3][2]);
    result.m[3][2] = (m1.m[2][0] * m2.m[0][3]) + (m1.m[2][1] * m2.m[1][3]) + (m1.m[2][2] * m2.m[2][3]) + (m1.m[2][3] * m2.m[3][3]);

    result.m[0][3] = (m1.m[3][0] * m2.m[0][0]) + (m1.m[3][1] * m2.m[1][0]) + (m1.m[3][2] * m2.m[2][0]) + (m1.m[3][3] * m2.m[3][0]);
    result.m[1][3] = (m1.m[3][0] * m2.m[0][1]) + (m1.m[3][1] * m2.m[1][1]) + (m1.m[3][2] * m2.m[2][1]) + (m1.m[3][3] * m2.m[3][1]);
    result.m[2][3] = (m1.m[3][0] * m2.m[0][2]) + (m1.m[3][1] * m2.m[1][2]) + (m1.m[3][2] * m2.m[2][2]) + (m1.m[3][3] * m2.m[3][2]);
    result.m[3][3] = (m1.m[3][0] * m2.m[0][3]) + (m1.m[3][1] * m2.m[1][3]) + (m1.m[3][2] * m2.m[2][3]) + (m1.m[3][3] * m2.m[3][3]);

    return result;
}

mat4 m4_scale_mat(vec3 scale){

    mat4 scale_mat;

    scale_mat = edenMat;

    scale_mat.m[0][0] = scale.x;
    scale_mat.m[1][1] = scale.y;
    scale_mat.m[2][2] = scale.z;

    return scale_mat;
}

mat4 m4_scale(mat4 mat, vec3 scale){
    mat4 scale_mat;

    scale_mat = edenMat;

    scale_mat.m[0][0] = scale.x;
    scale_mat.m[1][1] = scale.y;
    scale_mat.m[2][2] = scale.z;

    mat4 result = m4_mult(mat, scale_mat);

    return result;
}


vec3 v3_norm(vec3 v)
{
   float len = v3_length(v);
   if (len > 0)
       return (vec3){ v.x / len, v.y / len, v.z / len };
   else
       return (vec3){ 0, 0, 0};
}

vec3 v3_proj(vec3 v, vec3 onto)
{
   return v3_muls(onto, v3_dot(v, onto) / v3_dot(onto, onto));
}

vec3 v3_cross(vec3 a, vec3 b)
{
   return (vec3){
       a.y * b.z - a.z * b.y,
       a.z * b.x - a.x * b.z,
       a.x * b.y - a.y * b.x
   };

}

mat4 m4_perspective(float fov_degrees, float near_plane, float far_plane)

{

  float aspect_ratio = ((float)swapChainExtent.width) / ((float) swapChainExtent.height);

  float range = far_plane  - near_plane;
  float tan_half_fov = tanf((fov_degrees * (M_PI / 180)) / 2.0);

  mat4 matrix;

  matrix.m[0][0] = 1.0f / (tan_half_fov * aspect_ratio);
  matrix.m[0][1] = 0.0f;
  matrix.m[0][2] = 0.0f;
  matrix.m[0][3] = 0.0f;

  matrix.m[1][0] = 0.0f;
  matrix.m[1][1] = 1.0f / tan_half_fov;
  matrix.m[1][2] = 0.0f;
  matrix.m[1][3] = 0.0f;

  matrix.m[2][0] = 0.0f;
  matrix.m[2][1] = 0.0f;
  matrix.m[2][2] = far_plane / (float) range;
  matrix.m[2][3] = 1.0f;

  matrix.m[3][0] = 0.0f;
  matrix.m[3][1] = 0.0f;
  matrix.m[3][2] = -(far_plane * near_plane) / (float) range;
  matrix.m[3][3] = 0.0f;

  return matrix;
}

vec3 v3_add(vec3 a, vec3 b) { return (vec3){ a.x + b.x, a.y + b.y, a.z + b.z }; }
vec3 v3_adds  (vec3 a, float s) { return (vec3){ a.x + s,   a.y + s,   a.z + s   }; }
vec3 v3_sub   (vec3 a, vec3 b) { return (vec3){ a.x - b.x, a.y - b.y, a.z - b.z }; }
vec3 v3_subs  (vec3 a, float s) { return (vec3){ a.x - s,   a.y - s,   a.z - s   }; }
vec3 v3_mul   (vec3 a, vec3 b) { return (vec3){ a.x * b.x, a.y * b.y, a.z * b.z }; }
vec3 v3_muls  (vec3 a, float s) { return (vec3){ a.x * s,   a.y * s,   a.z * s   }; }
vec3 v3_div   (vec3 a, vec3 b) { return (vec3){ a.x / b.x, a.y / b.y, a.z / b.z }; }
vec3 v3_divs  (vec3 a, float s) { return (vec3){ a.x / s,   a.y / s,   a.z / s   }; }
float  v3_length(vec3 v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
float  v3_dot   (vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

mat4 mat4_f(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33)
{
    return (mat4){
        .m[0][0] = m00, .m[1][0] = m10, .m[2][0] = m20, .m[3][0] = m30,
        .m[0][1] = m01, .m[1][1] = m11, .m[2][1] = m21, .m[3][1] = m31,
        .m[0][2] = m02, .m[1][2] = m12, .m[2][2] = m22, .m[3][2] = m32,
        .m[0][3] = m03, .m[1][3] = m13, .m[2][3] = m23, .m[3][3] = m33
    };
}

char* add_string(char *s1, char *s2){
    int len = strlen(s1);
    int len2 = strlen(s2);

    char* text = (char *)calloc(len + len2, sizeof(char));

    memcpy(text, s1, len);
    memcpy(text + len, s2, len2);

    memset(text + len + len2, 0, 1);

    return text;
}
