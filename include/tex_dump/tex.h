#ifndef TEX_BUILDER_H_
#define TEX_BUILDER_H_


#include "diff/diff_defs.h"


void printOperator(Differentiator* diff, TreeNode* node);


void printNode(Differentiator* diff, TreeNode* node);


void printPaintedOperator(Differentiator* diff, TreeNode* node, TreeNode* color_node);


void printPaintedNode(Differentiator* diff, TreeNode* node, TreeNode* color_node);


void printExpression(Differentiator* diff, size_t tree_idx);


void printPlot(Differentiator* diff, size_t tree_idx);


void texInit(Differentiator* diff);


void texClose(Differentiator* diff);


void printTitle(Differentiator* diff);


void printIntroduction(Differentiator* diff);


#endif // TEX_BUILDER_H_