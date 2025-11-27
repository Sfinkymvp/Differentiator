#ifndef TREE_PARSE_H_
#define TREE_PARSE_H_


#include "diff/diff_defs.h"


TreeNode* getTree(char** buffer);


TreeNode* getExpression(char** buffer);


TreeNode* getTerm(char** buffer);


TreeNode* getParentheticalExpression(char** buffer) ;


TreeNode* getNumber(char** buffer);


#endif // TREE_PARSE_H_
