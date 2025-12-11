#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <assert.h>

#include "diff/diff_defs.h"
#include "diff/diff.h"
#include "diff/diff_process.h"
#include "diff/diff_optimize.h"
#include "diff/diff_evaluate.h"
#include "diff/diff_var_table.h"
#include "diff/diff_taylor.h"

#include "status.h"

#include "tex_dump/tex_struct.h"
#include "tex_dump/plot_generator.h"

#include "tree/tree.h"
#include "tree/tree_io.h"


int main(const int argc, const char** argv)
{
    Differentiator diff = {};
    OperationStatus status = diffConstructor(&diff, argc, argv);
    if (status != STATUS_OK) {
        printErrorStatus(status);
        return 1;
    }

    status = diffLoadExpression(&diff);
    if (status == STATUS_OK) {
        printIntroduction(&diff);
    }

    if (status == STATUS_OK && diff.args.derivative_info.compute) {
        status = defineVariables(&diff);
    }
    if (status == STATUS_OK) {
        for (size_t index = 1; index <= diff.args.derivative_info.order; index++) {
            fprintf(diff.tex_dump.file, "\\chapter{%zu-я производная}", index);
            status = diffCalculateDerivative(&diff, 0);
            if (status != STATUS_OK) {
                break;
            }

            optimizeTree(&diff, index);
            if (diff.args.derivative_info.compute)
                diffEvaluate(&diff, index);

            printPlot(&diff, index);
        }
    }

    if (status == STATUS_OK && diff.args.taylor_info.decomposition) {
        diffTaylorSeries(&diff);
    }

    diffDestructor(&diff);
    if (status != STATUS_OK) {
        printErrorStatus(status);
        return 1;
    } else {
        return 0;
    }
}
