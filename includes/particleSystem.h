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
    float life;
    float speed;
} Particle;

typedef struct{
    GameObject3D* go;
    Particle* particles;
} ParticleObject3D;

void InitParticle3D(GameObject3D* particle, vec3 position, const char* texturePath, const char* vertShader, const char* fragShader);
void AddParticle3D(GameObject3D* particle, vec3 position, vec3 direction,  float life, float speed);

#endif // PARTICLESYSTEM_H
