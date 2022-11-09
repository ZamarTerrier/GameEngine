#include "e_direct.h"


char* e_GetCurrectFilePath(){
    char *pBuf = (char *) calloc(256, sizeof(char));
    size_t len = sizeof(char) * 256;
#ifdef _WIN_BUILD_
    int bytes = GetCurrentDirectoryA(len, pBuf);
    return pBuf;
#else
    char * temp = getcwd(pBuf, len);

    return pBuf;
#endif
}
