#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tree/tree_parse.h"
#include "tree/tree.h"

#include "diff/diff_defs.h"


TreeNode* getTree(char** buffer)
{
    assert(buffer); assert(*buffer);
    
    TreeNode* node = getExpression(buffer);
    if (**buffer != '\0') {
        fprintf(stderr, "Syntax error in getTree!\n");
        deleteBranch(node);
        return NULL;
    }

    (*buffer)++;
    return node;
}


TreeNode* getExpression(char** buffer)
{
    assert(buffer); assert(*buffer);

    TreeNode* node_1= getTerm(buffer);
    while (**buffer == '+' || **buffer == '-') {
        char op = **buffer;
        (*buffer)++;
        TreeNode* node_2 = getTerm(buffer);

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
            assert(0 && "ti lox");
        }

        node_1 = op_node;
    }

    return node_1;
}


TreeNode* getTerm(char** buffer) 
{
    assert(buffer); assert(*buffer);

    TreeNode* node_1 = getParentheticalExpression(buffer);
    while (**buffer == '*' || **buffer == '/') {
        char op = **buffer;
        (*buffer)++;
        TreeNode* node_2 = getParentheticalExpression(buffer);

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


TreeNode* getParentheticalExpression(char** buffer) 
{
    assert(buffer); assert(*buffer);

    if (**buffer == '(') {
        (*buffer)++;
        TreeNode* node = getExpression(buffer);
        (*buffer)++;

        return node;
    } else {
        return getNumber(buffer);
    }
}


TreeNode* getNumber(char** buffer)
{
    assert(buffer); assert(*buffer);

    int value = 0;
    int iteration_count = 0;
    while ('0' <= **buffer && **buffer <= '9') {
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