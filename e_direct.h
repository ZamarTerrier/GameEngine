#ifndef E_DIRECT_H
#define E_DIRECT_H

#include <stdio.h>
#ifdef _WIN_BUILD_
#include "windows.h"
#else
#include <unistd.h>
#endif

char* e_GetCurrectFilePath();

#endif // E_DIRECT_H
