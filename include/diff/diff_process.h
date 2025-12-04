#ifndef DIFF_PROCESS_H_
#define DIFF_PROCESS_H_

#include "diff/diff_defs.h"


TreeNode* diffNode(Differentiator* diff, TreeNode* node, size_t var_idx);


bool containsVariable(TreeNode* node, size_t var_idx);


#endif // DIFF_PROCESS_H_