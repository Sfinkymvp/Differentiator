#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "diff/diff_defs.h"
#include "status.h"
#include "tree/tree.h"
#include "input/tree_io.h"

int main()
{
    Differentiator diff = {};
    diff.trees = (BinaryTree*)calloc(1, sizeof(BinaryTree));
    openDumpFile(&diff);
    assert(diff.dump_state.dump_file);
    assert(diff.trees);
    TREE_CREATE(&diff.trees[0], "hello");

    diff.var_table.variables = (Variable*)calloc(4, sizeof(Variable));
    diff.var_table.capacity = 4;
   
    FILE* input_file = fopen("../data/test", "r");
    assert(input_file);

    OperationStatus status = treeLoadExpression(&diff, &diff.trees[0], input_file);

    TREE_VERIFY(&diff, &diff.trees[0], "main dump");

    assert(status == STATUS_OK);
    assert(fclose(input_file) == 0);

    for (size_t index = 0; index < diff.var_table.count; index++)
        free(diff.var_table.variables[index].name);
    free(diff.var_table.variables);

    treeDestructor(&diff.trees[0]);
    free(diff.trees);
    fclose(diff.dump_state.dump_file);

    return 0;
}


