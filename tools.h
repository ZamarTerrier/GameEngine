#ifndef TOOLS_H
#define TOOLS_H

#include "stdinclude.h"
#include "graphicsItems.h"

#include "e_resource.h"

VkCommandBuffer beginSingleTimeCommands();

void endSingleTimeCommands(VkCommandBuffer commandBuffer);

bool isComplete(QueueFamilyIndices self);

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

shader readFile(const char* filename);

VkShaderModule createShaderModule(shader shdr);

void InitPlane3D(vertexParam *vParam, indexParam *iParam,int rows, int colmns);
int SphereGenerator3D(vertexParam *vParam, indexParam *iParam,float radius, int stackCount, int sectorCount);
void ConeGenerator(vertexParam *vParam, indexParam *iParam, const float height, int stackCount, int sectorCount);
int IcoSphereGenerator(vertexParam *vParam, indexParam *iParam,float radius);
int Cubesphere(vertexParam *vParam, indexParam *iParam, float radius,int vertexCountPerRow);

float dot_grad(int hash, float xf);
float dot_grad2(int hash, float xf, float yf);

vec3 perp(const vec3 v);

float fade(float t);

float noise1D( float x);
float noise2D(float x, float y);

float sinWithRange(float value, float minV, float range);
float cosWithRange(float value, float minV, float range);

void ToolsAddStrings(char *out, int buff_size, char *s1, char *s2);
bool ToolsCmpStrings(char *in, char *s1);

#endif // TOOLS_H
