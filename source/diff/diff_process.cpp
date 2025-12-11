#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_process.h"
#include "diff/diff_create.h"
#include "diff/diff_defs.h"
#include "diff/diff.h"

#include "tex_dump/tex_struct.h"
    

#define L node->left
#define R node->right


#define dL diffNode(diff, L)
#define dR diffNode(diff, R)
#define cL copyNode(L)
#define cR copyNode(R)


#define PRINT_EXPRESSION(printExpression)                                      \
    do {                                                                       \
        if (diff->tex_dump.print_steps) {                                      \
            printTex(diff, "\\begin{dmath*}\n\\left(%n\\right)' = ", node);    \
            printExpression;                                                   \
            printTex(diff, "\n\\end{dmath*}\n");                               \
        }                                                                      \
    } while (0)


typedef TreeNode* (*ComputeDerivativeFunc)(Differentiator* diff, TreeNode* node);


static TreeNode* diffOp(Differentiator* diff, TreeNode* node);
static TreeNode* copyNode(const TreeNode* node);
static TreeNode* nodeDup(const TreeNode* node);


static TreeNode* computeAddDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeSubDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeMulDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeDivDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computePowDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeLogDerivative(Differentiator* diff, TreeNode* node);

static TreeNode* computeSinDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeCosDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeTanDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeCotDerivative(Differentiator* diff, TreeNode* node);

static TreeNode* computeAsinDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeAcosDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeAtanDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeAcotDerivative(Differentiator* diff, TreeNode* node);

static TreeNode* computeSinhDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeCoshDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeTanhDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeCothDerivative(Differentiator* diff, TreeNode* node);

static TreeNode* computeAsinhDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeAcoshDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeAtanhDerivative(Differentiator* diff, TreeNode* node);
static TreeNode* computeAcothDerivative(Differentiator* diff, TreeNode* node);


ComputeDerivativeFunc computeDerivativeTable[OP_MAX_COUNT] = {
    [OP_ADD] = computeAddDerivative,
    [OP_SUB] = computeSubDerivative,
    [OP_MUL] = computeMulDerivative,
    [OP_DIV] = computeDivDerivative,
    [OP_POW] = computePowDerivative,
    [OP_LOG] = computeLogDerivative,

    [OP_SIN] = computeSinDerivative,
    [OP_COS] = computeCosDerivative,
    [OP_TAN] = computeTanDerivative,
    [OP_COT] = computeCotDerivative,

    [OP_ASIN] = computeAsinDerivative,
    [OP_ACOS] = computeAcosDerivative,
    [OP_ATAN] = computeAtanDerivative,
    [OP_ACOT] = computeAcotDerivative,

    [OP_SINH] = computeSinhDerivative,
    [OP_COSH] = computeCoshDerivative,
    [OP_TANH] = computeTanhDerivative,
    [OP_COTH] = computeCothDerivative,

    [OP_ASINH] = computeAsinhDerivative,
    [OP_ACOSH] = computeAcoshDerivative,
    [OP_ATANH] = computeAtanhDerivative,
    [OP_ACOTH] = computeAcothDerivative,
};


TreeNode* diffNode(Differentiator* diff, TreeNode* node)
{
    assert(node);

    switch (node->type) {
        case NODE_NUM: return CNUM(0);
        case NODE_VAR: 
            if (node->value.var_idx == diff->args.derivative_info.diff_var_idx) {
                return CNUM(1);
            } else {
                return CNUM(0);
            }
        case NODE_OP:  return diffOp(diff, node);
        default:       return NULL;
    }    
}


bool containsVariable(TreeNode* node, size_t var_idx)
{
    if (node == NULL)
        return false;

    switch (node->type) {
        case NODE_OP:  return containsVariable(L, var_idx) || containsVariable(R, var_idx);
        case NODE_VAR: return node->value.var_idx == var_idx;
        case NODE_NUM: return false;
        default:       return false; 
    }
}


static TreeNode* diffOp(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); assert(node->type == NODE_OP);
    assert(node->value.op < OP_MAX_COUNT);

    return computeDerivativeTable[node->value.op](diff, node);
}


static TreeNode* copyNode(const TreeNode* node)
{
    if (node == NULL)
        return NULL;
    TreeNode* new_node = nodeDup(node);
    if (new_node == NULL)
        return NULL;
    new_node->left = copyNode(node->left);
    if (node->left != NULL)
        new_node->left->parent = new_node;

    new_node->right = copyNode(node->right);
    if (new_node->right != NULL)
        new_node->right->parent = new_node;
   
    return new_node;

}


static TreeNode* nodeDup(const TreeNode* node)
{
    assert(node);

    TreeNode* new_node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (new_node == NULL)
        return NULL;
    new_node->type = node->type;
    new_node->value = node->value;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    
    return new_node;
}


// ------------------------------------------------------------------------------------------------
// OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_POS, OP_LOG
// ------------------------------------------------------------------------------------------------
static TreeNode* computeAddDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 
    PRINT_EXPRESSION(
        printTex(diff, "\\left(%n\\right)'+\\left(%n\\right)'", L, R)
    );

    return ADD(dL, dR);
}
static TreeNode* computeSubDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 
    
    PRINT_EXPRESSION(
        printTex(diff, "\\left(%n\\right)'-\\left(%n\\right)'", L, R)
    );

    return SUB(dL, dR);
}
static TreeNode* computeMulDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 
    
    PRINT_EXPRESSION(
        printTex(diff, "\\left(%n\\right)'\\cdot\\left(%n\\right)+"
            "\\left(%n\\right)\\cdot\\left(%n\\right)'",
            L, R, L, R)
    );

    return ADD(MUL(dL, cR), MUL(cL, dR));
}
static TreeNode* computeDivDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{\\left(%n\\right)'\\cdot\\left(%n\\right)-"
            "\\left(%n\\right)\\cdot\\left(%n\\right)'}{%n}",
            L, R, L, R, R)
    );

    return DIV(SUB(MUL(dL, cR), MUL(cL, dR)), POW(cR, CNUM(2)));
}
static TreeNode* computePowDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    size_t var_idx = diff->args.derivative_info.diff_var_idx;
    bool left_contains = containsVariable(L, var_idx);
    bool right_contains = containsVariable(R, var_idx);
    if (!left_contains && !right_contains) {
        PRINT_EXPRESSION(
            printTex(diff, "0")
        );
        return CNUM(0);
    } else if (left_contains && !right_contains) {
        PRINT_EXPRESSION(
            printTex(diff, "\\left(%n\\right)\\cdot\\left(%n\\right)^{%n-1}\\cdot\\left(%n\\right)",
                R, L, R, L)
        );
        return MUL(MUL(cR, POW(cL, SUB(cR, CNUM(1)))), dL);
    } else if (!left_contains && right_contains) {
        PRINT_EXPRESSION(
            printTex(diff, "\\left(%n\\right)\\cdot\\left(%n\\right)^{%n|\\cdot\\ln\\left(%n\\right)}",
                R, L, R, L)
        );
        return MUL(MUL(POW(cL, cR), LOG(CNUM(M_E), cL)), dR);
    } else {
        PRINT_EXPRESSION(
            printTex(diff, "\\left(\\left(%n\\right)'\\cdot\\ln\\left(%n\\right)+"
                "\\frac{\\left(%n\\right)\\cdot\\left(%n\\right)'}{%n}"
                "\\right)\\cdot\\left(%n\\right)^{%n}", R, L, R, L, L, L, R)
        );
        return MUL(ADD(MUL(dR, LOG(CNUM(M_E), cL)), MUL(DIV(cR, cL), dL)), POW(cL, cR));
    }
}
static TreeNode* computeLogDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    size_t var_idx = diff->args.derivative_info.diff_var_idx;
    bool left_contains = containsVariable(L, var_idx);
    bool right_contains = containsVariable(R, var_idx);
    if (!left_contains && !right_contains) {
        PRINT_EXPRESSION(
            printTex(diff, "0")
        );
        return CNUM(0);
    } else if (left_contains && !right_contains) {
        PRINT_EXPRESSION(
            printTex(diff, "-\\frac{\\ln\\left(%n\\right)\\cdot\\left(%n\\right)}{\\left(\\ln"
                "\\left(%n\right)\\right)^2\\cdot\\left(%n\\right)}", R, L, R, L)
        );
        return DIV(MUL(MUL(CNUM(-1), LOG(CNUM(M_E), cR)), dL), MUL(POW(LOG(CNUM(M_E), cL), CNUM(2)), cL));
    } else if (!left_contains && right_contains) {
        PRINT_EXPRESSION(
            printTex(diff, "\\frac{\\left(%n\\right)'}{%n\\cdot\\ln\\left(%n\\right)}", R, R, L)
        );
        return DIV(dR, MUL(cR, LOG(CNUM(M_E), cL)));
    } else {
        PRINT_EXPRESSION(
            printTex(diff, "\\frac{\\frac{\\left(%n\\right)'\\cdot\\ln\\left(%n\\right)}{%n}-"
                "\\frac{\\left(%n\\right)'\\cdot\\ln\\left(%n\\right)}{\\left(%n\\right)}"
                "{\\left(%n\\right)^2}", R, L, R, L, R, L, L)
        );
        return DIV(SUB(DIV(MUL(dR, LOG(CNUM(M_E), cL)), cR), DIV(MUL(dL, LOG(CNUM(M_E), cR)),
            cL)), POW(LOG(CNUM(M_E), cL), CNUM(2)));
    };
}

// ------------------------------------------------------------------------------------------------
// OP_SIN, OP_COS, OP_TAN, OP_COT
// ------------------------------------------------------------------------------------------------
static TreeNode* computeSinDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\cos\\left(%n\\right)\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(COS(cR), dR);
}
static TreeNode* computeCosDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "-\\sin\\left(%n\\right)\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(MUL(CNUM(-1), SIN(cR)), dR);
}
static TreeNode* computeTanDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{\\cos^2\\left(%n\\right)}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(DIV(CNUM(1), POW(COS(cR), CNUM(2))), dR);
}
static TreeNode* computeCotDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "-\\frac{1}{\\sin^2\\left(%n\\right)}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(DIV(CNUM(-1), POW(SIN(cR), CNUM(2))), dR);
}

// ------------------------------------------------------------------------------------------------
// OP_ASIN, OP_ACOS, OP_ATAN, OP_ACOT
// ------------------------------------------------------------------------------------------------
static TreeNode* computeAsinDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{\\left(1-\\left(%n\\right)^2\\right)^{0.5}}\\cdot\\left(%n\\right)'", R, R)
    ); 
    return MUL(POW(SUB(CNUM(1), POW(cR, CNUM(2))), CNUM(-0.5)), dR);
}
static TreeNode* computeAcosDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "-\\frac{1}{\\left(1-\\left(%n\\right)^2\\right)^{0.5}}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(MUL(CNUM(-1), POW(SUB(CNUM(1), POW(cR, CNUM(2))), CNUM(-0.5))), dR);
}
static TreeNode* computeAtanDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{1+\\left(%n\\right)^2}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(DIV(CNUM(1), ADD(CNUM(1), POW(cR, CNUM(2)))), dR);
}
static TreeNode* computeAcotDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "-\\frac{1}{1+\\left(%n\\right)^2}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(MUL(DIV(CNUM(1), ADD(CNUM(1), POW(cR, CNUM(2)))), CNUM(-1)), dR);
}

// ------------------------------------------------------------------------------------------------
// OP_SINH, OP_COSH, OP_TANH, OP_COTH
// ------------------------------------------------------------------------------------------------
static TreeNode* computeSinhDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\cosh\\left(%n\\right)\\cdot\\left(%n\\right)'", R, R)
    ); 
    return MUL(COSH(cR), dR);
}
static TreeNode* computeCoshDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\sinh\\left(%n\\right)\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(SINH(cR), dR);
}
static TreeNode* computeTanhDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{\\cosh^2\\left(%n\\right)}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(DIV(CNUM(1), POW(COSH(cR), CNUM(2))), dR);
}
static TreeNode* computeCothDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "-\\frac{1}{\\sinh^2\\left(%n\\right)}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(DIV(CNUM(-1), POW(SINH(cR), CNUM(2))), dR);
}

// ------------------------------------------------------------------------------------------------
// OP_ASINH, OP_ACOSH, OP_ATANH, OP_ACOTH
// ------------------------------------------------------------------------------------------------
static TreeNode* computeAsinhDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{\\left(\\left(%n\\right)^2+1\\right)^{0.5}}\\cdot\\left(%n\\right)'", R, R)
    ); 
    return MUL(DIV(CNUM(1), POW(ADD(POW(cR, CNUM(2)), CNUM(1)), CNUM(0.5))), dR);
}
static TreeNode* computeAcoshDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{\\left(\\left(%n\\right)^2-1\\right)^{0.5}}\\cdot\\left(%n\\right)'", R, R)
    ); 
    return MUL(DIV(CNUM(1), POW(SUB(POW(cR, CNUM(2)), CNUM(1)), CNUM(0.5))), dR);
}
static TreeNode* computeAtanhDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{1-\\left(%n\\right)^2}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(DIV(CNUM(1), SUB(CNUM(1), POW(cR, CNUM(2)))), dR);
}
static TreeNode* computeAcothDerivative(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); 

    PRINT_EXPRESSION(
        printTex(diff, "\\frac{1}{1-\\left(%n\\right)^2}\\cdot\\left(%n\\right)'", R, R)
    );
    return MUL(DIV(CNUM(1), SUB(CNUM(1), POW(cR, CNUM(2)))), dR);
}


// undef diff_create.h

#undef CNUM
#undef CVAR

#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef POW
#undef LOG

#undef SIN
#undef COS
#undef TAN
#undef COT

#undef ASIN
#undef ACOS
#undef ATAN
#undef ACOT

#undef SINH
#undef COSH
#undef TANH
#undef COTH

#undef ASINH
#undef ACOSH
#undef ATANH
#undef ACOTH