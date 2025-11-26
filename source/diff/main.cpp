#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "diff/diff_defs.h"
#include "diff/diff.h"
#include "diff/diff_process.h"
#include "diff/diff_optimize.h"

#include "status.h"

#include "tex_dump/tex.h"

#include "tree/tree.h"
#include "tree/tree_io.h"


int main(const int argc, const char** argv)
{
    Differentiator diff = {};
    OperationStatus status = diffConstructor(&diff, argc, argv);
    if (status != STATUS_OK)
        return 1;

    status = diffLoadExpression(&diff);
    if (status == STATUS_OK)
        printIntroduction(&diff);

    TREE_DUMP(&diff, 0, STATUS_OK, "source tree");
    if (status == STATUS_OK)
        status = defineVariables(&diff);
    if (status == STATUS_OK) {
        for (size_t index = 1; index <= diff.args.derivative_order; index++) {
            status = diffCalculateDerivative(&diff, 0);
            if (status != STATUS_OK) {
                printf("status: %d\n", status);
                printf("lox\n");
                break;
            }
            printf("INDEX: %zu, VALUE:\n", index);
            diffCalculateValue(&diff, index);
            optimizeTree(&diff, index);
            printf("INDEX: %zu, VALUE:\n", index);
            diffCalculateValue(&diff, index);
        }
    }

    diffDestructor(&diff);

    return 0;
}


