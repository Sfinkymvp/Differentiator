#ifndef INCLUDE_TREE_TREE_H_
#define INCLUDE_TREE_TREE_H_


#include "./data.h"
#include "./menu.h"
#include "./io.h"
#include "./comparison.h"


#ifdef DEBUG

#include "../debug/html_builder.h"


#define TREE_INIT(name)                          \
    BinaryTree name = {};                        \
    name.debug.creation = (TreeCreationInfo) {   \
        #name, __FILE__, __func__, __LINE__      \
    }


#define TREE_DUMP(tree, __status, format, ...)   \
    treeDump(tree, __status, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__)


#define TREE_VERIFY(tree, format, ...)                       \
    do {                                                     \
        TreeStatus _status = treeVerify(tree);               \
        TREE_DUMP(tree, _status, format, ##__VA_ARGS__);     \
        if (_status != TREE_OK) {                            \
            return _status;                                  \
        }                                                    \
    } while (0)

#endif // DEBUG


#define GENERATE_STATUS_MESSAGE(_status, message)      \
    ("[" #_status "] " message)


#define RETURN_IF_NOT_OK(_status) if (_status != TREE_OK) return _status


#define RETURN_IF_STACK_ERROR(_status) if (_status != SUCCESS) return TREE_STACK_ERROR


void printStatusMessage(TreeStatus status);


TreeStatus treeVerify(BinaryTree* tree);


TreeStatus readUserAnswer(char* buffer, int size);


TreeStatus createNode(Node** node);


TreeStatus akinatorGuess(BinaryTree* tree);


TreeStatus treeConstructor(BinaryTree* tree, const int argc, const char** argv);


void treeDestructor(BinaryTree* tree);


#endif // INCLUDE_TREE_TREE_H_
