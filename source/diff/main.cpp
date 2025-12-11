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
        if (diff.var_table.count == 0) {
            status = STATUS_DIFF_CONST_EXPRESSION;
            
        } else {
            printIntroduction(&diff);
        }
    }

    if (status == STATUS_OK && diff.args.derivative_info.compute) {
        status = defineVariables(&diff);
    }
    if (status == STATUS_OK) {
        for (size_t index = 1; index <= diff.args.derivative_info.order; index++) {
            if (index > 3) {
                diff.tex_dump.print_steps = false;
            }

            printTex(&diff, "\\chapter{%zu-я производная}", index);
            status = diffCalculateDerivative(&diff, index - 1);
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
