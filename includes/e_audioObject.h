#ifndef E_AUDIOOBJECT_H
#define E_AUDIOOBJECT_H

#include <engine_includes.h>

typedef uint64_t EAHandler;

typedef struct{
    void* object;
    void** sounds;
    uint32_t num_sounds;
}AudioObject;

void AudioObjectInit(AudioObject* ao);
void AudioObjectLoadFile(AudioObject* ao, const char* path);
void AudioObjectJustPlaySound(AudioObject* ao, const char* path);
void AudioObjectPlaySound(AudioObject* ao, uint32_t num);
void AudioObjectStopSound(AudioObject* ao, uint32_t num);
void AudioObjectDestroy(AudioObject* ao);

#endif // E_AUDIOOBJECT_H
