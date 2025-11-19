#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>


#include "akinator/tree.h"


static void printTree(Node* node, FILE* output_file)
{
    assert(node);
    assert(output_file);

    fprintf(output_file, "(");

    fprintf(output_file, "\"%s\"", node->data);
    if (node->left)
        printTree(node->left, output_file);
    else
        fprintf(output_file, " nil");
    if (node->right)
        printTree(node->right, output_file);
    else
        fprintf(output_file, " nil");
        
    fprintf(output_file, ")");
}


TreeStatus treeWriteToDisk(BinaryTree* tree)
{
    TREE_VERIFY(tree, "Before write tree to disk");

    FILE* output_file = fopen(tree->args.output_file, "w");
    if (output_file == NULL)
        return TREE_OUTPUT_FILE_OPEN_ERROR;

    printTree(tree->root, output_file);

    if (fclose(output_file) != 0)
        return TREE_OUTPUT_FILE_CLOSE_ERROR;

    return TREE_OK;
}


static size_t getFileSize(const char* input_filename)
{
    assert(input_filename != NULL);

    struct stat buf = {};

    if (stat(input_filename, &buf) == -1)
        return 0;

    return (size_t)buf.st_size;
}


static TreeStatus createInputFile(BinaryTree* tree)
{
    assert(tree); assert(tree->args.input_file);
           
    FILE* input_file = fopen(tree->args.input_file, "w");
    if (input_file != NULL) {
        fclose(input_file);
        return TREE_OK;
    }

    return TREE_INPUT_FILE_OPEN_ERROR;
}


TreeStatus treeLoadFromDisk(BinaryTree* tree)
{
    assert(tree); assert(tree->args.input_file);
    
    FILE* input_file = fopen(tree->args.input_file, "r");
    if (input_file == NULL) {
        if (errno == ENOENT)
            return createInputFile(tree);
        return TREE_INPUT_FILE_OPEN_ERROR; 
    }

    size_t file_size = getFileSize(tree->args.input_file); 
    if (file_size == 0)
        return TREE_OK;

    tree->buffer = (char*)calloc(file_size + 1, 1);
    if (tree->buffer == NULL) {
        fclose(input_file);
        return TREE_OUT_OF_MEMORY;
    }

    size_t read_size = fread(tree->buffer, 1, file_size, input_file);
    if (read_size != file_size) {
        fclose(input_file);
        free(tree->buffer);
        return TREE_INPUT_FILE_READ_ERROR;
    }

    printf(WHITE("BUFFER DUMP (load tree from disk)\n"));
    int position = 0;    
    TreeStatus status = readNode(tree, &tree->root, &position);
    if (status != TREE_OK) {
        fclose(input_file);
        free(tree->buffer);
        return status;
    }

    if (fclose(input_file) != 0) {
        free(tree->buffer);
        return TREE_INPUT_FILE_CLOSE_ERROR;
    }

    printSeparator();
    printf(GREEN("Tree loaded successfully\n\n"));
    return TREE_OK;
}


static inline void skipWhitespaces(char* buffer, int* position)
{
    assert(buffer), assert(position);

    while (buffer[*position] == ' '  || buffer[*position] == '\t' ||
           buffer[*position] == '\n' || buffer[*position] == '\r')
        (*position)++;
}


static TreeStatus readTitle(char** destination, char* buffer, int* position)
{
    assert(destination); assert(buffer); assert(position);

    int read_len = 0;
    int result = sscanf(buffer + *position, "\"%*[^\"]\"%n", &read_len);
    if (result != 0)
        return TREE_INPUT_FILE_READ_ERROR;

    buffer[*position + read_len - 1] = '\0';
    *destination = buffer + *position + 1;
    *position += read_len;

    return TREE_OK;
}


static TreeStatus parseNode(BinaryTree* tree, Node** node, int* position)
{
    assert(tree); assert(tree->buffer); assert(node); assert(position);
    assert(*node == NULL);
  
    TreeStatus status = createNode(node);
    RETURN_IF_NOT_OK(status);
    (*position)++;

    skipWhitespaces(tree->buffer, position);

    status = readTitle(&(*node)->data, tree->buffer, position);
    (*node)->is_dynamic = false;
    RETURN_IF_NOT_OK(status);

    skipWhitespaces(tree->buffer, position);
    status = readNode(tree, &(*node)->left, position);
    RETURN_IF_NOT_OK(status);
    if ((*node)->left != NULL)
        (*node)->left->parent = *node;

    skipWhitespaces(tree->buffer, position);
    status = readNode(tree, &(*node)->right, position);
    RETURN_IF_NOT_OK(status);
    if ((*node)->right != NULL)
        (*node)->right->parent = *node;

    skipWhitespaces(tree->buffer, position);
    (*position)++;

    return TREE_OK;
}


TreeStatus readNode(BinaryTree* tree, Node** node, int* position)
{
    assert(tree); assert(tree->buffer); assert(node); assert(position);
    assert(*node == NULL);

    skipWhitespaces(tree->buffer, position);

    printSeparator();
    printf("%s\n", tree->buffer + *position);

    if (tree->buffer[*position] == '(') {
        return parseNode(tree, node, position);
    } else if (strncmp(&tree->buffer[*position], "nil", 3) == 0) {
        (*position) += 3;
        *node = NULL;
        return TREE_OK;
    }
 
    return TREE_INPUT_FILE_READ_ERROR;
}


TreeStatus parseArgs(BinaryTree* tree, const int argc, const char** argv)
{
    assert(tree); assert(argv);

    for (int index = 1; index < argc; index++) {
        if (strcmp(argv[index], "-i") == 0) {
            if (index + 1 < argc && argv[index + 1][0] != '-') {
                tree->args.input_file = argv[index + 1];
                index++;
            } else {
                return TREE_UNKNOWN_CMD_ARGUMENTS;
            }
        }
        else if (strcmp(argv[index], "-o") == 0) {
            if (index + 1 < argc && argv[index + 1][0] != '-') {
                tree->args.output_file = argv[index + 1];
                index++;
            } else {
                return TREE_UNKNOWN_CMD_ARGUMENTS;
            }
        } else {
            return TREE_UNKNOWN_CMD_ARGUMENTS;
        }
    }

    return TREE_OK;
}
