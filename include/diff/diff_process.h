#ifndef DIFF_PROCESS_H_
#define DIFF_PROCESS_H_


#include "diff/diff_defs.h"


TreeNode* createOp(OpType op, TreeNode* left, TreeNode* right);


bool containsVariable(TreeNode* node, size_t var_idx);


TreeNode* diffPow(Differentiator* diff, TreeNode* left, TreeNode* right, size_t var_idx);


TreeNode* diffLog(Differentiator* diff, TreeNode* left, TreeNode* right, size_t var_idx);


TreeNode* diffOp(Differentiator* diff, TreeNode* node, size_t var_idx);


TreeNode* diffNode(Differentiator* diff, TreeNode* node, size_t var_idx);


//TreeNode* createTaylorTree(Differentiator* diff, size_t derivative_number);


#endif // DIFF_PROCESS_H_