#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "diff/diff.h"
#include "diff/diff_defs.h"
#include "status.h"
#include "tree/tree.h"
#include "tree/tree_io.h"

int main(const int argc, const char** argv)
{
    Differentiator diff = {};
    OperationStatus status = diffConstructor(&diff, argc, argv);
    if (status != STATUS_OK)
        return 1;

    TREE_CREATE(&diff.forest.trees[0], "");
    status = diffLoadExpression(&diff, &diff.forest.trees[0]);
    diff.forest.count++;
    TREE_DUMP(&diff, &diff.forest.trees[0], STATUS_OK, "source tree");

    
    diffCalculateValue(&diff);
    status = diffTree(&diff, 0);
    printf("status: %d\n", (int)status);
    diffCalculateValue(&diff);

    diffDestructor(&diff);

    return 0;
}


