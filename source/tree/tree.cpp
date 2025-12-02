#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "tree/tree.h"
#include "diff/diff_defs.h"
#include "status.h"


#define OP_INFO_ITEM(ENUM, SYMBOL) {ENUM, #ENUM, SYMBOL}


const OpInfo OP_TABLE[] = {
    OP_INFO_ITEM(OP_ADD, "+"),
    OP_INFO_ITEM(OP_SUB, "-"),
    OP_INFO_ITEM(OP_MUL, "*"),
    OP_INFO_ITEM(OP_DIV, "/"),

    OP_INFO_ITEM(OP_POW, "^"),
    OP_INFO_ITEM(OP_LOG, "log"),

    OP_INFO_ITEM(OP_SIN, "sin"),
    OP_INFO_ITEM(OP_COS, "cos"),
    OP_INFO_ITEM(OP_TAN, "tan"),
    OP_INFO_ITEM(OP_COT, "cot"),

    OP_INFO_ITEM(OP_ASIN, "asin"),
    OP_INFO_ITEM(OP_ACOS, "acos"),
    OP_INFO_ITEM(OP_ATAN, "atan"),
    OP_INFO_ITEM(OP_ACOT, "acot"),

    OP_INFO_ITEM(OP_SINH, "sinh"),
    OP_INFO_ITEM(OP_COSH, "cosh"),
    OP_INFO_ITEM(OP_TANH, "tanh"),
    OP_INFO_ITEM(OP_COTH, "coth"),

    OP_INFO_ITEM(OP_ASINH, "asinh"),
    OP_INFO_ITEM(OP_ACOSH, "acosh"),
    OP_INFO_ITEM(OP_ATANH, "atanh"),
    OP_INFO_ITEM(OP_ACOTH, "acoth"),

    OP_INFO_ITEM(OP_NONE, "")
};
const size_t OP_TABLE_COUNT = sizeof(OP_TABLE) / sizeof(*OP_TABLE);


static OperationStatus nodeVerify(TreeNode* node)
{
    assert(node);

    if (node->parent == NULL)
        return STATUS_TREE_MISSING_PARENT;
    if (node->parent->left != node && node->parent->right != node)
        return STATUS_TREE_PARENT_CHILD_MISMATCH;

    OperationStatus status = STATUS_OK;
    if (node->left == NULL && node->right == NULL) {
        return STATUS_OK;
    } else if (node->left != NULL && node->right != NULL) {
        status = nodeVerify(node->left);
        if (status != STATUS_OK)
            return status;
        status = nodeVerify(node->right);
        if (status != STATUS_OK)
            return status;
        return STATUS_OK;
    }
   
    return STATUS_OK;
    //return STATUS_TREE_INVALID_BRANCH_STRUCTURE;
}


OperationStatus treeVerify(BinaryTree* tree)
{
    assert(tree); assert(tree->root);

    OperationStatus status = STATUS_OK;
    if (tree->root->parent != NULL)
        return STATUS_TREE_ROOT_HAS_PARENT;
    if ((tree->root->left == NULL) ^ (tree->root->right == NULL))
        return STATUS_TREE_INVALID_BRANCH_STRUCTURE;

    if (tree->root->left) {
        status = nodeVerify(tree->root->left);
        if (status != STATUS_OK)
            return status;
    }
    if (tree->root->right) {
        status = nodeVerify(tree->root->right);
        if (status != STATUS_OK)
            return status;
    }

    return STATUS_OK;
}


OperationStatus createNode(TreeNode** node)
{
    assert(node);

    *node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (*node == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;

    return STATUS_OK;
}


OperationStatus treeConstructor(BinaryTree* tree, const char* identifier, const char* name,
                                const char* file, const char* function, int line)
{
    assert(tree); assert(identifier); assert(name); assert(file); assert(function);

    tree->root = NULL;
    tree->identifier = strdup(identifier);
    if (tree->identifier == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    tree->origin = (CreationInfo){name, file, function, line};

    return STATUS_OK;
}


void deleteBranch(TreeNode* node)
{
    assert(node);

    if (node->left) {
        deleteBranch(node->left);
        node->left = NULL;
    }
    if (node->right) {
        deleteBranch(node->right);
        node->right = NULL;
    }
    free(node);
}


void treeDestructor(BinaryTree* tree)
{
    assert(tree);

    if (tree->identifier)
        free(tree->identifier); 
    if (tree->root == NULL)
        return;
    deleteBranch(tree->root);
    tree->root = NULL;
}