#ifndef DIFF_H_
#define DIFF_H_


#include "diff/diff_defs.h"
#include "status.h"


OperationStatus diffCalculateDerivative(Differentiator* diff, size_t var_idx);


double diffOp(Differentiator* diff, const TreeNode* node);


double diffEvaluate(Differentiator* diff, const TreeNode* node);


void diffCalculateValue(Differentiator* diff, size_t tree_idx);


void diffTaylorSeries(Differentiator* diff);


OperationStatus defineVariables(Differentiator* diff);


OperationStatus diffConstructor(Differentiator* diff, const int argc, const char** argv);


void diffDestructor(Differentiator* diff);


#endif // DIFF_H_