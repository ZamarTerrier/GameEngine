#ifndef E_MATH_VARIABLES_H
#define E_MATH_VARIABLES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    float x;
    float y;
} vec2;

typedef struct{
    float x;
    float y;
    float z;
} vec3;

typedef struct{
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct{
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t w;
} vec4_u8;

typedef struct{
    float m[2][2];
} mat2;

typedef struct{
    float m[3][3];
} mat3;

typedef struct{
    float m[4][4];
} mat4;

typedef struct{
    float m[16];
} mat4_p;

#ifdef __cplusplus
}
#endif

#endif // E_MATH_VARIABLES_H
