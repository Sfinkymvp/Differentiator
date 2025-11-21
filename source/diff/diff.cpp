#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


#include "diff/diff.h"
#include "diff/diff_dsl.h"
#include "diff/diff_var_table.h"
#include "graph_dump/html_builder.h"
#include "diff/diff_defs.h"
#include "tree/tree.h"
#include "tree/tree_io.h"
#include "status.h"


static OperationStatus diffForestResize(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees); assert(diff->forest.capacity != 0);

    void* temp_ptr = realloc(diff->forest.trees, diff->forest.capacity * 2);
    if (temp_ptr == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;

    diff->forest.trees = (BinaryTree*)temp_ptr;
    diff->forest.capacity *= 2;

    return STATUS_OK;  
}


OperationStatus diffTree(Differentiator* diff, size_t diff_var)
{
    assert(diff);

    if (diff->forest.count == diff->forest.capacity)
        diffForestResize(diff);
    assert(diff->forest.count < diff->forest.capacity);

    TREE_CREATE(&diff->forest.trees[diff->forest.count], "create diff tree");
    diff->forest.trees[diff->forest.count].root = diffNode(
        diff->forest.trees[diff->forest.count - 1].root, diff_var);
    diff->forest.count++;
    assert(diff->forest.trees[diff->forest.count- 1].root);
    TREE_VERIFY(diff, &diff->forest.trees[diff->forest.count - 1], "diff tree");

    diff->forest.count++;
    return STATUS_OK;
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


double diffOp(Differentiator* diff, const TreeNode* node)
{
    assert(diff);

    double left_res  = diffEvaluate(diff, node->left);
    double right_res = diffEvaluate(diff, node->right);

    switch (node->value.op) {
        case OP_ADD: return left_res + right_res;
        case OP_SUB: return left_res - right_res;
        case OP_MUL: return left_res * right_res;
        case OP_DIV: 
            if (fabs(right_res) < 0.000001) {
                printf("Division by zero\n");
                return NAN;
            }
            return left_res / right_res;
        case OP_SIN: return sin(right_res);
        case OP_COS: return cos(right_res);
        default:
            printf("Unknown operation for evaluation\n");
            return 0;
    }
}


void diffCalculateValue(Differentiator* diff)
{
    assert(diff);

    for (size_t index = 0; index < diff->var_table.count; index++) {
        printf("What is value of variable '%s'?", diff->var_table.variables[index].name);
        if (scanf("%lf", &diff->var_table.variables[index].value) != 1)
            return;
    }
    double value = diffEvaluate(diff, diff->forest.trees[diff->forest.count - 1].root);
    printf("Expression value: %lf\n", value);
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

    openDumpFile(diff);
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

    assert(fclose(diff->dump_state.dump_file) == 0);
    diff->dump_state.dump_file = NULL;
}
