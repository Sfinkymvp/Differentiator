#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_create.h"
#include "diff/diff_defs.h"


TreeNode* createOp(OpType op, TreeNode* left, TreeNode* right)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (node == NULL)
        return NULL;

    node->type = NODE_OP;
    node->value.op = op;
    node->left = left;
    node->right = right;
    
    if (left) left->parent = node;
    if (right) right->parent = node;

    return node;
}


TreeNode* createVar(size_t var_idx)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (node == NULL)
        return NULL;

    node->type = NODE_VAR;
    node->value.var_idx = var_idx;
    return node;
}


TreeNode* createNum(double value) 
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (node == NULL)
        return NULL;

    node->type = NODE_NUM;
    node->value.num_val = value;
    return node;
}
