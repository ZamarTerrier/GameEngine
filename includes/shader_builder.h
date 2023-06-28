#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <stdint.h>

#define EXTEND_IMPORT_POINT_INDEX 0x1

#define SHADER_MAX_LENGTH 2048
#define SHADER_MAX_IODATA 32
#define SHADER_MAX_VARIABLES 256
#define SHADER_MAX_FUNCTIONS 32

typedef enum{
    SHADER_TYPE_FRAGMENT,
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_COMPUTED,
    SHADER_TYPE_GEOMETRY,
    SHADER_TYPE_TESELLATION_CONTROL,
    SHADER_TYPE_TESELLATION_EVALUATION,
} ShaderType;

typedef enum{
    SHADER_VARIABLE_TYPE_VOID,
    SHADER_VARIABLE_TYPE_INT,
    SHADER_VARIABLE_TYPE_FLOAT,
    SHADER_VARIABLE_TYPE_VECTOR,
    SHADER_VARIABLE_TYPE_ARRAY,
    SHADER_VARIABLE_TYPE_STRUCT,
    SHADER_VARIABLE_TYPE_VARIABLE,
    SHADER_VARIABLE_TYPE_CONSTANT,
    SHADER_VARIABLE_TYPE_POINTER,
    SHADER_VARIABLE_TYPE_FUNCTION,
    SHADER_VARIABLE_TYPE_EXTENDED_IMPORT,
} ShaderVariableType;

typedef enum{
    SHADER_DATA_FLAG_UNSIGNED = 0x1,
    SHADER_DATA_FLAG_OUTPUT = 0x2,
} ShaderDataFlags;

typedef struct{
    ShaderVariableType type;
    uint32_t indx;
} InputOutputData;


typedef struct ShaderVariable{
    ShaderVariableType type;
    uint32_t indx;
    uint32_t args[6];
    uint32_t num_args;
    uint32_t values[6];
    uint32_t num_values;
    ShaderDataFlags flags;
    uint32_t result_type_indx;
} ShaderVariable;

typedef struct{
    ShaderVariableType type;
    uint32_t index;
} ShaderFuncParam;

typedef struct{
    uint32_t result_type_indx;
    uint32_t func_type_indx;
    uint32_t indx;
    uint32_t function_control;
    ShaderFuncParam params[6];
    uint32_t num_params;
} ShaderFunc;

typedef struct{
    uint32_t code[SHADER_MAX_LENGTH];
    uint32_t size;
    InputOutputData ioData[SHADER_MAX_IODATA];
    uint32_t num_io_data;
    ShaderVariable variables[SHADER_MAX_VARIABLES];
    uint32_t num_variables;
    ShaderFunc functions[SHADER_MAX_FUNCTIONS];
    uint32_t num_functions;
    ShaderType type;
    uint32_t current_index;
    uint32_t main_point_index;
} ShaderBuilder;

void ShaderBuilderInit(ShaderBuilder *builder, ShaderType type);
void ShaderBuilderMake(ShaderBuilder *builder);
void ShaderBuilderWriteToFile(ShaderBuilder *builder);

#endif // SHADER_BUILDER_H
