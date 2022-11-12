#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "engine_includes.h"

#include "gameObject3D.h"

typedef struct {
    vec2 mouse;
    float time;
    vec3 camRot __attribute__ ((aligned (16)));
    vec3 camPos __attribute__ ((aligned (16)));
} DataBuffer;

typedef struct{
    vec3 position;
    vec3 direction;
    vec3 color;
    float scale;
    float life;
    float speed;
    float gravity;
} Particle;

typedef struct{
    GameObject3D go;
    Particle* particles;
    int num_parts;
} ParticleObject3D;

EIVertexInputBindingDescription Particle3DGetBindingDescription();

void Particle3DInit(ParticleObject3D* particle, DrawParam dParam);
void Particle3DAdd(ParticleObject3D* particle, vec3 position, vec3 direction, float speed, float gravity, float life);

#endif // PARTICLESYSTEM_H
