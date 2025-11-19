#ifndef TREE_IO_H_
#define TREE_IO_H_


#include "diff/diff_defs.h"
#include "status.h"


OperationStatus treeLoadExpression(Differentiator* diff, BinaryTree* tree,
                                   FILE* input_file);


OperationStatus readTitle(TreeNode* node, Differentiator* diff,
                          BinaryTree* tree, int* position);


OperationStatus readNode(TreeNode** node, Differentiator* diff,
                                BinaryTree* tree, int* position);



OperationStatus parseArgs(Differentiator* diff, const int argc, const char** argv);


#endif // TREE_IO_H_
