#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "diff/diff_var_table.h"
#include "diff/diff_defs.h"
#include "status.h"


static OperationStatus findVariable(Differentiator* diff, size_t* var_idx, const char* variable, size_t var_len);
static OperationStatus variableTableResize(Differentiator* diff);


void setVariableValue(Differentiator* diff, size_t var_idx, double value)
{
    assert(diff); assert(diff->var_table.variables); assert(var_idx < diff->var_table.count);

    diff->var_table.variables[var_idx].value = value;
}


OperationStatus defineDiffVariable(Differentiator* diff)
{
    assert(diff); assert(diff->var_table.variables);

    for (size_t index = 0; index < diff->var_table.count; index++) {
        if (strcmp(diff->var_table.variables[index].name, diff->args.derivative_info.diff_var_s) == 0) {
            diff->args.derivative_info.diff_var_idx = index;
            return STATUS_OK;
        }
    }

    return STATUS_DIFF_UNKNOWN_VARIABLE;
}


OperationStatus defineVariables(Differentiator* diff)
{
    assert(diff); assert(diff->var_table.variables);

    for (size_t index = 0; index < diff->var_table.count; index++) {
        printf("Value of variable '%s': ", diff->var_table.variables[index].name);
        if (scanf("%lf", &diff->var_table.variables[index].value) != 1) {
            fprintf(stderr, "Error: variable %s not defined\n", diff->var_table.variables[index].name);
            return STATUS_IO_INVALID_USER_INPUT;
        }
    }

    return STATUS_OK;
}


OperationStatus addVariable(Differentiator* diff, size_t* var_idx, const char* variable, size_t var_len)
{
    assert(diff); assert(diff->var_table.variables); assert(var_idx); assert(variable);

    OperationStatus status = findVariable(diff, var_idx, variable, var_len);
    if (status == STATUS_OK)
        return STATUS_OK;
    if (diff->var_table.count == diff->var_table.capacity) {
        status = variableTableResize(diff);
        RETURN_IF_STATUS_NOT_OK(status);
    }
    assert(diff->var_table.count < diff->var_table.capacity);

    char* temp_ptr = (char*)calloc(var_len + 1, 1);
    if (temp_ptr == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    strncpy(temp_ptr, variable, var_len);
    temp_ptr[var_len] = '\0';

    diff->var_table.variables[diff->var_table.count].name = temp_ptr;
    diff->var_table.variables[diff->var_table.count].value = 0;
    *var_idx = diff->var_table.count;
    diff->var_table.count++;

    return STATUS_OK;
}


static OperationStatus findVariable(Differentiator* diff, size_t* var_idx, const char* variable, size_t var_len)
{
    assert(diff); assert(diff->var_table.variables); assert(var_idx); assert(variable);

    for (size_t index = 0; index < diff->var_table.count; index++) {
        const char* existing_name = diff->var_table.variables[index].name;
        if (strncmp(variable, existing_name, var_len) == 0 && existing_name[var_len] == '\0') { 
            *var_idx = index;
            return STATUS_OK;
        }
    }

    return STATUS_DIFF_UNKNOWN_VARIABLE;
}


static OperationStatus variableTableResize(Differentiator* diff)
{
    assert(diff); assert(diff->var_table.variables); assert(diff->var_table.capacity != 0);

    void* temp_ptr = realloc(diff->var_table.variables, diff->var_table.capacity * 2);
    if (temp_ptr == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;

    diff->var_table.variables = (Variable*)temp_ptr;
    diff->var_table.capacity *= 2;

    return STATUS_OK;
}

