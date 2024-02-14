#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <stdint.h>

#define EXTEND_IMPORT_POINT_INDEX 0x1

#define SHADER_MAX_LENGTH 2048
#define SHADER_MAX_IODATA 32
#define SHADER_MAX_VARIABLES 256
#define SHADER_MAX_DEBUG_NAMES 256
#define SHADER_MAX_FUNCTIONS 32
#define SHADER_MAX_DECORATIONS 32

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

typedef struct{
    ShaderVariableType var_type;
    uint32_t size;
    uint32_t type;
    char name[32];
} ShaderStructConstr;

typedef enum{
    SHADER_OPERAND_TYPE_ACCESS,
    SHADER_OPERAND_TYPE_LOAD,
    SHADER_OPERAND_TYPE_STORE,
    SHADER_OPERAND_TYPE_COMPOSITE_EXTRACT,
    SHADER_OPERAND_TYPE_COMPOSITE_CONSTRUCT,
    SHADER_OPERAND_TYPE_ADD,
    SHADER_OPERAND_TYPE_SUB,
    SHADER_OPERAND_TYPE_MUL,
    SHADER_OPERAND_TYPE_DIV,
} ShaderOperandType;

typedef enum{
    SHADER_DATA_FLAG_UNSIGNED = 0x1,
    SHADER_DATA_FLAG_OUTPUT = 0x2,
    SHADER_DATA_FLAG_UNIFORM= 0x4,
} ShaderDataFlags;

typedef struct{
    ShaderVariableType type;
    uint32_t indx;
    uint32_t orig_indx;
    uint32_t location;
} InputOutputData;

typedef struct{
    uint32_t indx;
    uint32_t res_store_indx;
    ShaderOperandType op_type;
    uint32_t var_indx[8];
    uint32_t num_vars;
} ShaderOperand;

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
    uint32_t index;
    ShaderOperand operands[256];
    uint32_t num_operands;
} ShaderLabel;

typedef struct{
    uint32_t result_type_indx;
    uint32_t func_type_indx;
    uint32_t indx;
    uint32_t function_control;
    ShaderFuncParam params[6];
    uint32_t num_params;
    ShaderLabel labels[32];
    uint32_t num_labels;
} ShaderFunc;

typedef struct{
    uint32_t indx;
    char name[64];
    char child_name[6][64];
    uint32_t num_childs;
} ShaderDebugInfo;

typedef struct{
    uint32_t indx;
    uint32_t type;
    uint32_t val;
} ShaderDecoration;

typedef struct{
    uint32_t code[SHADER_MAX_LENGTH];
    uint32_t size;
    InputOutputData ioData[SHADER_MAX_IODATA];
    uint32_t num_io_data;
    ShaderVariable variables[SHADER_MAX_VARIABLES];
    uint32_t num_variables;
    ShaderFunc functions[SHADER_MAX_FUNCTIONS];
    uint32_t num_functions;
    ShaderDebugInfo infos[SHADER_MAX_DEBUG_NAMES];
    uint32_t num_debug_infos;
    ShaderDecoration decors[SHADER_MAX_DECORATIONS];
    uint32_t num_decorations;
    ShaderType type;
    uint32_t current_index;
    ShaderFunc *main_point_index;
    uint32_t gl_struct_indx;
} ShaderBuilder;

void ShaderBuilderInit(ShaderBuilder *builder, ShaderType type);
void ShaderBuilderMake(ShaderBuilder *builder);
void ShaderBuilderWriteToFile(ShaderBuilder *builder,  const char *path);

int ShaderBuilderAddFuncAdd(ShaderBuilder *builder, ShaderLabel *label, uint32_t val_1, uint32_t val_2, uint32_t size, uint32_t res_store);
uint32_t ShaderBuilderAddFuncMove(ShaderBuilder *builder, ShaderLabel *label, uint32_t src_indx, uint32_t src_size, uint32_t dest_indx);

uint32_t ShaderBuilderAddUniform(ShaderBuilder *builder, ShaderStructConstr *struct_arr, uint32_t count, char *name);
uint32_t ShaderBuilderAddIOData(ShaderBuilder *builder, ShaderVariableType type, ShaderDataFlags flags, ShaderStructConstr *struct_arr, uint32_t size, char *name, uint32_t location);

#endif // SHADER_BUILDER_H
