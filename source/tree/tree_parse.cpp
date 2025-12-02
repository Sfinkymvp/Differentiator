#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "tree/tree_parse.h"
#include "tree/tree.h"

#include "diff/diff_defs.h"
#include "diff/diff_var_table.h"


TreeNode* getTree(Differentiator* diff, char** buffer)
{
    assert(diff); assert(buffer); assert(*buffer);
    
    TreeNode* node = getExpression(diff, buffer);
    if (**buffer != '\0') {
        fprintf(stderr, "Syntax error in getTree!\n");
        if (node) deleteBranch(node);
        return NULL;
    }

    (*buffer)++;
    return node;
}


TreeNode* getExpression(Differentiator* diff, char** buffer)
{
    assert(diff); assert(buffer); assert(*buffer);

    TreeNode* node_1= getTerm(diff, buffer);
    while (**buffer == '+' || **buffer == '-') {
        char op = **buffer;
        (*buffer)++;
        TreeNode* node_2 = getTerm(diff, buffer);

        TreeNode* op_node = NULL;
        if (createNode(&op_node) != STATUS_OK) {
            deleteBranch(node_1);
            deleteBranch(node_2);
        }

        op_node->type = NODE_OP;
        op_node->left = node_1;
        if (op_node->left) op_node->left->parent = op_node;
        op_node->right = node_2;
        if (op_node->right) op_node->right->parent = op_node;

        if (op == '+') {
            op_node->value.op = OP_ADD;
        } else if (op == '-') {
            op_node->value.op = OP_SUB;
        } else {
            assert(0);
        }

        node_1 = op_node;
    }

    return node_1;
}


TreeNode* getTerm(Differentiator* diff, char** buffer) 
{
    assert(diff); assert(buffer); assert(*buffer);

    TreeNode* node_1 = getPrimary(diff, buffer);
    while (**buffer == '*' || **buffer == '/') {
        char op = **buffer;
        (*buffer)++;
        TreeNode* node_2 = getPrimary(diff, buffer);

        TreeNode* op_node = NULL;
        if (createNode(&op_node) != STATUS_OK) {
            deleteBranch(node_1);
            deleteBranch(node_2);
            return NULL;
        }

        op_node->type = NODE_OP;
        op_node->left = node_1;
        if (op_node->left) op_node->left->parent = op_node;
        op_node->right = node_2;
        if (op_node->right) op_node->right->parent = op_node;

        if (op == '*') {
            op_node->value.op = OP_MUL;
        } else if (op == '/') {
            op_node->value.op = OP_DIV;
        } else {
            assert(0 && "ti lox");
        }

        node_1 = op_node;
    }

    return node_1;
}


TreeNode* getPrimary(Differentiator* diff, char** buffer) 
{
    assert(diff); assert(buffer); assert(*buffer);

    if (**buffer == '(') {
        (*buffer)++;
        TreeNode* node = getExpression(diff, buffer);
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


static bool isReservedFunctionName(char* str, size_t len)
{
    assert(str); 

    for (size_t index = 0; index < OP_TABLE_COUNT; index++) {
        const char* existing_name = OP_TABLE[index].symbol;
        if (strncmp(str, existing_name, len) == 0 &&
            existing_name[len] == '\0')
            return true;
    }

    return false;
}


TreeNode* getVariable(Differentiator* diff, char** buffer)
{
    assert(diff); assert(diff->var_table.variables); assert(buffer); assert(*buffer);

    printf("%s\n", *buffer);
    if (isalpha(**buffer)) {
        char* variable_end = *buffer;
        while (isalpha(*variable_end) || isdigit(*variable_end) || *variable_end == '_')
            variable_end++;
      
        size_t len = (size_t)(variable_end - *buffer);
        if (isReservedFunctionName(*buffer, len))
            return NULL;
        if (*variable_end == '(')
            return NULL;

        size_t var_idx = 0;
        if (addVariable(diff, &var_idx, *buffer, len) != STATUS_OK)
            return NULL;

        TreeNode* node = NULL;
        createNode(&node);
        if (node == NULL)
            return NULL;

        node->type = NODE_VAR;
        node->value.var_idx = var_idx;
        node->left = NULL;
        node->right = NULL;

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
        if (!is_function)
            return NULL;

        *buffer += len;
        if (**buffer != '(')
            return NULL;

        (*buffer)++;
        TreeNode* expression = getExpression(diff, buffer);
        if (expression == NULL)
            return NULL;
        if (**buffer != ')') {
            deleteBranch(expression);
            return NULL;
        }
        (*buffer)++;
        
        TreeNode* node = NULL;
        createNode(&node);
        if (node == NULL)
            return NULL;
        node->type = NODE_OP;
        node->value.op = OP_TABLE[index].op;
        node->left = NULL;
        node->right = expression;
        expression->parent = node;

        return node;
    } else {
        return NULL;
    }
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

    TreeNode* node = NULL;
    if (createNode(&node) != STATUS_OK)
        return NULL;

    node->type = NODE_NUM;
    node->value.num_val = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}