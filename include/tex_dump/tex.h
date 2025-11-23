#ifndef TEX_BUILDER_H_
#define TEX_BUILDER_H_


#include "diff/diff_defs.h"


void printOperator(Differentiator* diff, TreeNode* node);


void printNode(Differentiator* diff, TreeNode* node);


void printExpression(Differentiator* diff, TreeNode* node);


void texInit(Differentiator* diff);


void texClose(Differentiator* diff);


#endif // TEX_BUILDER_H_