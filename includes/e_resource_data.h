#ifndef E_RESOURCE_DATA_H
#define E_RESOURCE_DATA_H

#include "engine_includes.h"

extern vec2 viewSize;
extern vec2 diffSize;

extern int WIDTH;
extern int HEIGHT;

extern char app_name[256];

extern void *e_var_current_entry;

extern void* cam2D;
extern void* cam3D;

extern char *rootDirPath;

extern void *e_var_images;
extern int e_var_num_images;

extern void **e_var_lights;
extern int e_var_num_lights;

extern PipelineCache *e_var_pipelines;
extern uint32_t e_var_num_pipelines;

extern FontCache *e_var_fonts;
extern uint32_t e_var_num_fonts;

extern int define_font_loaded;

extern const char* validationLayers[];
extern uint32_t num_valid_layers;

extern const char* deviceExtensions[];
extern uint32_t num_dev_extensions;

extern const FontIndexes fontIndexes[];

#endif // E_RESOURCE_DATA_H
