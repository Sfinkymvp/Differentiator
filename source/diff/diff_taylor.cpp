#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


#include "diff/diff_taylor.h"
#include "diff/diff_defs.h"
#include "diff/diff_dsl.h"
#include "diff/diff_evaluate.h"
#include "diff/diff_var_table.h"

#include "status.h"

#include "tree/tree.h"

#include "tex_dump/tex_struct.h"
#include "tex_dump/plot_generator.h"


static double factorial(size_t n);


 void diffTaylorSeries(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees);

    size_t tree_idx = diff->forest.count;
    TREE_CREATE(&diff->forest.trees[tree_idx], "creating taylor tree");

    setVariableValue(diff, diff->args.derivative_info.diff_var_idx, diff->args.taylor_info.center);
    diff->forest.trees[tree_idx].root = createTaylorTree(diff, 0);
    assert(diff->forest.trees[tree_idx].root);
    TREE_DUMP(diff, tree_idx, STATUS_OK, "creating taylor tree");

    printTex(diff,
        "\\chapter{Разложение функции по формуле Тейлора}\n"
        "Напомню, что наша функция выглядит следующим образом:\n");
    printExpression(diff, 0);
    printTex(diff, "Разложим функцию по формуле Тейлора с остаточным членом в форме Пеано\n");
    printExpression(diff, tree_idx);

    char output_filename[BUFFER_SIZE] = "";
    snprintf(output_filename, BUFFER_SIZE, "%s/%s_%03zu", GNUPLOT_IMAGES_DIRECTORY,
        GNUPLOT_OUTPUT_FILENAME, tree_idx);

    OperationStatus status = generatePlot(diff, output_filename, 2, 0, tree_idx);
    
    treeDestructor(&diff->forest.trees[tree_idx]);
    if (status != STATUS_OK) return;

    printTex(diff,
        "Посмотрим, как выглядит график функции и ее разложения в точке %g\n"
        "\\begin{figure}[H]\n"
        "\\centering\n"
        "\\includegraphics[width=0.8\\textwidth]{%s}\n"
        "\\caption{Сравнение исходной функции и ее приближения по формуле Тейлора.}\n"
        "\\end{figure}\n", diff->args.taylor_info.center, output_filename);
}


TreeNode* createTaylorTree(Differentiator* diff, size_t derivative_counter)
{
    assert(diff); assert(diff->forest.trees); assert(diff->forest.count);

    if (derivative_counter > diff->args.derivative_info.order) {
        return CNUM(0);
    }

    double derivative_value = evaluateNode(diff,
        diff->forest.trees[derivative_counter].root) / factorial(derivative_counter);
    TreeNode* next_derivative = createTaylorTree(diff, derivative_counter + 1);
    if (fabs(derivative_value) < EPS) {
        return next_derivative;
    }
    TreeNode* current_derivative = MUL(CNUM(derivative_value),
                                       POW(SUB(CVAR(diff->args.derivative_info.diff_var_idx),
                                               CNUM(diff->args.taylor_info.center)),
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