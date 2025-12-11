#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


#include "diff/diff.h"
#include "diff/diff_process.h"
#include "diff/diff_optimize.h"
#include "diff/diff_taylor.h"
#include "diff/diff_cmd_args.h"

#include "status.h"

#include "graph_dump/html_builder.h"

#include "tex_dump/tex_struct.h"
#include "tex_dump/plot_generator.h"

#include "tree/tree.h"
#include "tree/tree_io.h"


#define STRING(string) #string
#define CREATE_ERROR_INFO(enum, description) [enum] = {enum, #enum, description}


const ErrorInfo ErrorTable[ERROR_COUNT] = {
// Successful status
    CREATE_ERROR_INFO(STATUS_OK,                      "Operation completed successfully."),
// Tree Errors
    CREATE_ERROR_INFO(STATUS_TREE_ROOT_HAS_PARENT,    "Root node must not have a parent."),
    CREATE_ERROR_INFO(STATUS_TREE_MISSING_PARENT,     "Node references an invalid or missing parent."),
    CREATE_ERROR_INFO(STATUS_TREE_PARENT_CHILD_MISMATCH, "Parent-child links do not match."),
    CREATE_ERROR_INFO(STATUS_TREE_INVALID_BRANCH_STRUCTURE, "Detected an invalid branch structure."),
// Differentiation Errors
    CREATE_ERROR_INFO(STATUS_DIFF_CALCULATE_ERROR,    "An error occurred during expression calculation."),
    CREATE_ERROR_INFO(STATUS_DIFF_UNKNOWN_VARIABLE,   "Differentiation attempted on an unknown variable."),
    CREATE_ERROR_INFO(STATUS_DIFF_CONST_EXPRESSION,   "There are no variables in the passed expression."),
// System Errors
    CREATE_ERROR_INFO(STATUS_SYSTEM_OUT_OF_MEMORY,    "Failed to allocate memory (Out of Memory)."),
    CREATE_ERROR_INFO(STATUS_SYSTEM_CALL_ERROR,       "Error during execution of a system call."),
// Command Line Argument Errors
    CREATE_ERROR_INFO(STATUS_CLI_UNKNOWN_OPTION,      "An unknown command-line option was provided."),
// Syntax Errors
    CREATE_ERROR_INFO(STATUS_PARSER_INVALID_IDENTIFIER, "An invalid identifier was encountered by the parser."),
// I/O Errors
    CREATE_ERROR_INFO(STATUS_IO_INVALID_USER_INPUT,   "Invalid input data received from the user."),
    CREATE_ERROR_INFO(STATUS_IO_FILE_OPEN_ERROR,      "Failed to open the specified file."),
    CREATE_ERROR_INFO(STATUS_IO_FILE_EMPTY,           "The file is empty (0 bytes)."),
    CREATE_ERROR_INFO(STATUS_IO_FILE_READ_ERROR,      "An error occurred while reading the file."),
    CREATE_ERROR_INFO(STATUS_IO_FILE_CLOSE_ERROR,     "Error during file closing operation.")
};


static OperationStatus diffForestResize(Differentiator* diff);


void printErrorStatus(OperationStatus status)
{
    assert(status < ERROR_COUNT);

    fprintf(stderr, "[%s] %s\n", ErrorTable[status].status_string, ErrorTable[status].error_message);
}


OperationStatus diffCalculateDerivative(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->forest.trees); assert(tree_idx < diff->forest.count);

    if (diff->forest.count >= diff->forest.capacity - 1)
        diffForestResize(diff);
    assert(diff->forest.count < diff->forest.capacity);

    if (diff->tex_dump.print_steps) {
        printTex(diff, "\n\\subsection{Вычисление}\n");
    }
    TREE_CREATE(&diff->forest.trees[tree_idx + 1]);
    diff->forest.trees[tree_idx + 1].root = diffNode(diff,
        diff->forest.trees[tree_idx].root);
    if (!diff->forest.trees[tree_idx + 1].root)
        return STATUS_DIFF_CALCULATE_ERROR;
    TREE_VERIFY(diff, tree_idx + 1, "differentiation process");

    printTex(diff, "\n\\subsection{Результат вычисления}\n");
    printExpression(diff, diff->forest.count);

    diff->forest.count++;
    return STATUS_OK;
}


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


OperationStatus diffConstructor(Differentiator* diff, const int argc, const char** argv)
{
    assert(diff); assert(argv);

    OperationStatus status = parseArgs(diff, argc, argv);
    RETURN_IF_STATUS_NOT_OK(status);

    diff->forest.capacity = START_ELEMENT_COUNT;
    diff->forest.count = 0;
    diff->highlight_node = NULL;
    diff->graph_dump.file = NULL;
    diff->tex_dump.print_steps = true;
    diff->tex_dump.range.x_min = -5;
    diff->tex_dump.range.x_max = 5;
    diff->tex_dump.range.y_min = -10;
    diff->tex_dump.range.y_max = 10;

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

    OperationStatus status = texClose(diff);
    if (status != STATUS_OK) {
        printErrorStatus(status);
    }

    diff->graph_dump.file = NULL;
}
