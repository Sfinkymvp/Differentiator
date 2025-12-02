#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "diff/diff_defs.h"
#include "diff/diff.h"
#include "diff/diff_process.h"
#include "diff/diff_optimize.h"
#include "diff/diff_evaluate.h"

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

    TREE_DUMP(&diff, 0, status, "source tree");
    if (status == STATUS_OK && diff.args.compute_derivative) {
        status = defineVariables(&diff);
    }
    if (status == STATUS_OK) {
        for (size_t index = 1; index <= diff.args.derivative_order; index++) {
            status = diffCalculateDerivative(&diff, 0);
            if (status != STATUS_OK) {
                printf("status: %d\n", status);
                printf("lox\n");
                break;
            }

            optimizeTree(&diff, index);
            if (diff.args.compute_derivative)
                diffEvaluate(&diff, index);
        }
    }

    if (status == STATUS_OK && diff.args.taylor_decomposition)
        printTaylorSeries(&diff);

    diffDestructor(&diff);
    return 0;
}


