#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


#include "diff/diff_taylor.h"
#include "diff/diff_defs.h"
#include "diff/diff_create.h"
#include "diff/diff_evaluate.h"
#include "diff/diff_var_table.h"
#include "diff/diff_optimize.h"

#include "status.h"

#include "tree/tree.h"

#include "tex_dump/tex_struct.h"
#include "tex_dump/plot_generator.h"

#include "tree/tree.h"


static double factorial(size_t n);


OperationStatus diffTaylorSeries(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees);

    size_t tree_idx = diff->forest.count;
    TREE_CREATE(&diff->forest.trees[tree_idx]);

    setVariableValue(diff, diff->args.derivative_info.diff_var_idx, diff->args.taylor_info.center);
    diff->forest.trees[tree_idx].root = createTaylorTree(diff, 0);
    if (diff->forest.trees[tree_idx].root == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    TREE_DUMP(diff, tree_idx, STATUS_OK, "Creating Taylor Tree");

    diff->tex_dump.print_steps = false;
    optimizeTree(diff, tree_idx);
    diff->tex_dump.print_steps = true;
    TREE_DUMP(diff, tree_idx, STATUS_OK, "Optimizing Taylor Tree");

    char output_filename[BUFFER_SIZE] = "";
    snprintf(output_filename, BUFFER_SIZE, "%s/%s_%03zu", GNUPLOT_IMAGES_DIRECTORY,
        GNUPLOT_OUTPUT_FILENAME, tree_idx);

    OperationStatus status = generatePlot(diff, output_filename, 2, 0, tree_idx);

    if (status == STATUS_OK) {
        printTaylorSeries(diff, output_filename, tree_idx);
    }

    treeDestructor(&diff->forest.trees[tree_idx]);
    return status;

}


TreeNode* createTaylorTree(Differentiator* diff, size_t derivative_counter)
{
    assert(diff); assert(diff->forest.trees); assert(diff->forest.count);

    if (derivative_counter > diff->args.derivative_info.order) {
        return NULL;
    }

    double derivative_value = evaluateNode(diff,
        diff->forest.trees[derivative_counter].root) / factorial(derivative_counter);
    TreeNode* next_derivative = createTaylorTree(diff, derivative_counter + 1);
    if (fabs(derivative_value) < EPS) {
        return next_derivative;
    }
    TreeNode* current_derivative = MUL(CNUM(derivative_value), 
        POW(SUB(CVAR(diff->args.derivative_info.diff_var_idx), CNUM(diff->args.taylor_info.center)),
        CNUM((double)derivative_counter)));

    return ADD(current_derivative, next_derivative);
}


static double factorial(size_t number)
{
    if (number == 0 || number == 1) return 1;

    double result = 1;
    for (size_t index = 2; index <= number; index++)
        result *= (double)index;

    return result;
}


// undef diff_create.h

#undef CNUM
#undef CVAR

#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef POW
#undef LOG

#undef SIN
#undef COS
#undef TAN
#undef COT

#undef ASIN
#undef ACOS
#undef ATAN
#undef ACOT

#undef SINH
#undef COSH
#undef TANH
#undef COTH

#undef ASINH
#undef ACOSH
#undef ATANH
#undef ACOTH
