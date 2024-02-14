#ifndef E_AUDIOOBJECT_H
#define E_AUDIOOBJECT_H

#include "Variabels/engine_includes.h"

typedef uint64_t EAHandler;

typedef struct{
    void* object;
    void** sounds;
    void* g_delayNode;
    uint32_t num_sounds;
    float volume;
    float pitch;
    float pan;
}AudioObject;

void AudioObjectInit(AudioObject* ao);
void AudioObjectLoadFile(AudioObject* ao, const char* path);
void AudioObjectJustPlaySound(AudioObject* ao, const char* path);
void AudioObjectPlaySound(AudioObject* ao, uint32_t num);
void AudioObjectSetPan(AudioObject *ao, float pan);
void AudioObjectSetVolume(AudioObject *ao, float volume);
void AudioObjectSetPitch(AudioObject *ao, float pitch);
void AudioObjectSetSourcePosition3D(AudioObject *ao, vec3 position);
void AudioObjectStopSound(AudioObject* ao, uint32_t num);
void AudioObjectDestroy(AudioObject* ao);

#endif // E_AUDIOOBJECT_H
