#ifndef DIFF_EVALUATE_H_
#define DIFF_EVALUATE_H_


#include "diff/diff_defs.h"


void diffEvaluate(Differentiator* diff, size_t tree_idx);

double evaluateNode(Differentiator* diff, const TreeNode* node);


#endif // DIFF_EVALUATE_H_