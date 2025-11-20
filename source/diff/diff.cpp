#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "diff/diff.h"
#include "diff/diff_var_table.h"
#include "graph_dump/html_builder.h"
#include "diff/diff_defs.h"
#include "tree/tree.h"
#include "status.h"


OperationStatus diffConstructor(Differentiator* diff)
{
    assert(diff);

    diff->forest.capacity = START_ELEMENT_COUNT;
    diff->forest.count = 0;

    diff->forest.trees = (BinaryTree*)calloc(START_ELEMENT_COUNT, sizeof(BinaryTree));
    if (diff->forest.trees == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;

    diff->var_table.capacity = START_ELEMENT_COUNT;
    diff->var_table.count = 0;

    diff->var_table.variables = (Variable*)calloc(START_ELEMENT_COUNT, sizeof(Variable));
    if (diff->var_table.variables == NULL) {
        free(diff->forest.trees);
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    }

    openDumpFile(diff);
    return STATUS_OK;
}


void diffDestructor(Differentiator* diff)
{
    assert(diff);

    for (size_t index = 0; index < diff->forest.count; index++)
        treeDestructor(&diff->forest.trees[index]);
    free(diff->forest.trees);
    diff->forest.trees = NULL;

    for (size_t index = 0; index < diff->var_table.count; index++)
        free(diff->var_table.variables[index].name);
    free(diff->var_table.variables);
    diff->var_table.variables = NULL;

    assert(fclose(diff->dump_state.dump_file) == 0);
    diff->dump_state.dump_file = NULL;
}