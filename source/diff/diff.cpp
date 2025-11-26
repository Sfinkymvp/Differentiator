#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


#include "diff/diff.h"
#include "diff/diff_process.h"
#include "diff/diff_optimize.h"

#include "status.h"

#include "graph_dump/html_builder.h"

#include "tex_dump/tex.h"

#include "tree/tree.h"
#include "tree/tree_io.h"



static OperationStatus diffForestResize(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees); assert(diff->forest.capacity != 0);

    void* temp_ptr = realloc(diff->forest.trees, 2 * diff->forest.capacity * sizeof(BinaryTree));
    if (temp_ptr == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;

    diff->forest.trees = (BinaryTree*)temp_ptr;
    diff->forest.capacity *= 2;

    return STATUS_OK;  
}


OperationStatus diffCalculateDerivative(Differentiator* diff, size_t var_idx)
{
    assert(diff); assert(diff->forest.trees); assert(var_idx < diff->var_table.count);

    if (diff->forest.count == diff->forest.capacity)
        diffForestResize(diff);
    assert(diff->forest.count < diff->forest.capacity);

    fprintf(diff->tex_dump.file, "Начинаем вычислять %zu производную функции\n", diff->forest.count);

    TREE_CREATE(&diff->forest.trees[diff->forest.count], "create diff tree");
    diff->forest.trees[diff->forest.count].root = diffNode(diff,
        diff->forest.trees[diff->forest.count - 1].root, var_idx);
    if (!diff->forest.trees[diff->forest.count].root)
        return STATUS_DIFF_CALCULATE_ERROR;

    printExpression(diff, diff->forest.count);
    diff->forest.count++;

    TREE_VERIFY(diff, diff->forest.count - 1, "diff tree");

    return STATUS_OK;
}


double diffOp(Differentiator* diff, const TreeNode* node)
{
    assert(diff);

    double l_res  = diffEvaluate(diff, node->left);
    double r_res = diffEvaluate(diff, node->right);

    switch (node->value.op) {
        case OP_ADD:   return l_res + r_res;
        case OP_SUB:   return l_res - r_res;
        case OP_MUL:   return l_res * r_res;
        case OP_DIV: 
            if (fabs(r_res) < 0.000001) {
                printf("Division by zero\n");
                return NAN;
            }
            return l_res / r_res;
        
        case OP_POW:   return pow(l_res, r_res);
        case OP_LOG:   return log(r_res) / log(l_res);

        case OP_SIN:   return sin(r_res);
        case OP_COS:   return cos(r_res);
        case OP_TAN:   return tan(r_res);
        case OP_COT:   return 1 / tan(r_res);

        case OP_ASIN:  return asin(r_res);
        case OP_ACOS:  return acos(r_res);
        case OP_ATAN:  return atan(r_res);
        case OP_ACOT:  return atan(1 / r_res);

        case OP_SINH:  return sinh(r_res);
        case OP_COSH:  return cosh(r_res);
        case OP_TANH:  return tanh(r_res);
        case OP_COTH:  return 1 / tanh(r_res);
        
        case OP_ASINH: return asinh(r_res);
        case OP_ACOSH: return acosh(r_res);
        case OP_ATANH: return atanh(r_res);
        case OP_ACOTH: return atanh(1 / r_res);

        case OP_NONE:  return 0;
        default:
            printf("Unknown operation for evaluation\n");
            return 0;
    }
}


double diffEvaluate(Differentiator* diff, const TreeNode* node)
{
    assert(diff);

    if (node == NULL)
        return 0;

    switch (node->type) {
        case NODE_OP:  return diffOp(diff, node);
        case NODE_VAR: return diff->var_table.variables[node->value.var_idx].value;
        case NODE_NUM: return node->value.num_val;
        default: fprintf(stderr, "Unknown node type!\n"); return 0;
    }
}

void diffCalculateValue(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(tree_idx < diff->forest.count);
    assert(diff->forest.trees[tree_idx].root);

    double value = diffEvaluate(diff, diff->forest.trees[diff->forest.count - 1].root);
    printf("Expression value: %g\n", value);
}


OperationStatus defineVariables(Differentiator* diff)
{
    assert(diff); assert(diff->var_table.variables);

    for (size_t index = 0; index < diff->var_table.count; index++) {
        printf("What is value of variable '%s'?", diff->var_table.variables[index].name);
        if (scanf("%lf", &diff->var_table.variables[index].value) != 1) {
            fprintf(stderr, "Error: variable %s not defined\n", diff->var_table.variables[index].name);
            return STATUS_IO_INVALID_USER_INPUT;
        }
    }

    return STATUS_OK;
}


OperationStatus diffConstructor(Differentiator* diff, const int argc, const char** argv)
{
    assert(diff); assert(argv);

    OperationStatus status = parseArgs(diff, argc, argv);
    RETURN_IF_STATUS_NOT_OK(status);

    diff->forest.capacity = START_ELEMENT_COUNT;
    diff->forest.count = 0;

    diff->forest.trees = (BinaryTree*)calloc(START_ELEMENT_COUNT, sizeof(BinaryTree));
    if (diff->forest.trees == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;

    diff->var_table.capacity = START_ELEMENT_COUNT;
    diff->var_table.count = 0;

    diff->var_table.variables = (Variable*)calloc(START_ELEMENT_COUNT, sizeof(Variable));
    if (diff->var_table.variables == NULL) {
        free(diff->forest.trees);
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    }

    openGraphDumpFile(diff);
    texInit(diff);

    return STATUS_OK;
}


void diffDestructor(Differentiator* diff)
{
    assert(diff);

    for (size_t index = 0; index < diff->forest.count; index++)
        treeDestructor(&diff->forest.trees[index]);
    free(diff->forest.trees);
    diff->forest.trees = NULL;

    for (size_t index = 0; index < diff->var_table.count; index++)
        free(diff->var_table.variables[index].name);
    free(diff->var_table.variables);
    diff->var_table.variables = NULL;

    assert(fclose(diff->graph_dump.file) == 0);

    texClose(diff);

    diff->graph_dump.file = NULL;
}
