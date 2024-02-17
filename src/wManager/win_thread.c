
#ifdef _WIN_

#include "wManager/win_defines.h"

#include <assert.h>
#include <stdio.h>


//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int32_t _wManagerPlatformCreateTls(_wManagertls* tls)
{
    assert(tls->allocated == false);

    tls->index = TlsAlloc();
    if (tls->index == TLS_OUT_OF_INDEXES)
    {
        printf("Win32: Failed to allocate TLS index\n");
        return false;
    }

    tls->allocated = true;
    return true;
}

void _wManagerPlatformDestroyTls(_wManagertls* tls)
{
    if (tls->allocated)
        TlsFree(tls->index);
    memset(tls, 0, sizeof(_wManagertls));
}

void* _wManagerPlatformGetTls(_wManagertls* tls)
{
    assert(tls->allocated == true);
    return TlsGetValue(tls->index);
}

void _wManagerPlatformSetTls(_wManagertls* tls, void* value)
{
    assert(tls->allocated == true);
    TlsSetValue(tls->index, value);
}

int32_t _wManagerPlatformCreateMutex(_wManagermutex* mutex)
{
    assert(mutex->allocated == false);
    InitializeCriticalSection(&mutex->section);
    return mutex->allocated = true;
}

void _wManagerPlatformDestroyMutex(_wManagermutex* mutex)
{
    if (mutex->allocated)
        DeleteCriticalSection(&mutex->section);
    memset(mutex, 0, sizeof(_wManagermutex));
}

void _wManagerPlatformLockMutex(_wManagermutex* mutex)
{
    assert(mutex->allocated == true);
    EnterCriticalSection(&mutex->section);
}

void _wManagerPlatformUnlockMutex(_wManagermutex* mutex)
{
    assert(mutex->allocated == true);
    LeaveCriticalSection(&mutex->section);
}

#endif
