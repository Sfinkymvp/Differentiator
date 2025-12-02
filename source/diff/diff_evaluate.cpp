#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_evaluate.h"
#include "diff/diff_defs.h"
#include "diff/diff.h"

#include "status.h"


typedef struct {
    OpType op;
    size_t arg_count;
    double (*function)(double, double);
} OpFuncTable;


static double evaluateAdd(double left_arg, double right_arg);
static double evaluateSub(double left_arg, double right_arg);
static double evaluateMul(double left_arg, double right_arg);
static double evaluateDiv(double left_arg, double right_arg);

static double evaluatePow(double left_arg, double right_arg);
static double evaluateLog(double left_arg, double right_arg);

static double evaluateSin(double, double right_arg);
static double evaluateCos(double, double right_arg);
static double evaluateTan(double, double right_arg);
static double evaluateCot(double, double right_arg);

static double evaluateAsin(double, double right_arg);
static double evaluateAcos(double, double right_arg);
static double evaluateAtan(double, double right_arg);
static double evaluateAcot(double, double right_arg);

static double evaluateSinh(double, double right_arg);
static double evaluateCosh(double, double right_arg);
static double evaluateTanh(double, double right_arg);
static double evaluateCoth(double, double right_arg);

static double evaluateAsinh(double, double right_arg);
static double evaluateAcosh(double, double right_arg);
static double evaluateAtanh(double, double right_arg);
static double evaluateAcoth(double, double right_arg);

static double diffOp(Differentiator* diff, const TreeNode* node);


static OpFuncTable table[] = {
    {OP_ADD, 2, evaluateAdd},
    {OP_SUB, 2, evaluateSub},
    {OP_MUL, 2, evaluateMul},
    {OP_DIV, 2, evaluateDiv},

    {OP_POW, 2, evaluatePow},
    {OP_LOG, 2, evaluateLog},

    {OP_SIN, 1, evaluateSin},
    {OP_COS, 1, evaluateCos},
    {OP_TAN, 1, evaluateTan},
    {OP_COT, 1, evaluateCot},

    {OP_ASIN, 1, evaluateAsin},
    {OP_ACOS, 1, evaluateAcos},
    {OP_ATAN, 1, evaluateAtan},
    {OP_ACOT, 1, evaluateAcot},

    {OP_SINH, 1, evaluateSinh},
    {OP_COSH, 1, evaluateCosh},
    {OP_TANH, 1, evaluateTanh},
    {OP_COTH, 1, evaluateCoth},

    {OP_ASINH, 1, evaluateAsinh},
    {OP_ACOSH, 1, evaluateAcosh},
    {OP_ATANH, 1, evaluateAtanh},
    {OP_ACOTH, 1, evaluateAcoth},
};


static double evaluateAdd(double left_arg, double right_arg) { return left_arg + right_arg; }
static double evaluateSub(double left_arg, double right_arg) { return left_arg - right_arg; }
static double evaluateMul(double left_arg, double right_arg) { return left_arg * right_arg; }
static double evaluateDiv(double left_arg, double right_arg)
{
    if (fabs(right_arg) < EPS) {
        fprintf(stderr, "Division by zero: %g / %g!\n", left_arg, right_arg);
        return NAN;
    }
    return left_arg / right_arg;
}


static double evaluatePow(double left_arg, double right_arg)
{
    if (left_arg < EPS) {
        fprintf(stderr, "Incorrect base of power: (%g)^(%g) (must be > 0)!\n", right_arg, left_arg);
        return NAN;
    }
    return pow(left_arg, right_arg);
}
static double evaluateLog(double left_arg, double right_arg)
{
    if (fabs(left_arg) <= EPS || fabs(left_arg - 1) < EPS) {
        fprintf(stderr, "Incorrect base of logarithm: log(%g, %g)  (must be > 0 and != 1)!\n",
                left_arg, right_arg);
        return NAN;
    }
    if (fabs(right_arg) <= EPS) {
        fprintf(stderr, "Incorrect logarithm argument: log(%g, %g) (must be > 0)!\n",
                left_arg, right_arg);
        return NAN;
    }
    return log(right_arg) / log(left_arg);
}


static double evaluateSin(double, double right_arg) { return sin(right_arg); }
static double evaluateCos(double, double right_arg) { return cos(right_arg); }
static double evaluateTan(double, double right_arg) { return tan(right_arg); }
static double evaluateCot(double, double right_arg)
{
    if (fabs(right_arg) < EPS) {
        fprintf(stderr, "Division by zero: 1 / tan(%g)!\n", right_arg);
        return NAN;
    }
    return 1 / tan(right_arg);
}


static double evaluateAsin(double, double right_arg)
{
    if (fabs(right_arg) > 1 + EPS) {
        fprintf(stderr, "Incorrect asin argument: asin(%g) (must be >= -1 and <= 1)!\n", right_arg);
        return NAN;
    }
    return asin(right_arg);
}
static double evaluateAcos(double, double right_arg)
{
    if (fabs(right_arg) > 1 + EPS) {
        fprintf(stderr, "Incorrect acos argument: acos(%g) (must be >= -1 and <= 1)!\n", right_arg);
        return NAN;
    }
    return acos(right_arg);
}
static double evaluateAtan(double, double right_arg) { return atan(right_arg); }
static double evaluateAcot(double, double right_arg) { return atan(1 / right_arg); }

static double evaluateSinh(double, double right_arg) { return sinh(right_arg); }
static double evaluateCosh(double, double right_arg) { return cosh(right_arg); }
static double evaluateTanh(double, double right_arg) { return tanh(right_arg); }
static double evaluateCoth(double, double right_arg)
{
    if (fabs(right_arg) < EPS) {
        fprintf(stderr, "Division by zero: 1 / tanh(%g)!\n", right_arg);
        return NAN;
    }
    return 1 / tanh(right_arg);
}


static double evaluateAsinh(double, double right_arg) { return asinh(right_arg); }
static double evaluateAcosh(double, double right_arg)
{
    if (right_arg < 1 + EPS) {
        fprintf(stderr, "Invalid acosh argument: acosh(%g) (must be >= 1)!\n", right_arg);
        return NAN;
    }
    return acosh(right_arg);
}
static double evaluateAtanh(double, double right_arg) 
{
    if (fabs(right_arg) > 1 - EPS) {
        fprintf(stderr, "Invalid atanh argument: acosh(%g) (must be > -1 and < 1)!\n", right_arg);
        return NAN;
    }
    return atanh(right_arg);
}
static double evaluateAcoth(double, double right_arg) 
{
    if (fabs(right_arg) < 1 + EPS) {
        fprintf(stderr, "Invalid acoth argument: acoth(%g) (must be < -1 and > 1)!\n", right_arg);
        return NAN;
    }
    return atanh(1 / right_arg);
}


static double diffOp(Differentiator* diff, const TreeNode* node)
{
    assert(diff);

    double left_arg  = evaluateNode(diff, node->left);
    if (isnan(left_arg)) return NAN;
    double right_arg = evaluateNode(diff, node->right);
    if (isnan(right_arg)) return NAN;

    if (table[node->value.op].arg_count == 1)
        return table[node->value.op].function(NAN, right_arg);
    else
        return table[node->value.op].function(left_arg, right_arg);
}


double evaluateNode(Differentiator* diff, const TreeNode* node)
{
    assert(diff);

    if (node == NULL)
        return 0;

    switch (node->type) {
        case NODE_OP:  return diffOp(diff, node);
        case NODE_VAR: return diff->var_table.variables[node->value.var_idx].value;
        case NODE_NUM: return node->value.num_val;
        default: fprintf(stderr, "Unknown node type!\n"); return 0;
    }
}


void diffEvaluate(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(tree_idx < diff->forest.count);
    assert(diff->forest.trees[tree_idx].root);

    double value = evaluateNode(diff, diff->forest.trees[diff->forest.count - 1].root);
    if (!isnan(value))
        printf("Value of %zu derivative: %g\n", tree_idx, value);
    else 
        printf("Value of %zu derivative is not defined\n", tree_idx);
};