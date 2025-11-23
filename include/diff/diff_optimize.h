#ifndef DIFF_OPTIMIZE_H_
#define DIFF_OPTIMIZE_H_


#include "diff/diff_defs.h"


bool foldConstants(Differentiator* diff, TreeNode* node);


bool simplifyOperations(Differentiator* diff, TreeNode* node);


void optimizeTree(Differentiator* diff, size_t tree_idx);


#endif // DIFF_OPTIMIZE_H_