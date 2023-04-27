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

extern void *dir_shadow_array;
extern uint32_t num_dir_shadows;

extern void *point_shadow_array;
extern uint32_t num_point_shadows;

extern void *spot_shadow_array;
extern uint32_t num_spot_shadows;

extern void *geom_texture;
extern void *geom_uniform;
extern void *geom_geometry;

extern GeometrySBO geometrySBO;

extern char *rootDirPath;

extern void *e_var_images;
extern int e_var_num_images;

extern void **e_var_lights;
extern int e_var_num_lights;

extern FontCache *e_var_fonts;
extern uint32_t e_var_num_fonts;

extern int define_font_loaded;

extern const char* validationLayers[];
extern uint32_t num_valid_layers;

extern const char* deviceExtensions[];
extern uint32_t num_dev_extensions;

extern const FontIndexes fontIndexes[];

#endif // E_RESOURCE_DATA_H
