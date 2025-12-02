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
    assert(diff); assert(diff->forest.trees); 

    if (diff->forest.count == diff->forest.capacity)
        diffForestResize(diff);
    assert(diff->forest.count < diff->forest.capacity);

    fprintf(diff->tex_dump.file, "Начинаем вычислять %zu производную функции:\n", diff->forest.count);

    TREE_CREATE(&diff->forest.trees[diff->forest.count], "create diff tree");
    diff->forest.trees[diff->forest.count].root = diffNode(diff,
        diff->forest.trees[diff->forest.count - 1].root, var_idx);
    if (!diff->forest.trees[diff->forest.count].root)
        return STATUS_DIFF_CALCULATE_ERROR;

    fprintf(diff->tex_dump.file, "\nПосле дифференцирования:\n");
    printExpression(diff, diff->forest.count);
    diff->forest.count++;

    TREE_VERIFY(diff, diff->forest.count - 1, "diff tree");

    return STATUS_OK;
}


void diffTaylorSeries(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees);

    fprintf(diff->tex_dump.file, "\\chapter{Разложение функции по формуле Тейлора}");
    fprintf(diff->tex_dump.file, "Напомню, что наша функция выглядит следующим образом:\n");
    printExpression(diff, 0);
    diff->var_table.variables[0].value = diff->args.taylor_center;

    fprintf(diff->tex_dump.file, "Разложим его по формуле Тейлора с остаточным членом в форме Пеано:\n");
    printTaylorSeries(diff);
}





OperationStatus defineVariables(Differentiator* diff)
{
    assert(diff); assert(diff->var_table.variables);

    for (size_t index = 0; index < diff->var_table.count; index++) {
        printf("Value of variable '%s': ", diff->var_table.variables[index].name);
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
