#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "diff/diff_var_table.h"
#include "diff/diff_defs.h"
#include "status.h"


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


OperationStatus addVariable(Differentiator* diff, size_t* var_idx, const char* variable)
{
    assert(diff); assert(diff->var_table.variables); assert(var_idx); assert(variable);

    for (size_t index = 0; index < diff->var_table.count; index++) {
        if (strcmp(variable, diff->var_table.variables[index].name) == 0) { 
            *var_idx = index;
            return STATUS_OK;
        }
    }

    if (diff->var_table.count == diff->var_table.capacity) {
        OperationStatus status = variableTableResize(diff);
        RETURN_IF_STATUS_NOT_OK(status);
    }
    assert(diff->var_table.count < diff->var_table.capacity);

    char* temp_ptr = strdup(variable);
    if (temp_ptr == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;

    diff->var_table.variables[diff->var_table.count].name = temp_ptr;
    diff->var_table.variables[diff->var_table.count].value = 0;
    *var_idx = diff->var_table.count;
    diff->var_table.count++;

    return STATUS_OK;
}

