#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "tree/tree.h"
#include "diff/diff_defs.h"
#include "status.h"


const char* status_messages[] = {};


void openDumpFile(Differentiator* diff)
{
    assert(diff);

    static int dump_counter = 1; 

    snprintf(diff->dump_state.directory, BUFFER_SIZE, "%s/tree_dump_%03d",
             DUMP_DIRECTORY, dump_counter);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm -rf %s && mkdir -p %s",
             diff->dump_state.directory, diff->dump_state.directory);
    system(command);

    char filename[BUFFER_SIZE * 2] = {};
    snprintf(filename, BUFFER_SIZE * 2, "%s/tree_dump_%03d.html",
             diff->dump_state.directory, dump_counter);

    diff->dump_state.dump_file = fopen(filename, "w");
    assert(diff->dump_state.dump_file);

    dump_counter++;
}


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
    
    return STATUS_TREE_INVALID_BRANCH_STRUCTURE;
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
    tree->identifier = (const char*)strdup(identifier);
    if (tree->identifier == NULL)
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    tree->origin = (CreationInfo){name, file, function, line};

    return STATUS_OK;
}


static void deleteBranch(TreeNode* node)
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
    assert(tree); assert(tree->identifier);

    free((char*)tree->identifier); 
    if (tree->root == NULL)
        return;
    deleteBranch(tree->root);
    tree->root = NULL;
}