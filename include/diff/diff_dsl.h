#ifndef DIFF_DSL_H_
#define DIFF_DSL_H_


#include "diff/diff_defs.h"


TreeNode* createOp(OpType op, TreeNode* left, TreeNode* right);


TreeNode* createNum(double value);


TreeNode* MakeAdd(TreeNode* left, TreeNode* right);


TreeNode* MakeSub(TreeNode* left, TreeNode* right);


TreeNode* MakeMul(TreeNode* left, TreeNode* right);


TreeNode* MakeDiv(TreeNode* left, TreeNode* right);


TreeNode* MakePow(TreeNode* left, TreeNode* right);


TreeNode* MakeSin(TreeNode* arg);


TreeNode* MakeCos(TreeNode* arg);


TreeNode* diffNode(TreeNode* node, size_t diff_var);


#endif // DIFF_DSL_H_