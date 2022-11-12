#ifndef RECOURCE_H
#define RECOURCE_H

#include "engine_includes.h"

extern uint32_t MAX_FRAMES_IN_FLIGHT;
extern int WIDTH;
extern int HEIGHT;
extern uint32_t VALIDSIZE;
extern uint32_t DEVEXTSIZE;

extern char app_name[256];

extern bool enableValidationLayers;

extern void* e_window;
extern void* instance;
extern void* debugMessenger;

extern void* physicalDevice;
extern void* device;

extern void* graphicsQueue;
extern void* presentQueue;

extern void* surface;

extern void* swapChain;
extern void** swapChainImages;
extern void** swapChainImageViews;

extern void* depthImage;
extern void* depthImageMemory;
extern void* depthImageView;

extern uint32_t swapChainImageFormat;
extern EIExtent2D swapChainExtent;

extern void* renderPass;

extern void** swapChainFramebuffers;

extern void* commandPool;
extern void** commandBuffers;

extern void** imageAvailableSemaphores;
extern void** renderFinishedSemaphores;

extern void** inFlightFences;
extern void** imagesInFlight;
extern size_t currentFrame;

extern bool framebufferResized;
extern bool framebufferwasResized;

extern const Vertex3D triVert[];
extern const Vertex3D quadVert[];
extern const Vertex2D planeVert[];
extern const Vertex2D projPlaneVert[];

extern const Vertex3D lineVert[];
extern const uint32_t triIndx[];
extern const uint32_t quadIndx[];
extern const uint32_t planeIndx[];
extern const uint32_t projPlaneIndx[];

extern const char* validationLayers[];
extern const char* deviceExtensions[];

extern EIVertexInputAttributeDescription planeAttributeDescription[];
extern EIVertexInputAttributeDescription cubeAttributeDescription[];
extern EIVertexInputAttributeDescription modelAttributeDescription[];
extern EIVertexInputAttributeDescription particleAttributeDescription[];

typedef struct{
    int FindLetter;
    int IndexLetter;
} FontIndexes;

extern const FontIndexes fontIndexes[];

extern uint32_t imageIndex;
extern uint32_t extensionCount;
extern uint32_t imagesCount;
extern uint32_t glfwExtensionCount;

extern void* cam2D;
extern void* cam3D;

extern void *e_var_current_entry;

extern vec2 viewSize;
extern vec2 diffSize;

extern char *rootDirPath;

extern void *e_var_images;
extern int e_var_num_images;

extern void **e_var_lights;
extern int e_var_num_lights;

extern char _binary_shaders_model_frag_spv_end;
extern int _binary_shaders_model_frag_spv_size;
extern char _binary_shaders_model_frag_spv_start;
extern char _binary_shaders_model_vert_spv_end;
extern int _binary_shaders_model_vert_spv_size;
extern char _binary_shaders_model_vert_spv_start;

extern char _binary_shaders_text_frag_spv_end;
extern int _binary_shaders_text_frag_spv_size;
extern char _binary_shaders_text_frag_spv_start;
extern char _binary_shaders_text_vert_spv_end;
extern int _binary_shaders_text_vert_spv_size;
extern char _binary_shaders_text_vert_spv_start;

extern char _binary_shaders_sprite_frag_spv_end;
extern int _binary_shaders_sprite_frag_spv_size;
extern char _binary_shaders_sprite_frag_spv_start;
extern char _binary_shaders_sprite_vert_spv_end;
extern int _binary_shaders_sprite_vert_spv_size;
extern char _binary_shaders_sprite_vert_spv_start;

extern char _binary_shaders_particle_frag_spv_end;
extern int _binary_shaders_particle_frag_spv_size;
extern char _binary_shaders_particle_frag_spv_start;
extern char _binary_shaders_particle_vert_spv_end;
extern int _binary_shaders_particle_vert_spv_size;
extern char _binary_shaders_particle_vert_spv_start;

extern char _binary_shaders_3d_object_frag_spv_end;
extern int _binary_shaders_3d_object_frag_spv_size;
extern char _binary_shaders_3d_object_frag_spv_start;
extern char _binary_shaders_3d_object_vert_spv_end;
extern int _binary_shaders_3d_object_vert_spv_size;
extern char _binary_shaders_3d_object_vert_spv_start;

extern char _binary_shaders_gui_widget_frag_spv_end;
extern int _binary_shaders_gui_widget_frag_spv_size;
extern char _binary_shaders_gui_widget_frag_spv_start;
extern char _binary_shaders_gui_widget_vert_spv_end;
extern int _binary_shaders_gui_widget_vert_spv_size;
extern char _binary_shaders_gui_widget_vert_spv_start;

extern char _binary_shaders_gui_widget_window_frag_spv_end;
extern int _binary_shaders_gui_widget_window_frag_spv_size;
extern char _binary_shaders_gui_widget_window_frag_spv_start;
extern char _binary_shaders_gui_widget_window_vert_spv_end;
extern int _binary_shaders_gui_widget_window_vert_spv_size;
extern char _binary_shaders_gui_widget_window_vert_spv_start;

extern char _binary_fonts_fantazer_normal_ttf_end;
extern int _binary_fonts_fantazer_normal_ttf_size;
extern char _binary_fonts_fantazer_normal_ttf_start;

extern char _binary_fonts_arial_ttf_end;
extern int _binary_fonts_arial_ttf_size;
extern char _binary_fonts_arial_ttf_start;

#endif
