#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_process.h"
#include "diff/diff_dsl.h"
#include "diff/diff_defs.h"
#include "diff/diff.h"

#include "tex_dump/tex_struct.h"
    

#define PR(string) fprintf(TEX_FILE, string)

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


static TreeNode* diffOp(Differentiator* diff, TreeNode* node, size_t var_idx);

static inline void printDerivativeExpression(Differentiator* diff, TreeNode* node);

static TreeNode* copyNode(const TreeNode* node);

static TreeNode* nodeDup(const TreeNode* node);


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


static TreeNode* diffOp(Differentiator* diff, TreeNode* node, size_t var_idx)
{
    assert(node);

    switch (node->value.op) {
        case OP_ADD: {
            printDerivativeExpression(diff, node);
            printTex(diff, "\\left(%n\\right)'+\\left(%n\\right)'\n\\end{dmath*}\n", NL, NR);
            return ADD(dNL, dNR);
        }
        case OP_SUB: {
            printDerivativeExpression(diff, node);
            printTex(diff, "\\left(%n\\right)'-\\left(%n\\right)'\n\\end{dmath*}\n", NL, NR);
            return SUB(dNL, dNR);
        }
        case OP_MUL: {
            printDerivativeExpression(diff, node);
            printTex(diff, "\\left(%n\\right)'\\cdot\\left(%n\\right)+"
                "\\left(%n\\right)\\cdot\\left(%n\\right)'\n\\end{dmath*}\n",
                NL, NR, NL, NR);

            return ADD(MUL(dNL, cNR), MUL(cNL, dNR));
        }
        case OP_DIV: {
            printDerivativeExpression(diff, node);
            printTex(diff, "\\frac{\\left(%n\\right)'\\cdot\\left(%n\\right)-"
                "\\left(%n\\right)\\cdot\\left(%n\\right)'}{%n}\n\\end{dmath*}\n",
                NL, NR, NL, NR, NR);

            return DIV(SUB(MUL(dNL, cNR), MUL(cNL, dNR)), POW(cNR, CNUM(2)));
        }
        case OP_POW: {
            printDerivativeExpression(diff, node);
            printTex(diff, "\\left(\\left(%n\\right)'\\cdot\\ln\\left(%n\\right)+"
                "\\frac{\\left(%n\\right)\\cdot\\left(%n\\right)'}{%n}\\right)\\cdot\\left(%n\\right)^{%n}\n\\end{dmath*}\n",
                NR, NL, NR, NL, NL, NR);

            return MUL(ADD(MUL(dNR, LOG(CNUM(M_E), cNL)), MUL(DIV(cNR, cNL), dNL)), POW(cNL, cNR));
        }
        case OP_LOG: {
            printDerivativeExpression(diff, node);
            printTex(diff, "\\frac{\\frac{\\left(%n\\right)'\\cdot\\ln\\left(%n\\right)}{%n}-"
                "\\frac{\\left(%n\\right)'\\cdot\\ln\\left(%n\\right)}{\\left(%n\\right)}{\\left(%n\\right)^2}\n\\end{dmath*}\n",
                NR, NL, NR, NL, NR, NL, NL);

            return DIV(SUB(DIV(MUL(dNR, LOG(CNUM(M_E), cNL)), cNR), DIV(MUL(dNL, LOG(CNUM(M_E), cNR)), cNL)), POW(LOG(CNUM(M_E), cNL), CNUM(2)));
        }
        case OP_SIN:   return MUL(COS(cNR), dNR);
        case OP_COS:   return MUL(MUL(CNUM(-1), SIN(cNR)), dNR);
        case OP_TAN:   return DIV(dNR, POW(COS(cNR), CNUM(2)));
        case OP_COT:   return MUL(DIV(CNUM(-1), POW(SIN(cNR), CNUM(2))), dNR);

        case OP_ASIN:  return MUL(POW(SUB(CNUM(1), POW(cNR, CNUM(2))), CNUM(-0.5)), dNR);
        case OP_ACOS:  return MUL(MUL(CNUM(-1), POW(SUB(CNUM(1), POW(cNR, CNUM(2))), CNUM(-0.5))), dNR);
        case OP_ATAN:  return DIV(CNUM(1), ADD(CNUM(1), POW(cNR, CNUM(2))));
        case OP_ACOT:  return MUL(DIV(CNUM(1), ADD(CNUM(1), POW(cNR, CNUM(2)))), CNUM(-1));
        
        case OP_SINH:  return MUL(COSH(cNR), dNR);
        case OP_COSH:  return MUL(SINH(cNR), dNR);
        case OP_TANH:  return MUL(DIV(CNUM(1), POW(COSH(cNR), CNUM(2))), dNR);
        case OP_COTH:  return MUL(DIV(CNUM(-1), POW(SINH(cNR), CNUM(2))), dNR);

        case OP_ASINH: return MUL(DIV(CNUM(1), POW(ADD(POW(cNR, CNUM(2)), CNUM(1)), CNUM(0.5))), dNR);
        case OP_ACOSH: return MUL(DIV(CNUM(1), POW(SUB(POW(cNR, CNUM(2)), CNUM(1)), CNUM(0.5))), dNR);
        case OP_ATANH: return MUL(DIV(CNUM(1), SUB(CNUM(1), POW(cNR, CNUM(2)))), dNR);
        case OP_ACOTH: return MUL(DIV(CNUM(1), SUB(CNUM(1), POW(cNR, CNUM(2)))), dNR);

        case OP_NONE:  fprintf(stderr, "Operation 'NONE' detected\n"); return NULL;
        default:       fprintf(stderr, "Unknown operation\n"); return NULL;
    }
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


static inline void printDerivativeExpression(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node);

    printTex(diff, "\\begin{dmath*}\n\\left(%n\\right)' = ", node);
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

