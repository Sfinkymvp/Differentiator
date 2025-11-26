#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_process.h"
#include "diff/diff_defs.h"
#include "diff/diff.h"

#include "tex_dump/tex.h"
    

#define FILE diff->tex_dump.file

#define dL diffNode(diff, left, var_idx)
#define dR diffNode(diff, right, var_idx)
#define cL copyNode(left)
#define cR copyNode(right)

#define NL node->left
#define NR node->right

#define dNL diffNode(diff, NL, var_idx)
#define dNR diffNode(diff, NR, var_idx)
#define cNL copyNode(NL)
#define cNR copyNode(NR)

#define CNUM(val) createNum(val)

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


static inline void printDerivativeExpression(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node);

    fprintf(FILE, "\\begin{dmath*}\n");
    fprintf(FILE, "\\left( ");
    printNode(diff, node);
    fprintf(FILE, " \\right)' = ");
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


TreeNode* createOp(OpType op, TreeNode* left, TreeNode* right)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (node == NULL)
        return NULL;

    node->type = NODE_OP;
    node->value.op = op;
    node->left = left;
    node->right = right;
    
    if (left) left->parent = node;
    if (right) right->parent = node;

    return node;
}


static TreeNode* createNum(double value) {
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (node == NULL)
        return NULL;

    node->type = NODE_NUM;
    node->value.num_val = value;
    return node;
}


bool containsVariable(TreeNode* node, size_t var_idx)
{
    if (node == NULL)
        return false;

    switch (node->type) {
        case NODE_OP:  return containsVariable(NL, var_idx) || containsVariable(NR, var_idx);
        case NODE_VAR: return node->value.var_idx == var_idx;
        case NODE_NUM: return false;
        default:       return false; 
    }
}


TreeNode* diffPow(Differentiator* diff, TreeNode* left, TreeNode* right, size_t var_idx)
{
    assert(diff); assert(left); assert(right); assert(left->parent);

    if (!left || !right)
        return NULL;

    bool left_contains = containsVariable(left, var_idx);
    bool right_contains = containsVariable(right, var_idx);

    printDerivativeExpression(diff, left->parent);

    if (!left_contains && !right_contains) {
        fprintf(FILE, "0"); fprintf(FILE, "\\end{dmath*}\n");
        return CNUM(0);
    }
    if (left_contains && !right_contains) {
        fprintf(FILE, "\\left( "); printNode(diff, right);
        fprintf(FILE, " \\right) \\cdot \\left("); printNode(diff, left);
        fprintf(FILE, "\\right)^{ "); printNode(diff, right); fprintf(FILE, " - 1} \\cdot ");
        fprintf(FILE, "\\left( "); printNode(diff, left); fprintf(FILE, "\\right)'\n");
        fprintf(FILE, "\\end{dmath*}\n");

        return MUL(MUL(cR, POW(cL, SUB(cR, CNUM(1)))), dL);
    }
    if (!left_contains && right_contains)
        return MUL(MUL(POW(cL, cR), LOG(CNUM(M_E), cL)), dR);
    else
        return MUL(ADD(MUL(dR, LOG(CNUM(M_E), cL)), MUL(DIV(cR, cL), dL)), POW(cL, cR));
}


TreeNode* diffLog(Differentiator* diff, TreeNode* left, TreeNode* right, size_t var_idx)
{
    if (!left || !right)
        return NULL;
    
    bool left_contains = containsVariable(left, var_idx);
    if (!left_contains)
        return DIV(dR, MUL(cR, LOG(CNUM(M_E), cL)));
    else
        return DIV(SUB(DIV(MUL(dR, LOG(CNUM(M_E), cL)), cR), DIV(MUL(dL, LOG(CNUM(M_E), cR)), cL)), POW(LOG(CNUM(M_E), cL), CNUM(2)));

}


TreeNode* diffOp(Differentiator* diff, TreeNode* node, size_t var_idx)
{
    assert(node);

    switch (node->value.op) {
        case OP_ADD: {
            printDerivativeExpression(diff, node);
            fprintf(FILE, "\\left( ");
            printNode(diff, NL); fprintf(FILE, " \\right)' + \\left( ");
            printNode(diff, NR); fprintf(FILE, " \\right)'\n");
            fprintf(FILE, "\\end{dmath*}\n");

            return ADD(dNL, dNR);
        }
        case OP_SUB: {
            printDerivativeExpression(diff, node);
            fprintf(FILE, "\\left( ");
            printNode(diff, NL); fprintf(FILE, " \\right)' - \\left( ");
            printNode(diff, NR); fprintf(FILE, " \\right)'\n");
            fprintf(FILE, "\\end{dmath*}\n");

            return SUB(dNL, dNR);
        }
        case OP_MUL: {
            printDerivativeExpression(diff, node);
            fprintf(FILE, "\\left( "); printNode(diff, NL);
            fprintf(FILE, " \\right)' \\cdot \\left( "); printNode(diff, NR);
            fprintf(FILE, " \\right) + \\left( "); printNode(diff, NL);
            fprintf(FILE, "\\right) \\cdot \\left( "); printNode(diff, NR);
            fprintf(FILE, " \\right)'\n"); fprintf(FILE, "\\end{dmath*}\n");

            return ADD(MUL(dNL, cNR), MUL(cNL, dNR));
        }
        case OP_DIV: {
            printDerivativeExpression(diff, node);
            fprintf(FILE, "\\frac{\\left( "); printNode(diff, NL);
            fprintf(FILE, " \\right)' \\cdot \\left( "); printNode(diff, NR);
            fprintf(FILE, " \\right) - \\left( "); printNode(diff, NL);
            fprintf(FILE, "\\right) \\cdot \\left( "); printNode(diff, NR);
            fprintf(FILE, " \\right)'}{"); printNode(diff, NR);
            fprintf(FILE, "}\n"); fprintf(FILE, "\\end{dmath*}\n");

            return DIV(SUB(MUL(dNL, cNR), MUL(cNL, dNR)), POW(cNR, CNUM(2)));
        }

        case OP_POW:   return diffPow(diff, NL, NR, var_idx);
        case OP_LOG:   return diffLog(diff, NL, NR, var_idx);

        case OP_SIN:   return MUL(COS(cNR), dNR);
        case OP_COS:   return MUL(MUL(CNUM(-1), SIN(cNR)), dNR);
        case OP_TAN:   return DIV(dNR, POW(COS(cNR), CNUM(2)));
        case OP_COT:   return MUL(DIV(CNUM(-1), POW(SIN(cNR), CNUM(2))), dNR);

        case OP_ASIN:  return MUL(POW(SUB(CNUM(1), POW(cNR, CNUM(2))), CNUM(-0.5)), dNR);
        case OP_ACOS:  return MUL(MUL(CNUM(-1), POW(SUB(CNUM(1), POW(cNR, CNUM(2))), CNUM(-0.5))), dNR);
        case OP_ATAN:  return MUL(POW(ADD(CNUM(1), POW(cNR, CNUM(2))), CNUM(-1)), dNR);
        case OP_ACOT:  return MUL(MUL(POW(ADD(CNUM(1), POW(cNR, CNUM(2))), CNUM(-1)), CNUM(-1)), dNR);
        
        case OP_SINH:  return MUL(COSH(cNR), dNR);
        case OP_COSH:  return MUL(SINH(cNR), dNR);
        case OP_TANH:  return DIV(dNR, POW(COSH(cNR), CNUM(2)));
        case OP_COTH:  return MUL(DIV(CNUM(-1), POW(SINH(cNR), CNUM(2))), dNR);

        case OP_ASINH: return DIV(dNR, POW(ADD(POW(cNR, CNUM(2)), CNUM(1)), CNUM(0.5)));
        case OP_ACOSH: return DIV(dNR, POW(SUB(POW(cNR, CNUM(2)), CNUM(1)), CNUM(0.5)));
        case OP_ATANH: return MUL(POW(SUB(CNUM(1), POW(cNR, CNUM(2))), CNUM(-1)), dNR);
        case OP_ACOTH: return MUL(POW(SUB(CNUM(1), POW(cNR, CNUM(2))), CNUM(-1)), dNR);

        case OP_NONE:  fprintf(stderr, "Operation 'NONE' detected\n"); return NULL;
        default:       fprintf(stderr, "Unknown operation\n"); return NULL;
    }
}


TreeNode* diffNode(Differentiator* diff, TreeNode* node, size_t var_idx)
{
    assert(node);

    switch (node->type) {
        case NODE_NUM: return CNUM(0);
        case NODE_VAR:
            if (node->value.var_idx == var_idx)
                return CNUM(1);
            else
                return CNUM(0);
        case NODE_OP: {
            return diffOp(diff, node, var_idx);
        }
        default: return NULL;
    }    
}