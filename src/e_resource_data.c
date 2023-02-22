#include "e_resource_data.h"

#include <vulkan/vulkan.h>

vec2 viewSize = { 0, 0 };
vec2 diffSize = { 1.0f, 1.0f };


int WIDTH = 1024;
int HEIGHT = 800;

char app_name[256];

void *e_var_current_entry = NULL;

void* cam2D;
void* cam3D;

char *rootDirPath;

void *e_var_images;
int e_var_num_images = 0;

void **e_var_lights = NULL;
int e_var_num_lights = 0;

FontCache *e_var_fonts = NULL;
uint32_t e_var_num_fonts = 0;

int define_font_loaded = 0;

const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
uint32_t num_valid_layers = 1;

const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
uint32_t num_dev_extensions = 1;

const FontIndexes fontIndexes[] ={
    { 1025, 0xD081 },//Ё
    { 1040, 0xD090 },//А
    { 1041, 0xD091 },//Б
    { 1042, 0xD092 },//В
    { 1043, 0xD093 },//Г
    { 1044, 0xD094 },//Д
    { 1045, 0xD095 },//Е
    { 1046, 0xD096 },//Ж
    { 1047, 0xD097 },//З
    { 1048, 0xD098 },//И
    { 1049, 0xD099 },//Й
    { 1050, 0xD09A },//К
    { 1051, 0xD09B },//Л
    { 1052, 0xD09C },//М
    { 1053, 0xD09D },//Н
    { 1054, 0xD09E },//О
    { 1055, 0xD09F },//П
    { 1056, 0xD0A0 },//Р
    { 1057, 0xD0A1 },//С
    { 1058, 0xD0A2 },//Т
    { 1059, 0xD0A3 },//У
    { 1060, 0xD0A4 },//Ф
    { 1061, 0xD0A5 },//Х
    { 1062, 0xD0A6 },//Ц
    { 1063, 0xD0A7 },//Ч
    { 1064, 0xD0A8 },//Ш
    { 1065, 0xD0A9 },//Щ
    { 1066, 0xD0AA },//Ъ
    { 1067, 0xD0AB },//Ы
    { 1068, 0xD0AC },//Ь
    { 1069, 0xD0AD },//Э
    { 1070, 0xD0AE },//Ю
    { 1071, 0xD0AF },//Я
    { 1072, 0xD0B0 },//а
    { 1073, 0xD0B1 },//б
    { 1074, 0xD0B2 },//в
    { 1075, 0xD0B3 },//г
    { 1076, 0xD0B4 },//д
    { 1077, 0xD0B5 },//е
    { 1078, 0xD0B6 },//ж
    { 1079, 0xD0B7 },//з
    { 1080, 0xD0B8 },//и
    { 1081, 0xD0B9 },//й
    { 1082, 0xD0BA },//к
    { 1083, 0xD0BB },//л
    { 1084, 0xD0BC },//м
    { 1085, 0xD0BD },//н
    { 1086, 0xD0BE },//о
    { 1087, 0xD0BF },//п
    { 1088, 0xD180 },//р
    { 1089, 0xD181 },//с
    { 1090, 0xD182 },//т
    { 1091, 0xD183 },//у
    { 1092, 0xD184 },//ф
    { 1093, 0xD185 },//х
    { 1094, 0xD186 },//ц
    { 1095, 0xD187 },//ч
    { 1096, 0xD188 },//ш
    { 1097, 0xD189 },//щ
    { 1098, 0xD18A },//ъ
    { 1099, 0xD18B },//ы
    { 1100, 0xD18C },//ь
    { 1101, 0xD18D },//э
    { 1102, 0xD18E },//ю
    { 1103, 0xD18F },//я
    { 1105, 0xD191 },//ё
};
