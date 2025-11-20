#ifndef TREE_H_
#define TREE_H_


#include "diff/diff_defs.h"
#include "graph_dump/html_builder.h"
#include "status.h"


#define TREE_CREATE(name, identifier)            \
    treeConstructor(name, identifier, #name, __FILE__, __func__, __LINE__)


#define TREE_DUMP(diff, tree, _status, format, ...)   \
    treeDump(diff ,tree, _status, __FILE__, __func__, __LINE__, format, ##__VA_ARGS__)


#define TREE_VERIFY(diff, tree, format, ...)                       \
    do {                                                     \
        OperationStatus _status = treeVerify(tree);               \
        TREE_DUMP(diff, tree, _status, format, ##__VA_ARGS__);     \
        if (_status != STATUS_OK) {                            \
            return _status;                                  \
        }                                                    \
    } while (0)


#define GENERATE_STATUS_MESSAGE(_status, message)      \
    ("[" #_status "] " message)


OperationStatus treeVerify(BinaryTree* tree);


OperationStatus createNode(TreeNode** node);


OperationStatus treeConstructor(BinaryTree* tree, const char* identifier, const char* name,
                                const char* file, const char* function, int line);


void treeDestructor(BinaryTree* tree);


#endif // TREE_H_
