#ifndef DIFF_TAYLOR_H_
#define DIFF_TAYLOR_H_


#include "diff/diff_defs.h"


void diffTaylorSeries(Differentiator* diff);


TreeNode* createTaylorTree(Differentiator* diff, size_t derivative_counter);


void printTaylorTree(Differentiator* diff, TreeNode* taylor_root);


#endif // DIFF_TAYLOR_H_