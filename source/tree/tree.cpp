#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "akinator/tree.h"
#include "akinator/colors.h"


const char* DEFAULT_INPUT_FILE = "../data/btree";
const char* DEFAULT_OUTPUT_FILE = "../data/tree";


const char* status_messages[] = {
    GENERATE_STATUS_MESSAGE(TREE_OK, "Tree is valid"),
    GENERATE_STATUS_MESSAGE(TREE_ROOT_HAS_PARENT, "Root node has parent (must be NULL)"),
    GENERATE_STATUS_MESSAGE(TREE_MISSING_PARENT, "Node has no parent (must have)"),
    GENERATE_STATUS_MESSAGE(TREE_PARENT_CHILD_MISMATCH, "Parent-child link mismatch"),
    GENERATE_STATUS_MESSAGE(TREE_INVALID_BRANCH_STRUCTURE, "Node has one child (must be 0 or 2)"),
    GENERATE_STATUS_MESSAGE(TREE_OUT_OF_MEMORY, "Failed to allocate memory"),
};


static void openDumpFile(BinaryTree* tree)
{
    assert(tree);

    static int dump_counter = 1; 

    snprintf(tree->debug.dump.directory, BUFFER_SIZE, "%s/tree_dump_%03d",
             DUMP_DIRECTORY, dump_counter);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm -rf %s && mkdir -p %s",
             tree->debug.dump.directory, tree->debug.dump.directory);
    system(command);

    char filename[BUFFER_SIZE * 2] = {};
    snprintf(filename, BUFFER_SIZE * 2, "%s/tree_dump_%03d.html",
             tree->debug.dump.directory, dump_counter);

    tree->debug.dump.file = fopen(filename, "w");
    assert(tree->debug.dump.file);

    dump_counter++;
}


static TreeStatus nodeVerify(BinaryTree* tree, Node* node)
{
    assert(node);

    if (node->data == NULL)
        return TREE_NULL_DATA_POINTER;
    if (node->parent == NULL)
        return TREE_MISSING_PARENT;
    if (node->parent->left != node && node->parent->right != node)
        return TREE_PARENT_CHILD_MISMATCH;

    TreeStatus status = TREE_OK;
    if (node->left == NULL && node->right == NULL) {
        return TREE_OK;
    } else if (node->left != NULL && node->right != NULL) {
        status = nodeVerify(tree, node->left);
        if (status != TREE_OK)
            return status;
        status = nodeVerify(tree, node->right);
        if (status != TREE_OK)
            return status;
        return TREE_OK;
    }
    
    return TREE_INVALID_BRANCH_STRUCTURE;
}


TreeStatus treeVerify(BinaryTree* tree)
{
    assert(tree); assert(tree->root);
    assert(tree->args.input_file); assert(tree->args.output_file);

    TreeStatus status = TREE_OK;
    if (tree->root->parent != NULL)
        return TREE_ROOT_HAS_PARENT;
    if (tree->root->data == NULL)
        return TREE_NULL_DATA_POINTER;
    if ((tree->root->left == NULL) ^ (tree->root->right == NULL))
        return TREE_INVALID_BRANCH_STRUCTURE;

    if (tree->root->left) {
        status = nodeVerify(tree, tree->root->left);
        if (status != TREE_OK)
            return status;
    }
    if (tree->root->right) {
        status = nodeVerify(tree, tree->root->right);
        if (status != TREE_OK)
            return status;
    }

    return TREE_OK;
}


TreeStatus readUserAnswer(char* buffer, int size)
{
    assert(buffer);

    printf(WHITE_FG);
    char* result = fgets(buffer, size, stdin);
    printf(DEFAULT_FG);
    if (result == NULL)
        return TREE_INPUT_READ_ERROR;

    result = strchr(buffer, '\n');
    if (result != NULL) {
        *result = '\0';
    } else {
        int c = 0;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }

    return TREE_OK;
}


TreeStatus createNode(Node** node)
{
    assert(node);

    *node = (Node*)calloc(1, sizeof(Node));
    if (*node == NULL)
        return TREE_OUT_OF_MEMORY;
    (*node)->is_dynamic = true;

    return TREE_OK;
}


static TreeStatus akinatorAddElement(BinaryTree* tree, Node* node, char* new_data, char* different_data)
{
    assert(tree); assert(node); assert(new_data); assert(different_data);

    TREE_VERIFY(tree, "Add new object");

    TreeStatus status = createNode(&node->left);
    RETURN_IF_NOT_OK(status);
    node->left->data = strdup(new_data);
    if (node->left->data == NULL)
        return TREE_OUT_OF_MEMORY;

    //TREE_DUMP(tree, TREE_OK, "After adding new object");

    status = createNode(&node->right);
    RETURN_IF_NOT_OK(status);
    node->right->data = node->data;
    node->right->is_dynamic = node->is_dynamic;

    //TREE_DUMP(tree, TREE_OK, "After moving old object");

    node->data = strdup(different_data);
    node->is_dynamic = true;
    if (node->data == NULL)
        return TREE_OUT_OF_MEMORY;

    TREE_DUMP(tree, TREE_OK, "After Adding new object");
#ifdef DEBUG
    node->left->parent = node;
    node->right->parent = node;
#endif // DEBUG

    return TREE_OK;
}


static TreeStatus akinatorProcessWrongGuess(BinaryTree* tree, Node* node)
{
    assert(tree); assert(node);

    TREE_VERIFY(tree, "Before processing a new object");
    char answer_buffer[BUFFER_SIZE] = {};
    char difference_buffer[BUFFER_SIZE] = {};

    printf("What is this?\n");
    TreeStatus status = readUserAnswer(answer_buffer, BUFFER_SIZE);
    RETURN_IF_NOT_OK(status);

    if (strcmp(node->data, UNKNOWN_STRING) == 0) {
        if (node->is_dynamic)
            free(node->data);
        node->data = strdup(answer_buffer);
        node->is_dynamic = true;
        if (node->data == NULL)
            return TREE_OUT_OF_MEMORY;

        return TREE_OK;
    }        

    printf("How is '" WHITE("%s") "' different from '" WHITE("%s") "'?\n", answer_buffer, node->data);
    status = readUserAnswer(difference_buffer, BUFFER_SIZE);
    RETURN_IF_NOT_OK(status);

    status = akinatorAddElement(tree, node, answer_buffer, difference_buffer);
    RETURN_IF_NOT_OK(status);
    
    return TREE_OK;
}


static TreeStatus akinatorProcessGuessing(BinaryTree* tree, Node* node)
{
    assert(tree); assert(node); assert(node->data);

    if (node->left == NULL && node->right == NULL)
        printf("Maybe this is %s?\n", node->data);
    else
        printf("Is this %s?\n", node->data);

    char answer_buffer[BUFFER_SIZE] = {};
    TreeStatus status = readUserAnswer(answer_buffer, BUFFER_SIZE);
    RETURN_IF_NOT_OK(status);
    
    if (strcmp(answer_buffer, "yes") == 0) {
        if (node->left == NULL) {
            printf(GREEN("Akinator guessed right!\n"));
            return TREE_OK;
        } else {
            return akinatorProcessGuessing(tree, node->left);
        }
    } else {
        if (node->right == NULL) {
            return akinatorProcessWrongGuess(tree, node);
        } else {
            return akinatorProcessGuessing(tree, node->right);
        }
    }
}


TreeStatus akinatorGuess(BinaryTree* tree)
{
    assert(tree);

    TreeStatus status = TREE_OK;
    if (tree->root == NULL) {
        status = createNode(&tree->root);
        RETURN_IF_NOT_OK(status);
        
        tree->root->parent = NULL;   
        tree->root->data = strdup(UNKNOWN_STRING);
        if (tree->root->data == NULL)
            return TREE_OUT_OF_MEMORY;
    }

    TREE_VERIFY(tree, "Start the akinator");

    status = akinatorProcessGuessing(tree, tree->root);
    return status;
}


TreeStatus treeConstructor(BinaryTree* tree, const int argc, const char** argv)
{
    assert(tree != NULL); assert(argv);

    tree->root = NULL;
    tree->args.input_file = DEFAULT_INPUT_FILE;
    tree->args.output_file = DEFAULT_OUTPUT_FILE;

    TreeStatus status = parseArgs(tree, argc, argv);
    RETURN_IF_NOT_OK(status);

    status = treeLoadFromDisk(tree);
    RETURN_IF_NOT_OK(status);

#ifdef DEBUG
    tree->debug.dump.image_counter = 1;
    openDumpFile(tree);
    assert(tree->debug.dump.file);
#endif // DEBUG

    return TREE_OK;
}


static void deleteBranch(Node* node)
{
    assert(node); assert(node->data);

    if (node->left) {
        deleteBranch(node->left);
        node->left = NULL;
    }
    if (node->right) {
        deleteBranch(node->right);
        node->right = NULL;
    }
    if (node->is_dynamic)
        free(node->data);
    free(node);
}


void treeDestructor(BinaryTree* tree)
{
    assert(tree);
    assert(tree->debug.dump.file);

    fclose(tree->debug.dump.file);
    tree->debug.dump.file = NULL;

    if (tree->root == NULL)
        return;
    free(tree->buffer); 
    deleteBranch(tree->root);
    tree->root = NULL;
}
