#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "diff/diff.h"
#include "diff/diff_defs.h"
#include "status.h"
#include "tree/tree.h"
#include "input/tree_io.h"

int main()
{
    Differentiator diff = {};

    OperationStatus status = diffConstructor(&diff);
    if (status != STATUS_OK)
        return 1;

    FILE* input_file = fopen("../data/test", "r");
    status = treeLoadExpression(&diff, &diff.forest.trees[0], input_file);
    diff.forest.count++;
    assert(fclose(input_file) == 0);
   
    diffDestructor(&diff);

    return 0;
}


