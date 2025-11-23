#ifndef DIFF_PROCESS_H_
#define DIFF_PROCESS_H_


#include "diff/diff_defs.h"


TreeNode* createOp(OpType op, TreeNode* left, TreeNode* right);


bool containsVariable(TreeNode* node, size_t var_idx);


TreeNode* diffPow(TreeNode* left, TreeNode* right, size_t var_idx);


TreeNode* diffLog(TreeNode* left, TreeNode* right, size_t var_idx);


TreeNode* diffNode(TreeNode* node, size_t var_idx);


#endif // DIFF_PROCESS_H_