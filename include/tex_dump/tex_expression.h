#ifndef TEX_EXPRESSION_H_
#define TEX_EXPRESSION_H_


#include "diff/diff_defs.h"


void printExpression(Differentiator* diff, size_t tree_idx);


void printTex(Differentiator* diff, const char* format, ...);


#endif // TEX_EXPRESSION_H_