#ifndef DIFF_EVALUATE_H_
#define DIFF_EVALUATE_H_


#include "diff/diff_defs.h"


double evaluateNode(Differentiator* diff, const TreeNode* node);


void diffEvaluate(Differentiator* diff, size_t tree_idx);


#endif // DIFF_EVALUATE_H_