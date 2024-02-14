#pragma once

#include "Variabels/engine_includes.h"

#include "Objects/models.h"

#ifdef __cplusplus
extern "C"
{
#endif

void Load3DObjModel(ModelObject3D * mo, char *filepath, DrawParam *dParam);

#ifdef __cplusplus
}
#endif
