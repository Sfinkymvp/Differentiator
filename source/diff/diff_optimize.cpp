#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_optimize.h"
#include "diff/diff_defs.h"
#include "diff/diff.h"

#include "tree/tree.h"


static const double EPS = 1e-7;

#define ZERO(node) isNum(node, 0)
#define ONE(node)  isNum(node, 1)

#define NL node->left
#define NR node->right


bool foldConstants(Differentiator* diff, TreeNode* node)
{
    assert(diff);
    
    if (!node)
        return true;

    switch (node->type) {
        case NODE_OP: {
            if (foldConstants(diff, NL) &&
                foldConstants(diff, NR)) {

                double value = diffOp(diff, node);
                deleteBranch(NL);
                NL = NULL;
                deleteBranch(NR);
                NR = NULL;
                node->type = NODE_NUM;
                node->value.num_val = value;

                return true;
            } else {
                return false;
            }
        }
        case NODE_VAR: return false;
        case NODE_NUM: return true;
        default:       return false; 
    }
}


static bool isNum(TreeNode* node, double num)
{
    assert(node);

    return node->type == NODE_NUM && (fabs(node->value.num_val - num) < EPS);
}


static void replaceWithChild(TreeNode* parent, TreeNode* child)
{
    assert(parent); assert(child);

    parent->type = child->type;
    parent->value = child->value;
    parent->left = child->left;
    parent->right = child->right;
    if (parent->left) parent->left->parent = parent;
    if (parent->right) parent->right->parent = parent;

    free(child);
}


static bool simplifyAdd(TreeNode* node) 
{
    assert(node);

    if (ZERO(NL)) {
        deleteBranch(NL);
        NL = NULL;
        replaceWithChild(node, NR);
    } else if (ZERO(NR)) {
        deleteBranch(NR);
        NR = NULL;
        replaceWithChild(node, NL);
    } else {
        return false;
    }

    return true;
}


static bool simplifySub(TreeNode* node)
{
    assert(node);

    if (ZERO(NR)) {
        deleteBranch(NR);
        NR = NULL;
        replaceWithChild(node, NL);
    } else {
        return false;
    }

    return true;
}


static bool simplifyMul(TreeNode* node)
{
    assert(node);

    if (ZERO(NL) || ZERO(NR)) {
        deleteBranch(NL);
        NL = NULL;
        deleteBranch(NR);
        NR = NULL;
        node->type = NODE_NUM;
        node->value.num_val = 0;
    } else if (ONE(NL)) {
        deleteBranch(NL);
        NL = NULL;
        replaceWithChild(node, NR);
    } else if (ONE(NR)) {
        deleteBranch(NR);
        NR = NULL;
        replaceWithChild(node, NL);
    } else {
        return false;
    }

    return true;
}


static bool simplifyDiv(TreeNode* node)
{
    assert(node);

    if (ZERO(NL)) {
        deleteBranch(NL);
        NL = NULL;
        deleteBranch(NR);
        NR = NULL;

        node->type = NODE_NUM;
        node->value.num_val = 0;
    } else if (ONE(NR)) {
        deleteBranch(NR);
        NR = NULL;
        replaceWithChild(node, NL);
    } else {
        return false;
    }

    return true;
}


static bool simplifyPow(TreeNode* node)
{
    assert(node);

    if (ZERO(NL) || ONE(NL) || ZERO(NR)) {
        deleteBranch(NL);
        NL = NULL;
        deleteBranch(NR);
        NR = NULL;
        node->type = NODE_NUM;
        if (ZERO(NL))            node->value.num_val = 0;
        if (ONE(NL) || ZERO(NR)) node->value.num_val = 1;
    } else if (ONE(NR)) {
        deleteBranch(NR);
        NR = NULL;
        replaceWithChild(node, NL);
    } else  {
        return false;
    }

    return true;
}


bool simplifyOperations(Differentiator* diff, TreeNode* node)
{
    assert(diff);
    
    if (!node) return false;

    bool changed = false;
    if (simplifyOperations(diff, node->left)) changed = true;
    if (simplifyOperations(diff, node->right)) changed = true;
   
    if (node->type != NODE_OP)
        return changed;

    bool curr_changed = false;
    switch (node->value.op) {
        case OP_ADD: curr_changed = simplifyAdd(node); break;
        case OP_SUB: curr_changed = simplifySub(node); break;
        case OP_MUL: curr_changed = simplifyMul(node); break;
        case OP_DIV: curr_changed = simplifyDiv(node); break;

        case OP_POW: curr_changed = simplifyPow(node); break;
        case OP_LOG:
        
        case OP_SIN:
        case OP_COS:
        case OP_TAN:
        case OP_COT:

        case OP_ASIN:
        case OP_ACOS:
        case OP_ATAN:
        case OP_ACOT:

        case OP_SINH:
        case OP_COSH:
        case OP_TANH:
        case OP_COTH:

        case OP_ASINH:
        case OP_ACOSH:
        case OP_ATANH:
        case OP_ACOTH:

        case OP_NONE:
        default: curr_changed = false; break;
    }
    
    return changed || curr_changed;
}


void optimizeTree(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->forest.trees); assert(tree_idx < diff->forest.count);

    bool changed = true;
    while (changed) {
        bool simplified = simplifyOperations(diff, diff->forest.trees[tree_idx].root);
        bool folded = foldConstants(diff, diff->forest.trees[tree_idx].root);
        changed = simplified || folded;
    }  
}

