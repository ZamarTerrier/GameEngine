#include "shader_builder.h"

#include "spirv.h"

#include <stdio.h>
#include <stdlib.h>

int ShaderBuilderCheckVariable(ShaderBuilder *builder, ShaderVariableType variable_type, uint32_t size){

    for(int i=0;i < builder->num_variables;i++)
    {
        if(builder->variables[i].type == variable_type && builder->variables[i].values[0] == size)
            return builder->variables[i].indx;
    }

    return 0;
}

void ShaderBuilderAddOp(ShaderBuilder *builder, uint32_t code, uint32_t value){

    builder->code[builder->size] = (value << 16) | code;

    builder->size++;
}

void ShaderBuilderAddValue(ShaderBuilder *builder, uint32_t value){

    builder->code[builder->size] = value;

    builder->size++;
}

void ShaderBuilderAddString(ShaderBuilder *builder, char* string){
    uint32_t len = strlen(string);

    char *point = &builder->code[builder->size];

    memcpy(point, string, len);

    builder->size += len / sizeof(uint32_t);

    if(string[len + 1] != 0)
        ShaderBuilderAddValue(builder, 0x0 );
}

uint32_t ShaderBuilderAddVariable(ShaderBuilder *builder, ShaderVariableType type, uint32_t flags, uint32_t *args, uint32_t num_args, uint32_t *vals,  uint32_t num_vals){

    ShaderVariable *variable = &builder->variables[builder->num_variables];

    variable->type = type;
    variable->indx = builder->current_index + 1;

    if(num_args > 0)
    {
        if(args == NULL)
            return;

        for(int i=0; i  < num_args;i++)
            variable->args[i] = args[i];

    }

    variable->num_args = num_args;

    if(num_vals > 0)
    {
        if(vals == NULL)
            return;

        for(int i=0; i  < num_vals;i++)
            variable->values[i] = vals[i];

    }

    variable->num_values = num_vals;
    variable->flags = flags;

    builder->num_variables++;
    builder->current_index++;

    return builder->current_index;

}

int ShaderBuilderAddVector(ShaderBuilder *builder, uint32_t size){
    int res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_VECTOR, size);

    if(!res){

        res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_FLOAT, 32);

        if(!res){
            uint32_t arr[] = { 32 };
            res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_FLOAT, 0, NULL, 0, arr, 1);
        }


        uint32_t arr[] = { res };
        uint32_t arr2[] = { size };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, arr, 1, arr2, 1);
    }

    return res;
}

int ShaderBuilderAddIOData(ShaderBuilder *builder, ShaderVariableType type, ShaderDataFlags flags, uint32_t size){

    int res = 0;

    switch(type){
        case SHADER_VARIABLE_TYPE_VECTOR:
            res = ShaderBuilderAddVector(builder, size);
            break;
    }

    {
        uint32_t arr[] = { res };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_POINTER, flags, arr, 1, NULL, 0);
    }

    {
        uint32_t arr[] = { res };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_VARIABLE, flags, arr, 1, NULL, 0);
    }

    builder->ioData[builder->num_io_data].type = type;
    builder->ioData[builder->num_io_data].indx = res;

    builder->num_io_data ++;

    return res;

}

int ShaderBuilderAddFunction(ShaderBuilder *builder, ShaderVariableType output){

    uint32_t void_type, func_type;

    int res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_FUNCTION, 0);

    if(!res){

        res = ShaderBuilderCheckVariable(builder, output, 0);

        if(!res)
            res = ShaderBuilderAddVariable(builder, output, 0, NULL, 0, NULL, 0);

        void_type = res;

        uint32_t arr[] = {res};

        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_FUNCTION, 0, arr, 1, NULL, 0);
    }

    func_type = res;

    builder->functions[builder->num_functions].result_type_indx = void_type;
    builder->functions[builder->num_functions].func_type_indx = func_type;
    builder->functions[builder->num_functions].indx = builder->current_index + 1;

    builder->num_functions++;
    builder->current_index+=2;

    return builder->current_index - 1;

}

void ShaderBuilderInit(ShaderBuilder *builder, ShaderType type){

    memset(builder, 0, sizeof(ShaderBuilder));

    ShaderBuilderAddValue(builder, SpvMagicNumber);
    ShaderBuilderAddValue(builder, SpvVersion);
    ShaderBuilderAddValue(builder, 0x00080001);
    ShaderBuilderAddValue(builder, 48);
    ShaderBuilderAddValue(builder, 0);

    ShaderBuilderAddOp(builder, SpvOpCapability, 2);
    ShaderBuilderAddValue(builder, SpvCapabilityShader);

    builder->type = type;

    ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_EXTENDED_IMPORT, 0, NULL, 0, NULL, 0);

    builder->main_point_index = ShaderBuilderAddFunction(builder, SHADER_VARIABLE_TYPE_VOID);

    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, 2);
    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, 3);
    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, 2);

    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, SHADER_DATA_FLAG_OUTPUT, 3);
    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, SHADER_DATA_FLAG_OUTPUT, 3);


}


void ShaderBuilderWriteFuncType(ShaderBuilder *builder, uint32_t i){

    ShaderBuilderAddOp(builder, SpvOpTypeFunction, 3);

    ShaderVariable *variable = &builder->variables[i];

    ShaderBuilderAddValue(builder, variable->indx);

    for(int i=0;i < variable->num_args;i++)
        ShaderBuilderAddValue(builder, variable->args[i]);

}

void ShaderBuilderMake(ShaderBuilder *builder){

    ShaderBuilderAddOp(builder, SpvOpExtInstImport, 6);
    ShaderBuilderAddValue(builder, builder->variables[0].indx);
    ShaderBuilderAddString(builder, "GLSL.std.450");

    ShaderBuilderAddOp(builder, SpvOpMemoryModel, 3);
    ShaderBuilderAddValue(builder, SpvAddressingModelLogical);
    ShaderBuilderAddValue(builder, SpvMemoryModelGLSL450);

    {
        ShaderBuilderAddOp(builder, SpvOpEntryPoint, 4 + builder->num_io_data + 1);

        switch(builder->type)
        {
            case SHADER_TYPE_FRAGMENT:
                ShaderBuilderAddValue(builder, SpvExecutionModelFragment);
                break;
            case SHADER_TYPE_VERTEX:
                ShaderBuilderAddValue(builder, SpvExecutionModelVertex);
                break;
            case SHADER_TYPE_COMPUTED:
                ShaderBuilderAddValue(builder, SpvExecutionModelGLCompute);
                break;
            case SHADER_TYPE_GEOMETRY:
                ShaderBuilderAddValue(builder, SpvExecutionModelGeometry);
                break;
            case SHADER_TYPE_TESELLATION_CONTROL:
                ShaderBuilderAddValue(builder, SpvExecutionModelTessellationControl);
                break;
            case SHADER_TYPE_TESELLATION_EVALUATION:
                ShaderBuilderAddValue(builder, SpvExecutionModelTessellationEvaluation);
                break;
        }

        ShaderBuilderAddValue(builder, builder->main_point_index);

        ShaderBuilderAddString(builder, "main");

        for(int i=0;i < builder->num_io_data;i++)
            ShaderBuilderAddValue(builder, builder->ioData[i].indx);

    }

    if(builder->type == SHADER_TYPE_FRAGMENT){
        ShaderBuilderAddOp(builder, SpvOpExecutionMode, 3);
        ShaderBuilderAddValue(builder, builder->main_point_index );
        ShaderBuilderAddValue(builder, SpvExecutionModeOriginLowerLeft );
    }

    {
        ShaderBuilderAddOp(builder, SpvOpSource, 3);
        ShaderBuilderAddValue(builder, SpvSourceLanguageGLSL );
        ShaderBuilderAddValue(builder, 450 );

        ShaderBuilderAddOp(builder, SpvOpName, 4);
        ShaderBuilderAddValue(builder, builder->main_point_index );
        ShaderBuilderAddString(builder, "main");

        char temp_str[256];
        for(int i=0;i < builder->num_io_data;i++){
            memset(temp_str, 0, 256);

            sprintf(temp_str, "vector_%i", i);

            ShaderBuilderAddOp(builder, SpvOpName, 5);
            ShaderBuilderAddValue(builder, builder->ioData[i].indx);
            ShaderBuilderAddString(builder, temp_str);
            ShaderBuilderAddValue(builder, 0x0 );
        }
    }

    {

        ShaderVariable *variable;
        for(int i=1;i < builder->num_variables;i++)
        {
            variable = &builder->variables[i];

            switch(variable->type){
                case SHADER_VARIABLE_TYPE_VOID:
                    ShaderBuilderAddOp(builder, SpvOpTypeVoid, 2);
                    ShaderBuilderAddValue(builder, variable->indx);
                    break;
                case SHADER_VARIABLE_TYPE_FUNCTION:
                    ShaderBuilderWriteFuncType(builder, i);
                    break;
                case SHADER_VARIABLE_TYPE_FLOAT:
                    ShaderBuilderAddOp(builder, SpvOpTypeFloat, 3);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, 32);
                    break;
                case SHADER_VARIABLE_TYPE_VECTOR:
                    ShaderBuilderAddOp(builder, SpvOpTypeVector, 4);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    ShaderBuilderAddValue(builder, variable->values[0]);
                    break;
                case SHADER_VARIABLE_TYPE_POINTER:
                    ShaderBuilderAddOp(builder, SpvOpTypePointer, 4);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->flags & SHADER_DATA_FLAG_OUTPUT ? SpvStorageClassOutput : SpvStorageClassInput);
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    break;
                case SHADER_VARIABLE_TYPE_VARIABLE:
                    ShaderBuilderAddOp(builder, SpvOpVariable, 4);
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->flags & SHADER_DATA_FLAG_OUTPUT ? SpvStorageClassOutput : SpvStorageClassInput);
                    break;
            }
        }
    }

    {
        for(int i=0;i < builder->num_functions;i++)
        {
            ShaderBuilderAddOp(builder, SpvOpFunction, 5);
            ShaderBuilderAddValue(builder, builder->functions[i].result_type_indx);
            ShaderBuilderAddValue(builder, builder->functions[i].indx);
            ShaderBuilderAddValue(builder, builder->functions[i].function_control);
            ShaderBuilderAddValue(builder, builder->functions[i].func_type_indx);

            ShaderBuilderAddOp(builder, SpvOpLabel, 2);
            ShaderBuilderAddValue(builder, 0x5);

            ShaderBuilderAddOp(builder, SpvOpReturn, 1);

            ShaderBuilderAddOp(builder, SpvOpFunctionEnd, 1);
        }
    }
}

void ShaderBuilderWriteToFile(ShaderBuilder *builder){

    FILE *somefile = fopen("/home/ilia/temp_shader.spv", "w");

    fwrite(builder->code, sizeof(uint32_t), builder->size, somefile);

    fclose(somefile);

}
