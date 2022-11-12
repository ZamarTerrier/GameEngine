#include "e_audioObject.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

void AudioObjectInit(AudioObject* ao)
{
    ao->object = calloc(1, sizeof(ma_engine));
    ma_engine_init(NULL, ao->object);
}

void AudioObjectLoadFile(AudioObject* ao, const char* path)
{
    ma_sound sound;
    ma_result result = ma_sound_init_from_file(ao->object, path, 0, NULL, NULL, &sound);

    if(result == MA_SUCCESS)
    {
        ao->num_sounds ++;

        ao->sounds = realloc(ao->sounds, ao->num_sounds * sizeof(ma_sound *));
        ao->sounds[ao->num_sounds - 1] = calloc(1, sizeof(ma_sound));
        memcpy(ao->sounds[ao->num_sounds - 1], &sound, sizeof(ma_sound));
    }
}

void AudioObjectJustPlaySound(AudioObject* ao, const char* path)
{
    ma_engine_play_sound(ao->object, path, NULL);
}

void AudioObjectPlaySound(AudioObject* ao, uint32_t num)
{
    if(num < ao->num_sounds)
        ma_sound_start(ao->sounds[num]);
}

void AudioObjectStopSound(AudioObject* ao, uint32_t num)
{
    if(num < ao->num_sounds)
        ma_sound_stop(ao->sounds[num]);
}

void AudioObjectDestroy(AudioObject* ao)
{
    for(int i=0; i < ao->num_sounds;i++)
    {
        ma_sound_stop(ao->sounds[i]);
        ma_sound_uninit(ao->sounds[i]);
        free(ao->sounds[i]);
    }
    free(ao->sounds);

    ma_engine_uninit(ao->object);
    free(ao->object);
}
