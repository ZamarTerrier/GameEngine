#ifndef TOOLS_H
#define TOOLS_H

#include "engine_includes.h"
#include "graphicsItems.h"

#include "e_device_variables.h"

#include "e_resource.h"

void* beginSingleTimeCommands();

void endSingleTimeCommands(void* commandBuffer);

bool isComplete(QueueFamilyIndices self);

QueueFamilyIndices findQueueFamilies(void* device);

shader readFile(const char* filename);

void* createShaderModule(shader shdr);

void InitPlane3D(vertexParam *vParam, indexParam *iParam,int rows, int colmns);
void InitTerrain3D(vertexParam *vParam, indexParam *iParam, int rows, int colmns, int cell_step);
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
int ToolsStr32BitLength(uint32_t *text);

void* ToolsLoadImageFromFile(size_t* len, char *filepath);

#endif // TOOLS_H
