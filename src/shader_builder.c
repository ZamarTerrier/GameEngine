#include "shader_builder.h"

#include "spirv.h"

#include <stdio.h>
#include <stdlib.h>

uint32_t ShaderBuilderCheckVariable(ShaderBuilder *builder, ShaderVariableType variable_type, uint32_t *vals,  uint32_t size){

    uint32_t find = 0;
    for(int i=0;i < builder->num_variables;i++)
    {
        find = 1;

        if(builder->variables[i].type == variable_type){
            for(int j=0;j < size;j++){
                if(builder->variables[i].values[j] != vals[j]){
                    find = 0;
                    break;
                }
            }

            if(find)
                return builder->variables[i].indx;
        }
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

    /*if(string[len + 1] != 0)
        ShaderBuilderAddValue(builder, 0x0 );*/
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

    uint32_t arr[] = { 32, 1 };
    uint32_t res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_FLOAT, arr, 2);

    if(!res){
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_FLOAT, 0, NULL, 0, arr, 2);
    }

    return res;
}

uint32_t ShaderBuilderAddInt(ShaderBuilder *builder, uint32_t sign){

    uint32_t arr[] = { 32, sign };
    uint32_t res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_INT, arr, 2);

    if(!res){
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_INT, 0, NULL, 0, arr, 2);
    }

    return res;
}

uint32_t ShaderBuilderCheckConstans(ShaderBuilder *builder, uint32_t type_indx, uint32_t valu){

    for(int i=0;i < builder->num_variables;i++)
    {
        if(builder->variables[i].type == SHADER_VARIABLE_TYPE_CONSTANT && builder->variables[i].args[0] == type_indx && builder->variables[i].values[0] == valu)
            return builder->variables[i].indx;
    }

    return 0;
}

int ShaderBuilderAddConstant(ShaderBuilder *builder, ShaderVariableType var_type, ShaderDataFlags flags,  uint32_t valu, uint32_t sign){

    uint32_t arr[] = { 32, sign };
    uint32_t type_indx = ShaderBuilderCheckVariable(builder, var_type, arr, 2);

    if(!type_indx){
        switch (var_type) {
            case SHADER_VARIABLE_TYPE_INT:
                type_indx = ShaderBuilderAddInt(builder, sign);
                break;
            case SHADER_VARIABLE_TYPE_FLOAT:
                type_indx = ShaderBuilderAddFloat(builder);
                break;
        }
    }

    uint32_t res = ShaderBuilderCheckConstans(builder, type_indx, valu);

    if(!res){
        ShaderVariable *variable = &builder->variables[builder->num_variables];

        variable->type = SHADER_VARIABLE_TYPE_CONSTANT;
        variable->indx = builder->current_index + 1;
        variable->args[0] = type_indx;
        variable->values[0] = valu;

        variable->num_args = 1;
        variable->num_values = 1;
        variable->flags = flags;

        builder->num_variables++;
        builder->current_index++;

        return builder->current_index;
    }

    return res;
}

uint32_t ShaderBuilderCheckArray(ShaderBuilder *builder, uint32_t type_indx, uint32_t const_indx){


    for(int i=0;i < builder->num_variables;i++)
    {
        ShaderVariable *variable = &builder->variables[i];
        if(variable->type == SHADER_VARIABLE_TYPE_ARRAY)
            if(variable->args[0] == type_indx && variable->args[1] == const_indx)
                return builder->variables[i].indx;
    }

    return 0;
}

uint32_t ShaderBuilderAddArray(ShaderBuilder *builder, ShaderVariableType var_type, uint32_t count, char *name){

    uint32_t res = 0, type_indx = 0;

    switch(var_type){
        case SHADER_VARIABLE_TYPE_FLOAT:
            type_indx = ShaderBuilderAddFloat(builder);
            break;
        case SHADER_VARIABLE_TYPE_INT:
             type_indx = ShaderBuilderAddInt(builder, 0);
            break;
    }

    uint32_t cnst = ShaderBuilderAddConstant(builder, SHADER_VARIABLE_TYPE_INT, 0, count, 0);

    uint32_t check = ShaderBuilderCheckArray(builder, type_indx, cnst);

    if(!check){
        uint32_t arr[] = { type_indx, cnst };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_ARRAY, 0, arr, 2, NULL, 0 );
    }else
        res = check;

    if(name != NULL){
        uint32_t len = strlen(name);

        memcpy(builder->infos[builder->num_debug_infos].name, name, len);
        builder->infos[builder->num_debug_infos].indx = res;

        builder->num_debug_infos++;
    }

    return res;

}

uint32_t ShaderBuilderAddVector(ShaderBuilder *builder, uint32_t size, char *name){

    uint32_t arr[] = { size };
    uint32_t res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_VECTOR, arr, 1);

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
                 elem_indx[i] = ShaderBuilderAddInt(builder, 1);
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

    builder->decors[builder->num_decorations].indx = res;
    builder->num_decorations++;

    {
        uint32_t arr[] = { res };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_POINTER, SHADER_DATA_FLAG_UNIFORM, arr, 1, NULL, 0);
    }

    {
        uint32_t arr[] = { res };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_VARIABLE, SHADER_DATA_FLAG_UNIFORM, arr, 1, NULL, 0);
    }


    builder->decors[builder->num_decorations].indx = res;
    builder->num_decorations++;

    return res;
}

uint32_t ShaderBuilderCheckPointer(ShaderBuilder *builder, uint32_t type_indx, uint32_t flags){

    for(int i=0;i < builder->num_variables;i++)
    {
        if(builder->variables[i].type == SHADER_VARIABLE_TYPE_POINTER && builder->variables[i].args[0] == type_indx && (builder->variables[i].flags & flags))
            return builder->variables[i].indx;
    }

    return 0;
}

uint32_t ShaderBuilderAddPointer(ShaderBuilder *builder, ShaderVariableType point_type, uint32_t size, ShaderDataFlags flags){

    uint32_t arr[] = { size };
    uint32_t type_indx = ShaderBuilderCheckVariable(builder, point_type, arr, 1);

    switch(point_type){
        case SHADER_VARIABLE_TYPE_INT:
            type_indx = ShaderBuilderAddInt(builder, 1);
            break;
        case SHADER_VARIABLE_TYPE_FLOAT:
            type_indx = ShaderBuilderAddFloat(builder);
            break;
        case SHADER_VARIABLE_TYPE_VECTOR:
            type_indx = ShaderBuilderAddVector(builder, size, NULL);
            break;
    }

    uint32_t res = ShaderBuilderCheckPointer(builder, type_indx, flags);

    if(!res){
        uint32_t arr[] = { type_indx };
        res = ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_POINTER, flags, arr, 1, NULL, 0);
    }

    return res;
}

uint32_t ShaderBuilderAddIOData(ShaderBuilder *builder, ShaderVariableType type, ShaderDataFlags flags, ShaderStructConstr *struct_arr, uint32_t size, char *name, uint32_t location){

    uint32_t res = 0;

    switch(type){
        case SHADER_VARIABLE_TYPE_VECTOR:
            res = ShaderBuilderAddVector(builder, size, NULL );
            break;
        case SHADER_VARIABLE_TYPE_STRUCT:
            res = ShaderBuilderAddStruct(builder, struct_arr, size, name);

            builder->decors[builder->num_decorations].indx = res;
            builder->num_decorations++;
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


    if(type != SHADER_VARIABLE_TYPE_STRUCT){
        uint32_t len = strlen(name);

        memcpy(builder->infos[builder->num_debug_infos].name, name, len);
        builder->infos[builder->num_debug_infos].indx = res;

        builder->num_debug_infos++;
    }else{

        memset(builder->infos[builder->num_debug_infos].name, 0, 64);
        builder->infos[builder->num_debug_infos].indx = res;

        builder->num_debug_infos++;
    }

    builder->ioData[builder->num_io_data].type = type;
    builder->ioData[builder->num_io_data].indx = res;
    builder->ioData[builder->num_io_data].orig_indx = orig_indx;
    builder->ioData[builder->num_io_data].location = location;

    builder->num_io_data ++;

    return res;

}

InputOutputData *ShaderBuilderFindIOData(ShaderBuilder *builder, uint32_t indx){

    InputOutputData *res = NULL;

    for(int i=0;i < builder->num_io_data;i++){
        if(builder->ioData[i].indx == indx)
        {
            res = &builder->ioData[i];
            break;
        }
    }

    return res;

}

uint32_t ShaderBuilderAddOperand(ShaderBuilder *builder, ShaderLabel *label, uint32_t *indexes, uint32_t count, ShaderOperandType operand){

    ShaderOperand *oper = &label->operands[label->num_operands];

    memcpy(oper->var_indx, indexes, sizeof(uint32_t) * count);

    oper->num_vars = count;
    oper->op_type = operand;
    oper->indx = builder->current_index + 1;

    label->num_operands++;
    builder->current_index++;

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

uint32_t ShaderBuilderAcceptLoad(ShaderBuilder *builder, ShaderLabel *label, ShaderVariableType load_type, uint32_t val_indx, uint32_t size, uint32_t iter){

    uint32_t res_type= 0, res = 0;

    switch (load_type) {
        case SHADER_VARIABLE_TYPE_INT:
            res_type = ShaderBuilderAddInt(builder, 1);
            break;
        case SHADER_VARIABLE_TYPE_FLOAT:
            res_type = ShaderBuilderAddFloat(builder);
            break;
        case SHADER_VARIABLE_TYPE_VECTOR:
            res_type = ShaderBuilderAddVector(builder, size, NULL);
            break;
    }

    ShaderVariable * variable = ShaderBuilderFindVar(builder, val_indx);

    if(variable == NULL)
        return 0;

    if(!(variable->flags & SHADER_DATA_FLAG_UNIFORM)){
        InputOutputData *data = ShaderBuilderFindIOData(builder, val_indx);


        ShaderVariable *var_orig = ShaderBuilderFindVar(builder, data->orig_indx);

        if(var_orig->type == SHADER_VARIABLE_TYPE_STRUCT){

            uint32_t cnst = ShaderBuilderAddConstant(builder, SHADER_VARIABLE_TYPE_INT, 0, iter, 1);

            uint32_t arr[] = {res_type, val_indx, cnst};
            ShaderBuilderAddOperand(builder, label, arr, 3,  SHADER_OPERAND_TYPE_ACCESS);
        }
    }else{
        uint32_t cnst = ShaderBuilderAddConstant(builder, SHADER_VARIABLE_TYPE_INT, 0, iter, 1);
        uint32_t point = ShaderBuilderAddPointer(builder, SHADER_VARIABLE_TYPE_VECTOR, 2, SHADER_DATA_FLAG_UNIFORM);

        uint32_t arr[] = {point, val_indx, cnst};
        res = ShaderBuilderAddOperand(builder, label, arr, 3,  SHADER_OPERAND_TYPE_ACCESS);
    }


    uint32_t arr[] = {res_type, variable->flags & SHADER_DATA_FLAG_UNIFORM ? res : val_indx};
    res = ShaderBuilderAddOperand(builder, label, arr, 2, SHADER_OPERAND_TYPE_LOAD);

    return res;
}

uint32_t ShaderBuilderAddFuncMove(ShaderBuilder *builder, ShaderLabel *label, uint32_t src_indx, uint32_t src_size, uint32_t dest_indx){

    uint32_t res = ShaderBuilderAcceptLoad(builder, label, SHADER_VARIABLE_TYPE_VECTOR, src_indx, src_size, 0);

    uint32_t arr[] = {dest_indx, res};

    ShaderOperand *oper = &label->operands[label->num_operands];

    memcpy(oper->var_indx, arr, sizeof(uint32_t) * 2);

    oper->num_vars = 2;
    oper->op_type = SHADER_OPERAND_TYPE_STORE;

    label->num_operands++;

    return res;
}

int ShaderBuilderAddFuncAdd(ShaderBuilder *builder, ShaderLabel *label, uint32_t val_1, uint32_t val_2, uint32_t size, uint32_t res_store){

    uint32_t val_indx_1 = ShaderBuilderAcceptLoad(builder, label, SHADER_VARIABLE_TYPE_VECTOR, val_1, size, 0);
    uint32_t val_indx_2 = ShaderBuilderAcceptLoad(builder, label, SHADER_VARIABLE_TYPE_VECTOR, val_2, size, 0);
    uint32_t val_indx_3 = ShaderBuilderAcceptLoad(builder, label, SHADER_VARIABLE_TYPE_VECTOR, val_1, size, 2);

    uint32_t type = ShaderBuilderAddVector(builder, size, NULL);

    uint32_t res = 0;
    {
        uint32_t arr[] = {type, val_indx_2, val_indx_3};
        res = ShaderBuilderAddOperand(builder, label, arr, 3, SHADER_OPERAND_TYPE_MUL);
    }

    {
        uint32_t arr[] = {type, val_indx_1, res};
        res = ShaderBuilderAddOperand(builder, label, arr, 3, SHADER_OPERAND_TYPE_ADD);
    }

    uint32_t res_type = ShaderBuilderAddVector(builder, 4, NULL);

    uint32_t arr_extract[size];
    {
        uint32_t float_point = ShaderBuilderAddFloat(builder);

        for(int i=0;i < size;i++){
            uint32_t arr[] = {float_point, res,  i};
            arr_extract[i] = ShaderBuilderAddOperand(builder, label, arr, 3, SHADER_OPERAND_TYPE_COMPOSITE_EXTRACT);
        }
    }

    {
        uint32_t cnst = ShaderBuilderAddConstant(builder, SHADER_VARIABLE_TYPE_FLOAT, 0, 0, 1);
        float v_f = 1.0f;
        uint32_t v_u = 0;
        memcpy(&v_u, &v_f, sizeof(uint32_t));
        uint32_t cnst2 = ShaderBuilderAddConstant(builder, SHADER_VARIABLE_TYPE_FLOAT, 0, v_u, 1);
        uint32_t arr[] = {res_type, arr_extract[0], arr_extract[1], cnst, cnst2};
        res = ShaderBuilderAddOperand(builder, label, arr, 5, SHADER_OPERAND_TYPE_COMPOSITE_CONSTRUCT);
    }

    uint32_t n_res;
    {
        uint32_t cnst = ShaderBuilderAddConstant(builder, SHADER_VARIABLE_TYPE_INT, 0, 0, 1);
        uint32_t point = ShaderBuilderAddPointer(builder, SHADER_VARIABLE_TYPE_VECTOR, 4, SHADER_DATA_FLAG_OUTPUT);
        uint32_t arr[] = {point, res_store, cnst};
        n_res = ShaderBuilderAddOperand(builder, label, arr, 3, SHADER_OPERAND_TYPE_ACCESS);
    }

    uint32_t arr[] = {n_res, res};
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

    int res = ShaderBuilderCheckVariable(builder, SHADER_VARIABLE_TYPE_FUNCTION, NULL, 0);

    if(!res){

        res = ShaderBuilderCheckVariable(builder, output,  NULL, 0);

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

    ShaderBuilderAddValue(builder, SpvMagicNumber);// Magic number
    ShaderBuilderAddValue(builder, 0x00010000);//SpvVersion);
    ShaderBuilderAddValue(builder, 0x000D000B);//Generator version
    ShaderBuilderAddValue(builder, 0);//Lines code
    ShaderBuilderAddValue(builder, 0);

    ShaderBuilderAddOp(builder, SpvOpCapability, 2);
    ShaderBuilderAddValue(builder, SpvCapabilityShader);

    builder->type = type;

    ShaderBuilderAddVariable(builder, SHADER_VARIABLE_TYPE_EXTENDED_IMPORT, 0, NULL, 0, NULL, 0);

    builder->main_point_index = ShaderBuilderAddFunction(builder, SHADER_VARIABLE_TYPE_VOID, "main");

    if(type == SHADER_TYPE_VERTEX){
        ShaderStructConstr struct_arr[] = {
            {SHADER_VARIABLE_TYPE_VECTOR, 4, 0, "gl_Position"},
            {SHADER_VARIABLE_TYPE_FLOAT, 32, 0, "gl_PointSize"},
            {SHADER_VARIABLE_TYPE_ARRAY, 1,  SHADER_VARIABLE_TYPE_FLOAT, "gl_ClipDistance"},
            {SHADER_VARIABLE_TYPE_ARRAY, 1,  SHADER_VARIABLE_TYPE_FLOAT, "gl_CullDistance"}
        };

        builder->gl_struct_indx = ShaderBuilderAddIOData(builder, SHADER_VARIABLE_TYPE_STRUCT, SHADER_DATA_FLAG_OUTPUT, struct_arr, 4, "gl_PerVertex", 0);
    }
    /*char name[] = "tbo";

    memcpy(builder->infos[builder->num_debug_infos].name, name, strlen(name));
    builder->infos[builder->num_debug_infos].indx = uniform;
    builder->infos[builder->num_debug_infos].num_childs = 0;

    builder->num_debug_infos++;*/

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

    //Debug informations
   {
        ShaderBuilderAddOp(builder, SpvOpSource, 3);
        ShaderBuilderAddValue(builder, SpvSourceLanguageGLSL );
        ShaderBuilderAddValue(builder, 450 );

        ShaderBuilderAddOp(builder, SpvOpSourceExtension, 10);
        ShaderBuilderAddString(builder, "GL_GOOGLE_cpp_style_line_directive");
        ShaderBuilderAddOp(builder, SpvOpSourceExtension, 8);
        ShaderBuilderAddString(builder, "GL_GOOGLE_include_directive");

        uint32_t orig_len = 0, len =  0, temp = 0;
        ShaderDebugInfo *info;
        for(int i=0;i < builder->num_debug_infos;i++){
            info = &builder->infos[i];

            orig_len = strlen( info->name);

            temp = orig_len;
            while(temp % sizeof(uint32_t))
                temp ++;

            len = temp / sizeof(uint32_t);

            if(orig_len % sizeof(uint32_t))
                len --;

            ShaderBuilderAddOp(builder, SpvOpName, 3 + len);
            ShaderBuilderAddValue(builder, info->indx );
            ShaderBuilderAddString(builder, info->name);

            if(!(orig_len % sizeof(uint32_t)))
                ShaderBuilderAddValue(builder, 0x0);

            for(int j=0;j < info->num_childs;j++){
                orig_len = strlen(info->child_name[j]);

                temp = orig_len;
                while(temp % sizeof(uint32_t))
                    temp ++;

                len = temp / sizeof(uint32_t);

                if(orig_len % sizeof(uint32_t))
                    len --;

                ShaderBuilderAddOp(builder, SpvOpMemberName, 4 + len);
                ShaderBuilderAddValue(builder, info->indx );
                ShaderBuilderAddValue(builder, j);
                ShaderBuilderAddString(builder, info->child_name[j]);

                if(!(orig_len % sizeof(uint32_t)))
                    ShaderBuilderAddValue(builder, 0x0);
            }
        }
    }

    //Decorations
    {

        uint32_t iter = 0;
        for(int i=0;i < 4;i++){
            ShaderBuilderAddOp(builder, SpvOpMemberDecorate, 5);
            ShaderBuilderAddValue(builder, builder->decors[0].indx );
            ShaderBuilderAddValue(builder, i );
            ShaderBuilderAddValue(builder, SpvDecorationBuiltIn);
            ShaderBuilderAddValue(builder, iter );

            iter++;

            if(iter == 2)
                iter ++;
        }

        ShaderBuilderAddOp(builder, SpvOpDecorate, 3);
        ShaderBuilderAddValue(builder, builder->decors[0].indx );
        ShaderBuilderAddValue(builder, SpvDecorationBlock );

        for(int i =0;i < 3;i++){
            ShaderBuilderAddOp(builder, SpvOpMemberDecorate, 5);
            ShaderBuilderAddValue(builder, builder->decors[1].indx );
            ShaderBuilderAddValue(builder, i );
            ShaderBuilderAddValue(builder, SpvDecorationOffset );
            ShaderBuilderAddValue(builder, i * 8 );
        }

        ShaderBuilderAddOp(builder, SpvOpDecorate, 3);
        ShaderBuilderAddValue(builder, builder->decors[1].indx );
        ShaderBuilderAddValue(builder, SpvDecorationBlock );

        ShaderBuilderAddOp(builder, SpvOpDecorate, 4);
        ShaderBuilderAddValue(builder, builder->decors[2].indx );
        ShaderBuilderAddValue(builder, SpvDecorationDescriptorSet );
        ShaderBuilderAddValue(builder, 0 );
        ShaderBuilderAddOp(builder, SpvOpDecorate, 4);
        ShaderBuilderAddValue(builder, builder->decors[2].indx );
        ShaderBuilderAddValue(builder, SpvDecorationBinding );
        ShaderBuilderAddValue(builder, 0 );

        for(int i=1;i < builder->num_io_data;i++){
            ShaderBuilderAddOp(builder, SpvOpDecorate, 4);
            ShaderBuilderAddValue(builder, builder->ioData[i].indx );
            ShaderBuilderAddValue(builder, SpvDecorationLocation );
            ShaderBuilderAddValue(builder, builder->ioData[i].location );
        }
    }

    //Variables
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
                    ShaderBuilderAddValue(builder, variable->values[1]);
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

    //Functions
    {
        ShaderOperand * operand;

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

                for(int l=0;l < builder->functions[i].labels[j].num_operands;l++){
                    operand = &builder->functions[i].labels[j].operands[l];

                    switch(operand->op_type){
                        case SHADER_OPERAND_TYPE_ACCESS:
                            ShaderBuilderAddOp(builder, SpvOpAccessChain, 4 + 1);
                            ShaderBuilderAddValue(builder, operand->var_indx[0]);
                            ShaderBuilderAddValue(builder, operand->indx);
                            ShaderBuilderAddValue(builder, operand->var_indx[1]);
                            ShaderBuilderAddValue(builder, operand->var_indx[2]);
                            break;
                        case SHADER_OPERAND_TYPE_COMPOSITE_EXTRACT:
                            ShaderBuilderAddOp(builder, SpvOpCompositeExtract, 4 + 1);
                            ShaderBuilderAddValue(builder, operand->var_indx[0]);
                            ShaderBuilderAddValue(builder, operand->indx);
                            ShaderBuilderAddValue(builder, operand->var_indx[1]);
                            ShaderBuilderAddValue(builder, operand->var_indx[2]);
                            break;
                        case SHADER_OPERAND_TYPE_COMPOSITE_CONSTRUCT:
                            ShaderBuilderAddOp(builder, SpvOpCompositeConstruct, 3 + 4);
                            ShaderBuilderAddValue(builder, operand->var_indx[0]);
                            ShaderBuilderAddValue(builder, operand->indx);
                            ShaderBuilderAddValue(builder, operand->var_indx[1]);
                            ShaderBuilderAddValue(builder, operand->var_indx[2]);
                            ShaderBuilderAddValue(builder, operand->var_indx[3]);
                            ShaderBuilderAddValue(builder, operand->var_indx[4]);
                            break;
                        case SHADER_OPERAND_TYPE_LOAD:
                            ShaderBuilderAddOp(builder, SpvOpLoad, 4);
                            ShaderBuilderAddValue(builder, operand->var_indx[0]);
                            ShaderBuilderAddValue(builder, operand->indx);
                            ShaderBuilderAddValue(builder, operand->var_indx[1]);
                            break;
                        case SHADER_OPERAND_TYPE_STORE:
                            ShaderBuilderAddOp(builder, SpvOpStore, 3);
                            ShaderBuilderAddValue(builder, operand->var_indx[0]);
                            ShaderBuilderAddValue(builder, operand->var_indx[1]);
                            break;
                        case SHADER_OPERAND_TYPE_ADD:
                            ShaderBuilderAddOp(builder, SpvOpFAdd, 5);
                            ShaderBuilderAddValue(builder, operand->var_indx[0]);
                            ShaderBuilderAddValue(builder, operand->indx);
                            ShaderBuilderAddValue(builder, operand->var_indx[1]);
                            ShaderBuilderAddValue(builder, operand->var_indx[2]);
                            break;
                        case SHADER_OPERAND_TYPE_MUL:
                            ShaderBuilderAddOp(builder, SpvOpFMul, 5);
                            ShaderBuilderAddValue(builder, operand->var_indx[0]);
                            ShaderBuilderAddValue(builder, operand->indx);
                            ShaderBuilderAddValue(builder, operand->var_indx[1]);
                            ShaderBuilderAddValue(builder, operand->var_indx[2]);
                            break;
                    }
                }

                ShaderBuilderAddOp(builder, SpvOpReturn, 1);
            }

            ShaderBuilderAddOp(builder, SpvOpFunctionEnd, 1);
        }
    }

    builder->code[3] = builder->current_index + 1;

    //if(builder->size % sizeof(uint32_t)) ShaderBuilderAddValue(builder, 0x0);
}

void ShaderBuilderWriteToFile(ShaderBuilder *builder, const char *path){

    FILE *somefile = fopen(path, "w");

    fwrite(builder->code, sizeof(uint32_t), builder->size, somefile);

    fclose(somefile);

}
