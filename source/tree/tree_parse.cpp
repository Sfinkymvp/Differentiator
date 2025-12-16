#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "tree/tree_parse.h"
#include "tree/tree.h"

#include "diff/diff_defs.h"
#include "diff/diff_var_table.h"


static TreeNode* getBinaryFunction(Differentiator* diff, char** buffer, OpInfo op_info);
static TreeNode* getUnaryFunction(Differentiator* diff, char** buffer, OpInfo op_info);

static TreeNode* createOperator(OpType op, TreeNode* left, TreeNode* right);
static TreeNode* createVariable(size_t var_idx);
static TreeNode* createNumber(double value);
static bool isReservedFunctionName(char* str, size_t len);

static void skipWhitespaces(char** buffer);


TreeNode* getTree(Differentiator* diff, char** buffer)
{
    assert(diff); assert(buffer); assert(*buffer);
   
    skipWhitespaces(buffer);
    TreeNode* node = getExpression(diff, buffer);
    skipWhitespaces(buffer);
    if (**buffer != '\n' && **buffer != '\0') {
        fprintf(stderr, "Syntax error in getTree!\n");
        if (node) {
            deleteBranch(node);
        }
        return NULL;
    }

    (*buffer)++;
    return node;
}


TreeNode* getExpression(Differentiator* diff, char** buffer)
{
    assert(diff); assert(buffer); assert(*buffer);

    TreeNode* node_1= getTerm(diff, buffer);
    skipWhitespaces(buffer);

    while (**buffer == '+' || **buffer == '-') {
        char op = **buffer;
        (*buffer)++;

        skipWhitespaces(buffer);
        TreeNode* node_2 = getTerm(diff, buffer);

        OpType op_type = OP_NONE;
        if (op == '+') {
            op_type = OP_ADD;
        } else if (op == '-') {
            op_type = OP_SUB;
        } else {
            deleteBranch(node_1);
            deleteBranch(node_2);
            return NULL;
        }

        TreeNode* op_node = createOperator(op_type, node_1, node_2);
        if (op_node == NULL) {
            deleteBranch(node_1);
            deleteBranch(node_2);
            return NULL;
        }
        node_1 = op_node;

        skipWhitespaces(buffer);
    }

    return node_1;
}


TreeNode* getTerm(Differentiator* diff, char** buffer) 
{
    assert(diff); assert(buffer); assert(*buffer);

    TreeNode* node_1 = getPower(diff, buffer);
    skipWhitespaces(buffer);

    while (**buffer == '*' || **buffer == '/') {
        char op = **buffer;
        (*buffer)++;

        skipWhitespaces(buffer);
        TreeNode* node_2 = getPower(diff, buffer);

        OpType op_type = OP_NONE;
        if (op == '*') {
            op_type = OP_MUL;
        } else if (op == '/') {
            op_type = OP_DIV;
        } else {
            deleteBranch(node_1);
            deleteBranch(node_2);
            return NULL;
        }

        TreeNode* op_node = createOperator(op_type, node_1, node_2);
        if (op_node == NULL) {
            deleteBranch(node_1);
            deleteBranch(node_2);
            return NULL;
        }
        node_1 = op_node;

        skipWhitespaces(buffer);
    }

    return node_1;
}


TreeNode* getPower(Differentiator* diff, char** buffer)
{
    assert(diff); assert(buffer); assert(*buffer);

    TreeNode* node_1 = getPrimary(diff, buffer);
    skipWhitespaces(buffer);

    while (**buffer == '^') {
        (*buffer)++;

        skipWhitespaces(buffer);
        TreeNode* node_2 = getPrimary(diff, buffer);

        TreeNode* op_node = createOperator(OP_POW, node_1, node_2);
        if (op_node == NULL) { deleteBranch(node_1);
            deleteBranch(node_2);
            return NULL;
        }
        node_1 = op_node;

        skipWhitespaces(buffer);
    }

    return node_1;
}


TreeNode* getPrimary(Differentiator* diff, char** buffer) 
{
    assert(diff); assert(buffer); assert(*buffer);

    if (**buffer == '(') {
        (*buffer)++;

        skipWhitespaces(buffer);
        TreeNode* node = getExpression(diff, buffer);
        skipWhitespaces(buffer);

        (*buffer)++;
        return node;
    }
   
    TreeNode* node = NULL;
    if (isdigit(**buffer)) {
        node = getNumber(diff, buffer);
        if (node != NULL) return node;
    }

    node = getVariable(diff, buffer);    
    if (node != NULL) return node;

    node = getFunction(diff, buffer);
    if (node != NULL) return node;

    return NULL;
}


TreeNode* getVariable(Differentiator* diff, char** buffer)
{
    assert(diff); assert(diff->var_table.variables); assert(buffer); assert(*buffer);

    //printf("%s\n", *buffer);
    if (isalpha(**buffer)) {
        char* variable_end = *buffer;
        while (isalpha(*variable_end) || isdigit(*variable_end) || *variable_end == '_')
            variable_end++;
      
        size_t len = (size_t)(variable_end - *buffer);
        if (isReservedFunctionName(*buffer, len)) return NULL;
        if (*variable_end == '(') return NULL;

        size_t var_idx = 0;
        if (addVariable(diff, &var_idx, *buffer, len) != STATUS_OK)
            return NULL;

        TreeNode* node = createVariable(var_idx);
        if (node == NULL) return NULL;

        *buffer = variable_end;
        return node;
    } else {
        return NULL;
    }
}


TreeNode* getFunction(Differentiator* diff, char** buffer)
{
    assert(diff); assert(buffer); assert(*buffer);

    if (isalpha(**buffer)) {
        char* function_end = *buffer;
        while (isalpha(*function_end) || isdigit(*function_end) || *function_end == '_')
            function_end++;
        
        size_t len = (size_t)(function_end - *buffer);
        bool is_function = false;
        size_t index = 0;

        for (; index < OP_TABLE_COUNT; index++) {
            const char* existing_name = OP_TABLE[index].symbol;
            if (strncmp(*buffer, existing_name, len) == 0 && existing_name[len] == '\0') {
                is_function = true;
                break;
            }
        }
        if (!is_function) return NULL;
        *buffer += len;

        if (OP_TABLE[index].op == OP_LOG) {
            return getBinaryFunction(diff, buffer, OP_TABLE[index]);
        } else {
            return getUnaryFunction(diff, buffer, OP_TABLE[index]);
        }
    } else {
        return NULL;
    }
}


static TreeNode* getUnaryFunction(Differentiator* diff, char** buffer, OpInfo op_info)
{
    assert(diff); assert(buffer); assert(*buffer);

    skipWhitespaces(buffer);

    if (**buffer != '(') return NULL;
    (*buffer)++;

    skipWhitespaces(buffer);

    TreeNode* expression = getExpression(diff, buffer);
    if (expression == NULL) return NULL;

    skipWhitespaces(buffer);

    if (**buffer != ')') {
        deleteBranch(expression);
        return NULL;
    }
    (*buffer)++;

    TreeNode* node = createOperator(op_info.op, NULL, expression);
    if (node == NULL) {
        deleteBranch(expression);
        return NULL;
    }

    return node;
}


static TreeNode* getBinaryFunction(Differentiator* diff, char** buffer, OpInfo op_info)
{
    assert(diff); assert(buffer); assert(*buffer);

    skipWhitespaces(buffer);

    if (**buffer != '(') return NULL;
    (*buffer)++;

    skipWhitespaces(buffer);

    TreeNode* left_expression = getExpression(diff, buffer);
    if (left_expression == NULL) return NULL;

    skipWhitespaces(buffer);

    if (**buffer != ',') {
        deleteBranch(left_expression);
        return NULL;
    }
    (*buffer)++;

    skipWhitespaces(buffer);

    TreeNode* right_expression = getExpression(diff, buffer);
    if (right_expression == NULL) return NULL;
    if (**buffer != ')') {
        deleteBranch(right_expression);
        deleteBranch(left_expression);
        return NULL;
    }
    (*buffer)++;

    skipWhitespaces(buffer);

    TreeNode* node = createOperator(op_info.op, left_expression, right_expression);
    if (node == NULL) {
        deleteBranch(left_expression);
        deleteBranch(right_expression);
        return NULL;
    }

    return node;
}


TreeNode* getNumber(Differentiator* diff, char** buffer)
{
    assert(diff); assert(buffer); assert(*buffer);

    int value = 0;
    int iteration_count = 0;
    while (isdigit(**buffer)) {
        value = value * 10 + (**buffer - '0');
        (*buffer)++;
        iteration_count++;
    }

    if (iteration_count == 0) {
        fprintf(stderr, "Syntax error in getNumber!\n");
        return NULL;
    }

    TreeNode* node = createNumber(value);
    if (node == NULL) return NULL;

    return node;
}


static TreeNode* createOperator(OpType op, TreeNode* left, TreeNode* right)
{
    TreeNode* node = NULL;
    createNode(&node);
    if (node == NULL) return NULL;

    node->type = NODE_OP;
    node->value.op = op;
    node->left = left;
    node->right = right;

    if (node->left)  node->left->parent  = node;
    if (node->right) node->right->parent = node;

    return node;
}


static TreeNode* createVariable(size_t var_idx)
{
    TreeNode* node = NULL;
    createNode(&node);
    if (node == NULL) return NULL;

    node->type = NODE_VAR;
    node->value.var_idx = var_idx;
    node->left = NULL;
    node->right = NULL;

    return node;
}


static TreeNode* createNumber(double value)
{
    TreeNode* node = NULL;
    createNode(&node);
    if (node == NULL) return NULL;

    node->type = NODE_NUM;
    node->value.num_val = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}


static bool isReservedFunctionName(char* str, size_t len)
{
    assert(str); 

    for (size_t index = 0; index < OP_TABLE_COUNT; index++) {
        const char* existing_name = OP_TABLE[index].symbol;
        if (strncmp(str, existing_name, len) == 0 && existing_name[len] == '\0')
            return true;
    }

    return false;
}


static void skipWhitespaces(char** buffer)
{
    assert(buffer); assert(*buffer);

    while (**buffer == '\t' || **buffer == ' ') {
        (*buffer)++;
    }
}