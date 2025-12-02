#ifndef TREE_PARSE_H_
#define TREE_PARSE_H_


#include "diff/diff_defs.h"


TreeNode* getTree(Differentiator* diff, char** buffer);


TreeNode* getExpression(Differentiator* diff, char** buffer);


TreeNode* getTerm(Differentiator* diff, char** buffer);


TreeNode* getPower(Differentiator* diff, char** buffer);


TreeNode* getPrimary(Differentiator* diff, char** buffer) ;


TreeNode* getVariable(Differentiator* diff, char** buffer);


TreeNode* getFunction(Differentiator* diff, char** buffer);


TreeNode* getNumber(Differentiator* diff, char** buffer);


#endif // TREE_PARSE_H_
