#ifndef DIFF_CREATE_H_
#define DIFF_CREATE_H_


#include "diff/diff_defs.h"


#define CNUM(value) createNum(value)
#define CVAR(var_idx) createVar(var_idx)

#define ADD(L, R) createOp(OP_ADD, L, R)
#define SUB(L, R) createOp(OP_SUB, L, R)
#define MUL(L, R) createOp(OP_MUL, L, R)
#define DIV(L, R) createOp(OP_DIV, L, R)

#define POW(L, R) createOp(OP_POW, L, R)
#define LOG(L, R) createOp(OP_LOG, L, R)

#define SIN(R) createOp(OP_SIN, NULL, R)
#define COS(R) createOp(OP_COS, NULL, R)
#define TAN(R) createOp(OP_TAN, NULL, R)
#define COT(R) createOp(OP_COT, NULL, R)

#define ASIN(R) createOp(OP_ASIN, NULL, R)
#define ACOS(R) createOp(OP_ACOS, NULL, R)
#define ATAN(R) createOp(OP_ATAN, NULL, R)
#define ACOT(R) createOp(OP_ACOT, NULL, R)

#define SINH(R) createOp(OP_SINH, NULL, R)
#define COSH(R) createOp(OP_COSH, NULL, R)
#define TANH(R) createOp(OP_TANH, NULL, R)
#define COTH(R) createOp(OP_COTH, NULL, R)

#define ASINH(R) createOp(OP_ASINH, NULL, R)
#define ACOSH(R) createOp(OP_ACOSH, NULL, R)
#define ATANH(R) createOp(OP_ATANH, NULL, R)
#define ACOTH(R) createOp(OP_ACOTH, NULL, R)


TreeNode* createOp(OpType op, TreeNode* left, TreeNode* right);

TreeNode* createVar(size_t var_idx);

TreeNode* createNum(double value);


#endif // DIFF_CREATE_H_