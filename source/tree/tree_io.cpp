#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

#include "tree/tree_io.h"
#include "tree/tree.h"
#include "tree/tree_parse.h"

#include "diff/diff_var_table.h"
#include "diff/diff_defs.h"

#include "status.h"


static size_t getFileSize(FILE* input_file)
{
    assert(input_file != NULL);

    struct stat buf = {};

    if (fstat(fileno(input_file), &buf) == -1)
        return 0;

    return (size_t)buf.st_size;
}


OperationStatus diffLoadExpression(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees); assert(diff->args.input_file);

    OperationStatus status = TREE_CREATE(&diff->forest.trees[0], "");
    RETURN_IF_STATUS_NOT_OK(status);

    FILE* input_file = fopen(diff->args.input_file, "r");
    if (input_file == NULL)
        return STATUS_IO_FILE_OPEN_ERROR;

    if (diff->args.infix_input)
        status = treeInfixLoad(diff, 0, input_file);
    else 
        status = treePrefixLoad(diff, 0, input_file);
    if (fclose(input_file) != 0 && status == STATUS_OK)
        status = STATUS_IO_FILE_CLOSE_ERROR;

    diff->forest.count++;    
    return status;
}


OperationStatus treeInfixLoad(Differentiator* diff, size_t tree_idx, FILE* input_file)
{
    assert(diff); assert(tree_idx < diff->forest.capacity); assert(input_file);
 
    size_t file_size = getFileSize(input_file);
    if (file_size == 0)
        return STATUS_IO_FILE_EMPTY;

    char* src_code = (char*)calloc(file_size + 1, 1);
    if (src_code == NULL) {
        fclose(input_file);
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    }

    size_t read_size = fread(src_code, 1, file_size, input_file);
    if (read_size != file_size)
        return STATUS_IO_FILE_READ_ERROR;

    char* buffer = src_code;
    diff->forest.trees[tree_idx].root = getTree(&buffer);
    free(src_code);
    if (diff->forest.trees[tree_idx].root == NULL)
        return STATUS_IO_FILE_READ_ERROR;

    return STATUS_OK;
}


OperationStatus treePrefixLoad(Differentiator* diff, size_t tree_idx, FILE* input_file)
{
    assert(diff); assert(diff->forest.trees); assert(input_file);
    
    size_t file_size = getFileSize(input_file);
    if (file_size == 0)
        return STATUS_IO_FILE_EMPTY;

    char* src_code = (char*)calloc(file_size + 1, 1);
    if (src_code == NULL) {
        fclose(input_file);
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    }

    OperationStatus status = STATUS_OK;
    size_t read_size = fread(src_code, 1, file_size, input_file);
    if (read_size != file_size) {
        status = STATUS_IO_FILE_READ_ERROR;
    }
    if (status == STATUS_OK) {
        int position = 0;    
        status = readNode(&diff->forest.trees[tree_idx].root, diff, src_code, &position);
    }

    free(src_code);
    return status;
}


static OpType getOpType(const char* buffer)
{
    assert(buffer);

    for (int index = 0; OP_TABLE[index].op != OP_NONE; index++) {
        if (strcmp(buffer, OP_TABLE[index].symbol) == 0)
            return OP_TABLE[index].op;
    }

    return OP_NONE;
}


OperationStatus readTitle(TreeNode* node, Differentiator* diff,
                          char* src_code, int* position)
{
    assert(node); ; assert(diff); assert(diff->var_table.variables); 
    assert(src_code); assert(position);

    char buffer[BUFFER_SIZE] = {};
    int read_count = 0;
    int result = sscanf(&src_code[*position], "%s%n", buffer, &read_count);
    *position += read_count;
    if (result != 1)
        return STATUS_IO_FILE_READ_ERROR;

    OpType optype = getOpType(buffer);
    if (optype != OP_NONE) {
        node->type = NODE_OP;
        node->value.op = optype;
        return STATUS_OK;
    }

    char* endptr = NULL;
    double value = strtod(buffer, &endptr);
    assert(endptr);
    OperationStatus status = STATUS_OK;
    if (*endptr == '\0') {
        node->type = NODE_NUM;
        node->value.num_val = value;
        status = STATUS_OK;
    } else if (endptr == buffer) {
        node->type = NODE_VAR;
        status = addVariable(diff, &node->value.var_idx, buffer);
    } else {
        status = STATUS_PARSER_INVALID_IDENTIFIER;
    }

    return status;
}


static inline void skipWhitespaces(char* src_code, int* position)
{
    assert(src_code); assert(position);

    while (src_code[*position] == ' '  || src_code[*position] == '\t' ||
           src_code[*position] == '\n' || src_code[*position] == '\r')
        (*position)++;
}


static OperationStatus parseNode(TreeNode** node, Differentiator* diff,
                                 char* src_code, int* position)
{
    assert(node); ; assert(diff); assert(diff->var_table.variables); 
    assert(src_code); assert(position);

    OperationStatus status = createNode(node);
    RETURN_IF_STATUS_NOT_OK(status);
    (*position)++;

    skipWhitespaces(src_code, position);

    status = readTitle(*node, diff, src_code, position);
    RETURN_IF_STATUS_NOT_OK(status);

    skipWhitespaces(src_code, position);
    status = readNode(&(*node)->left, diff, src_code, position);
    RETURN_IF_STATUS_NOT_OK(status);
    if ((*node)->left != NULL)
        (*node)->left->parent = *node;

    skipWhitespaces(src_code, position);
    status = readNode(&(*node)->right, diff, src_code, position);
    RETURN_IF_STATUS_NOT_OK(status);
    if ((*node)->right != NULL)
        (*node)->right->parent = *node;

    skipWhitespaces(src_code, position);
    (*position)++;

    return STATUS_OK;
}


OperationStatus readNode(TreeNode** node, Differentiator* diff,
                         char* src_code, int* position)
{
    assert(node); ; assert(diff); assert(diff->var_table.variables); 
    assert(src_code); assert(position);

    skipWhitespaces(src_code, position);

    if (src_code[*position] == '(') {
        return parseNode(node, diff, src_code, position);
    } else if (strncmp(&src_code[*position], "nil", 3) == 0) {
        (*position) += 3;
        *node = NULL;
        return STATUS_OK;
    }
 
    return STATUS_IO_FILE_READ_ERROR;
}


OperationStatus parseArgs(Differentiator* diff, const int argc, const char** argv)
{
    assert(diff); assert(argv);

    diff->args.input_file = "../data/test1";
    diff->args.output_file = NULL;
    diff->args.simple_graph = false;
    diff->args.derivative_order = 1;
    diff->args.infix_input = false;
    diff->args.compute_derivative = false;
    diff->args.taylor_center = 0;
    diff->args.taylor_decomposition = false;

    for (int index = 1; index < argc; index++) {
        if (strcmp(argv[index], "-i") == 0) {
            if (index + 1 < argc && argv[index + 1][0] != '-') {
                diff->args.input_file = argv[index + 1];
                index++;
            } else {
                return STATUS_CLI_UNKNOWN_OPTION;
            }
        }
        else if (strcmp(argv[index], "-o") == 0) {
            if (index + 1 < argc && argv[index + 1][0] != '-') {
                diff->args.output_file = argv[index + 1];
                index++;
            } else {
                return STATUS_CLI_UNKNOWN_OPTION;
            }
        } else if (strcmp(argv[index], "-n") == 0) {
            if (index + 1 < argc && argv[index + 1][0] != '-') {
                char* end = NULL;
                diff->args.derivative_order = strtoull(argv[index + 1], &end, 10);
                if (*end != '\0')
                    return STATUS_CLI_UNKNOWN_OPTION;
                
                index++;
            } else
                return STATUS_CLI_UNKNOWN_OPTION;
        } else if (strcmp(argv[index], "-t") == 0) {
            if (index + 1 < argc && argv[index + 1][0] != '-') {
                char* end = NULL;
                diff->args.taylor_center = strtod(argv[index + 1], &end);
                diff->args.taylor_decomposition = true;
                if (*end != '\0')
                    return STATUS_CLI_UNKNOWN_OPTION;
                
                index++;
            } else
                return STATUS_CLI_UNKNOWN_OPTION;
        } else if (strcmp(argv[index], "-s") == 0) {
            diff->args.simple_graph = true;
        } else if (strcmp(argv[index], "-infix") == 0) {
            diff->args.infix_input = true;
        } else if (strcmp(argv[index], "-c") == 0) {
            diff->args.compute_derivative = true;
        } else {
            return STATUS_CLI_UNKNOWN_OPTION;
        }
    }

    return STATUS_OK;
}