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

#include "tex_dump/tex.h"
#include "tex_dump/plot_generator.h"


static double factorial(size_t n);


 void diffTaylorSeries(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees);

    size_t tree_idx = diff->forest.count;
    TREE_CREATE(&diff->forest.trees[tree_idx], "creating taylor tree");

    setVariableValue(diff, diff->args.derivative_info.diff_var, diff->args.taylor_info.center);
    diff->forest.trees[tree_idx].root = createTaylorTree(diff, 0);
    assert(diff->forest.trees[tree_idx].root);
    TREE_DUMP(diff, tree_idx, STATUS_OK, "creating taylor tree");

    fprintf(TEX_FILE, "\\chapter{Разложение функции по формуле Тейлора}");
    fprintf(TEX_FILE, "Напомню, что наша функция выглядит следующим образом:\n");
    printExpression(diff, 0);
    fprintf(TEX_FILE, "Разложим функцию по формуле Тейлора с остаточным членом в форме Пеано\n");
    printTaylorTree(diff, diff->forest.trees[tree_idx].root);

    char output_file[BUFFER_SIZE] = "";
    snprintf(output_file, BUFFER_SIZE, "%s/%s_%03zu", GNUPLOT_IMAGES_DIRECTORY,
        GNUPLOT_OUTPUT_FILENAME, tree_idx);

    OperationStatus status = generatePlot(diff, diff->forest.trees[tree_idx].root, output_file, tree_idx, true);
    treeDestructor(&diff->forest.trees[tree_idx]);
    if (status != STATUS_OK) return;

    fprintf(TEX_FILE, "Посмотрим, как выглядит график функции и ее разложения в точке %g\n",
        diff->args.taylor_info.center);
    fprintf(TEX_FILE, "\\begin{figure}[H]\n");
    fprintf(TEX_FILE, "\\centering\n");
    fprintf(TEX_FILE, "\\includegraphics[width=0.8\\textwidth]{%s}\n", output_file);
    fprintf(TEX_FILE, "\\caption{Сравнение исходной функции и ее приближения полиномом Тейлора $T_n(x)$.}\n");
    fprintf(TEX_FILE, "\\end{figure}\n");
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
    TreeNode* current_derivative = MUL(CNUM(fabs(derivative_value)),
                                       POW(SUB(CVAR(diff->args.derivative_info.diff_var),
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


void printTaylorTree(Differentiator* diff, TreeNode* taylor_root)
{
    assert(diff); assert(TEX_FILE); assert(taylor_root);

    TreeNode* head_node = NULL;
    TreeNode* mul_node = NULL;
    TreeNode* pow_node = NULL;
    TreeNode* sub_node = NULL;

    fprintf(TEX_FILE, "\\begin{dmath*}\n");
    for (head_node = taylor_root; head_node->type == NODE_OP; head_node = head_node->right) {
        mul_node = head_node->left;
        assert(mul_node); assert(mul_node->left); assert(mul_node->right);

        if (head_node != taylor_root) {
            if (mul_node->left->value.num_val < 0) {
                fprintf(TEX_FILE, " - ");
            } else {
                fprintf(TEX_FILE, " + ");
            }
        }
        fprintf(TEX_FILE, "%g", fabs(mul_node->left->value.num_val));
 
        pow_node = mul_node->right;
        assert(pow_node); assert(pow_node->left); assert(pow_node->right);
        sub_node = pow_node->left;
        assert(sub_node); assert(sub_node->left); assert(sub_node->right);

        if (fabs(diff->args.taylor_info.center) < EPS) {
            fprintf(TEX_FILE, "%s^%g",
                diff->var_table.variables[diff->args.derivative_info.diff_var].name, pow_node->right->value.num_val);
        } else {
            fprintf(TEX_FILE, "(%s", diff->var_table.variables[diff->args.derivative_info.diff_var].name);
            if (sub_node->right->value.num_val >= 0) {
                fprintf(TEX_FILE, " - %g)^%g", fabs(sub_node->right->value.num_val), pow_node->right->value.num_val);
            } else {
                fprintf(TEX_FILE, " + %g)^%g", fabs(sub_node->right->value.num_val), pow_node->right->value.num_val);
            }
        }
        
    }

    if (fabs(diff->args.taylor_info.center) < EPS) {
        fprintf(TEX_FILE, " + o(%s^%g)\n",
            diff->var_table.variables[diff->args.derivative_info.diff_var].name, pow_node->right->value.num_val);
    } else {
        fprintf(TEX_FILE, " + o(%s", diff->var_table.variables[diff->args.derivative_info.diff_var].name);
        if (sub_node->right->value.num_val >= 0) {
            fprintf(TEX_FILE, " - %g)^%g\n", fabs(sub_node->right->value.num_val), pow_node->right->value.num_val);
        } else {
            fprintf(TEX_FILE, " + %g)^%g\n", fabs(sub_node->right->value.num_val), pow_node->right->value.num_val);
        }
    }
 
    fprintf(TEX_FILE, "\n\\end{dmath*}\n");
}
