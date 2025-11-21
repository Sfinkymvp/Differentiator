#ifndef DIFF_H_
#define DIFF_H_


#include "diff/diff_defs.h"
#include "status.h"


OperationStatus diffTree(Differentiator* diff, size_t diff_var);


double diffEvaluate(Differentiator* diff, const TreeNode* node);


double diffOp(Differentiator* diff, const TreeNode* node);


void diffCalculateValue(Differentiator* diff);


OperationStatus diffConstructor(Differentiator* diff, const int argc, const char** argv);


void diffDestructor(Differentiator* diff);


#endif // DIFF_H_