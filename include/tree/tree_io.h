#ifndef TREE_IO_H_
#define TREE_IO_H_


#include "diff/diff_defs.h"
#include "status.h"


OperationStatus diffLoadExpression(Differentiator* diff);


OperationStatus treeInfixLoad(Differentiator* diff, size_t tree_idx, FILE* input_file);


OperationStatus treePrefixLoad(Differentiator* diff, size_t tree_idx, FILE* input_file);


OperationStatus readTitle(TreeNode* node, Differentiator* diff,
                          char* src_code, int* position);


OperationStatus readNode(TreeNode** node, Differentiator* diff,
                         char* src_code, int* position);


OperationStatus parseArgs(Differentiator* diff, const int argc, const char** argv);


#endif // TREE_IO_H_
