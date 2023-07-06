#include "shader_builder.h"

#include "spirv.h"

#include <stdio.h>
#include <stdlib.h>

uint32_t ShaderBuilderCheckVariable(ShaderBuilder *builder, ShaderVariableType variable_type, uint32_t size){

    for(int i=0;i < builder->num_variables;i++)
    {
        if(builder->variables[i].type == variable_type && builder->variables[i].values[0] == size)
            return builder->variables[i].indx;
    }

    return 0;
}

ShaderVariable *ShaderBuilderFindVar(ShaderBuilder *builder, uint32_t indx){

    ShaderVariable *res = NULL;

    for(int i= 0;i < builder->num_variables;i++){
        if(builder->variables[i].indx == indx){
            res = &builder->variables[i];
            break;
        }
    }

    return res;
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


    while(len % sizeof(uint32_t)){
        builder->infos[builder->num_debug_infos].name[len] = 0;
        len ++;
    }

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

uint32_t ShaderBuilderAddFloat(ShaderBuilder *builder){

    uint32_t res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_FLOAT, 32);

    if(!res){
        uint32_t arr[] = { 32 };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_FLOAT, 0, NULL, 0, arr, 1);
    }

    return res;
}

uint32_t ShaderBuilderAddInt(ShaderBuilder *builder){

    uint32_t res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_INT, 32);

    if(!res){
        uint32_t arr[] = { 32 };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_INT, 0, NULL, 0, arr, 1);
    }

    return res;
}

int ShaderBuilderAddConstant(ShaderBuilder *builder, ShaderVariableType var_type, ShaderDataFlags flags,  uint32_t valu){

    uint32_t res = ShaderBuilderCheckVariable(builder, var_type, 32);

    if(!res){
        switch (var_type) {
            case SHADER_VARIABLE_TYPE_INT:
                res = ShaderBuilderAddInt(builder);
                break;
            case SHADER_VARIABLE_TYPE_FLOAT:
                res = ShaderBuilderAddFloat(builder);
                break;
        }
    }

    ShaderVariable *variable = &builder->variables[builder->num_variables];

    variable->type = SHADER_VARIABLE_TYPE_CONSTANT;
    variable->indx = builder->current_index + 1;
    variable->args[0] = res;
    variable->values[0] = valu;

    variable->num_args = 1;
    variable->num_values = 1;
    variable->flags = flags;

    builder->num_variables++;
    builder->current_index++;

    return builder->current_index;
}

uint32_t ShaderBuilderAddArray(ShaderBuilder *builder, ShaderVariableType var_type, uint32_t count, char *name){

    uint32_t res = 0;

    switch(var_type){
        case SHADER_VARIABLE_TYPE_FLOAT:
            res = ShaderBuilderAddFloat(builder);
            break;
        case SHADER_VARIABLE_TYPE_INT:
             res = ShaderBuilderAddInt(builder);
            break;
    }

    uint32_t cnst = ShaderBuilderAddConstant(builder, SHADER_VARIABLE_TYPE_INT, 0, count);

    uint32_t arr[] = { res, cnst };
    res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_ARRAY, 0, arr, 2, NULL, 0 );


    if(name != NULL){
        uint32_t len = strlen(name);

        memcpy(builder->infos[builder->num_debug_infos].name, name, len);
        builder->infos[builder->num_debug_infos].indx = res;

        builder->num_debug_infos++;
    }

    return res;

}

int ShaderBuilderAddVector(ShaderBuilder *builder, uint32_t size, char *name){
    int res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_VECTOR, size);

    if(!res){

        res = ShaderBuilderAddFloat(builder);

        uint32_t arr[] = { res };
        uint32_t arr2[] = { size };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, arr, 1, arr2, 1);
    }

    if(name != NULL){
        uint32_t len = strlen(name);

        memcpy(builder->infos[builder->num_debug_infos].name, name, len);
        builder->infos[builder->num_debug_infos].indx = res;

        builder->num_debug_infos++;
    }

    return res;
}

uint32_t ShaderBuilderAddStruct(ShaderBuilder *builder, ShaderStructConstr *struct_arr, uint32_t count, char *name ){

    uint32_t elem_indx[count];

    for(int i = 0;i < count;i++){

         switch(struct_arr[i].var_type){
             case SHADER_VARIABLE_TYPE_INT:
                 elem_indx[i] = ShaderBuilderAddInt(builder);
                 break;
             case SHADER_VARIABLE_TYPE_FLOAT:
                 elem_indx[i] = ShaderBuilderAddFloat(builder);
                 break;
             case SHADER_VARIABLE_TYPE_VECTOR:
                 elem_indx[i] = ShaderBuilderAddVector(builder, struct_arr[i].size, NULL);
                 break;
             case SHADER_VARIABLE_TYPE_ARRAY:
                 elem_indx[i] = ShaderBuilderAddArray(builder, struct_arr[i].type, struct_arr[i].size, NULL);
                 break;
         }
    }

    ShaderVariable *variable = &builder->variables[builder->num_variables];

    memcpy(variable->args, elem_indx, sizeof(uint32_t) * count);

    variable->type = SHADER_VARIABLE_TYPE_STRUCT;
    variable->indx = builder->current_index + 1;
    variable->num_args = count;
    variable->num_values = 0;
    variable->flags = 0;

    uint32_t len = strlen(name);

    memcpy(builder->infos[builder->num_debug_infos].name, name, len);
    builder->infos[builder->num_debug_infos].indx = builder->current_index + 1;

    for(int i=0;i < count;i++){
        len = strlen(struct_arr[i].name);
        memcpy(builder->infos[builder->num_debug_infos].child_name[i], struct_arr[i].name, len);
    }

    builder->infos[builder->num_debug_infos].num_childs = count;

    builder->num_debug_infos++;
    builder->num_variables++;
    builder->current_index++;

    return builder->current_index;
}

uint32_t ShaderBuilderAddUniform(ShaderBuilder *builder, ShaderStructConstr *struct_arr, uint32_t count, char *name){

    uint32_t res = ShaderBuilderAddStruct(builder, struct_arr, count, name);

    {
        uint32_t arr[] = { res };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_POINTER, SHADER_DATA_FLAG_UNIFORM, arr, 1, NULL, 0);
    }

    {
        uint32_t arr[] = { res };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_VARIABLE, SHADER_DATA_FLAG_UNIFORM, arr, 1, NULL, 0);
    }

    return res;
}

int ShaderBuilderAddIOData(ShaderBuilder *builder, ShaderVariableType type, ShaderDataFlags flags, ShaderStructConstr *struct_arr, uint32_t size, char *name){

    int res = 0;

    switch(type){
        case SHADER_VARIABLE_TYPE_VECTOR:
            res = ShaderBuilderAddVector(builder, size, name );
            break;
        case SHADER_VARIABLE_TYPE_STRUCT:
            res = ShaderBuilderAddStruct(builder, struct_arr, size, name);
            break;
    }

    uint32_t orig_indx = res;

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
    builder->ioData[builder->num_io_data].orig_indx = orig_indx;

    builder->num_io_data ++;

    return res;

}

int ShaderBuilderAddOperand(ShaderBuilder *builder, ShaderLabel *label, uint32_t *indexes, uint32_t count, ShaderOperandType operand){

    ShaderOperand *oper = &label->operands[label->num_operands];

    memcpy(oper->var_indx, indexes, sizeof(uint32_t) * count);

    oper->num_vars = count;
    oper->op_type = operand;
    oper->indx = builder->current_index + 1;

    return builder->current_index;
}

int ShaderBuilderFindOperand( ShaderLabel *label, ShaderOperandType op_type){

    for(int i=0;i < label->num_operands;i++){
        if(label->operands[i].op_type == op_type){
            return i;
        }
    }

    return -1;
}

uint32_t ShaderBuilderAcceptAccess(ShaderBuilder *builder, ShaderLabel *label, uint32_t val_indx){

    uint32_t res = 0;

    for(int i=0;i < label->num_operands;i++){
        if(label->operands[i].op_type == SHADER_OPERAND_TYPE_ACCESS && label->operands[i].var_indx[0] == val_indx)
            res =label->operands[i].indx ;
            break;
    }

    if(!res)
        ShaderBuilderAddOperand(builder, label, &val_indx, 1,  SHADER_OPERAND_TYPE_ACCESS);

    return res;
}

uint32_t ShaderBuilderAcceptLoad(ShaderBuilder *builder, ShaderLabel *label, uint32_t val_indx){

    ShaderVariable * varisable = ShaderBuilderFindVar(builder, val_indx);

    if(varisable == NULL)
        return 0;

    int res = 1;

    if(varisable->flags == SHADER_DATA_FLAG_UNIFORM)
        res = ShaderBuilderAcceptAccess(builder, label, val_indx);

    if(res < 0){
        uint32_t arr = {val_indx};
        ShaderBuilderAddOperand(builder, label, arr, 1,  SHADER_OPERAND_TYPE_ACCESS);
    }

    res = 0;
    for(int i=0;i < label->num_operands;i++){
        if(label->operands[i].op_type == SHADER_OPERAND_TYPE_LOAD && label->operands[i].var_indx[0] == val_indx){
            res =label->operands[i].indx;
            break;
        }
    }

    if(!res)
        res = ShaderBuilderAddOperand(builder, label, &val_indx, 1, SHADER_OPERAND_TYPE_LOAD);

    return res;
}

int ShaderBuilderAddFuncAdd(ShaderBuilder *builder, ShaderLabel *label, uint32_t val_1, uint32_t val_2, uint32_t res_store, ShaderOperandType op_type){

    uint32_t val_indx_1 = ShaderBuilderAcceptAccess(builder, label, val_1);

    uint32_t val_indx_2 = ShaderBuilderAcceptAccess(builder, label, val_2);

    uint32_t arr[] = {val_indx_1, val_indx_2};
    uint32_t res = ShaderBuilderAddOperand(builder, label, arr, 2, op_type);

    arr[0] = res;
    arr[1] = res_store;
    res = ShaderBuilderAddOperand(builder, label, arr, 2, SHADER_OPERAND_TYPE_STORE);

    return res;
}

ShaderLabel *ShaderBuilderAddLabel(ShaderBuilder *builder, uint32_t func_indx){

    uint32_t num_labels = builder->functions[func_indx].num_labels;

    builder->functions[func_indx].labels[num_labels].index =  builder->current_index + 1;

    builder->functions[func_indx].num_labels++;
    builder->current_index++;

    return &builder->functions[func_indx].labels[num_labels - 1];
}

ShaderFunc *ShaderBuilderAddFunction(ShaderBuilder *builder, ShaderVariableType output, char *name){

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

    uint32_t len = strlen(name);

    memcpy(builder->infos[builder->num_debug_infos].name, name, len);
    builder->infos[builder->num_debug_infos].indx = builder->current_index + 1;

    builder->num_debug_infos++;
    builder->num_functions++;
    builder->current_index++;

    ShaderBuilderAddLabel(builder, builder->num_functions - 1 );

    return &builder->functions[builder->num_functions - 1];

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

    builder->main_point_index = ShaderBuilderAddFunction(builder, SHADER_VARIABLE_TYPE_VOID, "main");

    /*ShaderStructConstr struct_arr[] = {
        {SHADER_VARIABLE_TYPE_VECTOR, 2, 0, "position"},
        {SHADER_VARIABLE_TYPE_VECTOR, 2, 0, "rotation"},
        {SHADER_VARIABLE_TYPE_VECTOR, 2, 0, "scale"},
        {SHADER_VARIABLE_TYPE_ARRAY, 1,  SHADER_VARIABLE_TYPE_FLOAT, "array"}
    };

    ShaderBuilderAddUniform(builder, struct_arr, 4, "some_uniform");*/


    ShaderStructConstr struct_arr[] = {
        {SHADER_VARIABLE_TYPE_VECTOR, 4, 0, "gl_Position"},
        {SHADER_VARIABLE_TYPE_FLOAT, 32, 0, "gl_PointSize"},
        {SHADER_VARIABLE_TYPE_ARRAY, 1,  SHADER_VARIABLE_TYPE_FLOAT, "gl_ClipDistance"}
    };

    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_STRUCT, SHADER_DATA_FLAG_OUTPUT, struct_arr, 3, "gl_PerVertex");

    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, NULL, 2, "position");
    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, NULL, 3, "color");
    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, 0, NULL, 2, "intTexCoord");

    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, SHADER_DATA_FLAG_OUTPUT, NULL, 3, "fragColor");
    ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_VECTOR, SHADER_DATA_FLAG_OUTPUT, NULL, 2, "fragTexCoord");

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
    ShaderBuilderAddValue(builder, 0x0);

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

        ShaderBuilderAddValue(builder, builder->main_point_index->indx);

        ShaderBuilderAddString(builder, builder->infos[0].name);
        ShaderBuilderAddValue(builder, 0x0);

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

        uint32_t len =  0, temp = 0;
        for(int i=0;i < builder->num_debug_infos;i++){
            temp = strlen(builder->infos[i].name) + sizeof(uint32_t);

            len = (uint32_t)floor(temp / (float)sizeof(uint32_t));

            ShaderBuilderAddOp(builder, SpvOpName, 3 + len - 1);
            ShaderBuilderAddValue(builder, builder->infos[i].indx );
            ShaderBuilderAddString(builder, builder->infos[i].name);
            if(!(temp % sizeof(uint32_t)))
                ShaderBuilderAddValue(builder, 0x0);

            for(int j=0;j < builder->infos[i].num_childs;j++){

                temp = strlen(builder->infos[i].child_name[j]) + sizeof(uint32_t);

                len = (uint32_t)floor(temp / (float)sizeof(uint32_t));

                ShaderBuilderAddOp(builder, SpvOpMemberName, 4 + len - 1);
                ShaderBuilderAddValue(builder, builder->infos[i].indx );
                ShaderBuilderAddValue(builder, j);
                ShaderBuilderAddString(builder, builder->infos[i].child_name[j]);

                if(!(temp % sizeof(uint32_t)))
                    ShaderBuilderAddValue(builder, 0x0);
            }
        }
    }

    {

        ShaderVariable *variable;
        int j=0;
        for(int i=1;i < builder->num_variables;i++)
        {
            variable = &builder->variables[i];
            j = 0;

            if(variable->indx == 0)
                continue;

            switch(variable->type){
                case SHADER_VARIABLE_TYPE_VOID:
                    ShaderBuilderAddOp(builder, SpvOpTypeVoid, 2);
                    ShaderBuilderAddValue(builder, variable->indx);
                    break;
                case SHADER_VARIABLE_TYPE_FUNCTION:
                    ShaderBuilderWriteFuncType(builder, i);
                    break;
                case SHADER_VARIABLE_TYPE_INT:
                    ShaderBuilderAddOp(builder, SpvOpTypeInt, 4);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->values[0]);
                    ShaderBuilderAddValue(builder, variable->flags & SHADER_DATA_FLAG_UNSIGNED ? 0 : 1);
                    break;
                case SHADER_VARIABLE_TYPE_FLOAT:
                    ShaderBuilderAddOp(builder, SpvOpTypeFloat, 3);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->values[0]);
                    break;
                case SHADER_VARIABLE_TYPE_ARRAY:
                    ShaderBuilderAddOp(builder, SpvOpTypeArray, 4);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    ShaderBuilderAddValue(builder, variable->args[1]);
                    break;
                case SHADER_VARIABLE_TYPE_CONSTANT:
                    ShaderBuilderAddOp(builder, SpvOpConstant, 4);
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->values[0]);
                    break;
                case SHADER_VARIABLE_TYPE_VECTOR:
                    ShaderBuilderAddOp(builder, SpvOpTypeVector, 4);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    ShaderBuilderAddValue(builder, variable->values[0]);
                    break;
                case SHADER_VARIABLE_TYPE_STRUCT:
                    ShaderBuilderAddOp(builder, SpvOpTypeStruct, 2 + variable->num_args);
                    ShaderBuilderAddValue(builder, variable->indx);
                    for(j=0;j < variable->num_args;j++)
                        ShaderBuilderAddValue(builder, variable->args[j]);
                    break;
                case SHADER_VARIABLE_TYPE_POINTER:
                    ShaderBuilderAddOp(builder, SpvOpTypePointer, 4);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->flags & SHADER_DATA_FLAG_UNIFORM ? SpvStorageClassUniform : (variable->flags & SHADER_DATA_FLAG_OUTPUT ? SpvStorageClassOutput : SpvStorageClassInput));
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    break;
                case SHADER_VARIABLE_TYPE_VARIABLE:
                    ShaderBuilderAddOp(builder, SpvOpVariable, 4);
                    ShaderBuilderAddValue(builder, variable->args[0]);
                    ShaderBuilderAddValue(builder, variable->indx);
                    ShaderBuilderAddValue(builder, variable->flags & SHADER_DATA_FLAG_UNIFORM ? SpvStorageClassUniform : (variable->flags & SHADER_DATA_FLAG_OUTPUT ? SpvStorageClassOutput : SpvStorageClassInput));
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

            for(int j=0;j < builder->functions[i].num_labels;j++)
             {
                ShaderBuilderAddOp(builder, SpvOpLabel, 2);
                ShaderBuilderAddValue(builder, builder->functions[i].labels[j].index);

                ShaderBuilderAddOp(builder, SpvOpReturn, 1);
            }

            ShaderBuilderAddOp(builder, SpvOpFunctionEnd, 1);
        }
    }
}

void ShaderBuilderWriteToFile(ShaderBuilder *builder){

    FILE *somefile = fopen("/home/ilia/temp_shader.spv", "w");

    fwrite(builder->code, sizeof(uint32_t), builder->size, somefile);

    fclose(somefile);

}
