#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_optimize.h"
#include "diff/diff_defs.h"
#include "diff/diff_evaluate.h"
#include "diff/diff.h"

#include "tree/tree.h"

#include "tex_dump/tex.h"

#include "graph_dump/html_builder.h"


#define ZERO(node) isNum(node, 0)
#define ONE(node)  isNum(node, 1)

#define NL node->left
#define NR node->right

typedef enum {
    FOLD_NOT_CONST = 0,
    FOLD_CONST,
    FOLD_OPTIMIZED
} FoldStatus;


static FoldStatus foldConstants(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff);
    
    if (!node)
        return FOLD_CONST;

    switch (node->type) {
        case NODE_OP: {
            FoldStatus left_res = foldConstants(diff, NL, tree_idx);
            FoldStatus right_res = foldConstants(diff, NR, tree_idx);

            if ((left_res == FOLD_OPTIMIZED || left_res == FOLD_CONST) &&
                (right_res == FOLD_OPTIMIZED || right_res == FOLD_CONST)) {

                fprintf(diff->tex_dump.file, "\\begin{dmath*}\n");
                printPaintedNode(diff, diff->forest.trees[tree_idx].root, node);
                fprintf(diff->tex_dump.file, " = ");

                double value = evaluateNode(diff, node);
                deleteBranch(NL);
                NL = NULL;
                deleteBranch(NR);
                NR = NULL;
                node->type = NODE_NUM;
                node->value.num_val = value;

                printPaintedNode(diff, diff->forest.trees[tree_idx].root, node);
                fprintf(diff->tex_dump.file, "\n\\end{dmath*}\n\n");

                return FOLD_OPTIMIZED;
            } else {
                return FOLD_NOT_CONST;
            }
        }
        case NODE_VAR: return FOLD_NOT_CONST;
        case NODE_NUM: return FOLD_CONST;
        default:       return FOLD_NOT_CONST;
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


static bool setNodeToChild(Differentiator* diff, TreeNode* node, size_t tree_idx, bool is_left)
{
    assert(diff); assert(node);

    fprintf(diff->tex_dump.file, "\\begin{dmath*}\n");
    printPaintedNode(diff, diff->forest.trees[tree_idx].root, node);
    fprintf(diff->tex_dump.file, " = ");

    if (is_left) {
        deleteBranch(NR);
        NR = NULL;
        replaceWithChild(node, NL);
    } else {
        deleteBranch(NL);
        NL = NULL;
        replaceWithChild(node, NR);
    }
    
    printPaintedNode(diff, diff->forest.trees[tree_idx].root, node);
    fprintf(diff->tex_dump.file, "\n\\end{dmath*}\n\n");

    return true;
}


static bool setNodeToNum(Differentiator* diff, TreeNode* node, size_t tree_idx, double num)
{
    assert(diff); assert(node);

    fprintf(diff->tex_dump.file, "\\begin{dmath*}\n");
    printPaintedNode(diff, diff->forest.trees[tree_idx].root, node);
    fprintf(diff->tex_dump.file, " = ");

    deleteBranch(NL);
    NL = NULL;
    deleteBranch(NR);
    NR = NULL;
    node->type = NODE_NUM;
    node->value.num_val = num;

    printPaintedNode(diff, diff->forest.trees[tree_idx].root, node);
    fprintf(diff->tex_dump.file, "\n\\end{dmath*}\n\n");

    return true;
}


static bool simplifyAdd(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL)) return setNodeToChild(diff, node, tree_idx, false);
    if (ZERO(NR)) return setNodeToChild(diff, node, tree_idx, true);

    return false;
}


static bool simplifySub(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NR)) return setNodeToChild(diff, node, tree_idx, true);

    return false;
}


static bool simplifyMul(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL) || ZERO(NR)) return setNodeToNum(diff, node, tree_idx, 0);
    if (ONE(NL))              return setNodeToChild(diff, node, tree_idx, false);
    if (ONE(NR))              return setNodeToChild(diff, node, tree_idx, true);

    return false;
}


static bool simplifyDiv(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL)) return setNodeToNum(diff, node, tree_idx, 0);
    if (ONE(NR))  return setNodeToChild(diff, node, tree_idx, true);

    return false;
}


static bool simplifyPow(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL))            return setNodeToNum(diff, node, tree_idx, 0);
    if (ONE(NL) || ZERO(NR)) return setNodeToNum(diff, node, tree_idx, 1);
    if (ONE(NR))             return setNodeToChild(diff, node, tree_idx, true);

    return false;
}


static bool simplifyOperations(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff);
    
    if (!node) return false;

    bool changed = false;
    if (simplifyOperations(diff, node->left, tree_idx)) changed = true;
    if (simplifyOperations(diff, node->right, tree_idx)) changed = true;
   
    if (node->type != NODE_OP)
        return changed;

    bool curr_changed = false;
    switch (node->value.op) {
        case OP_ADD: curr_changed = simplifyAdd(diff, node, tree_idx); break;
        case OP_SUB: curr_changed = simplifySub(diff, node, tree_idx); break;
        case OP_MUL: curr_changed = simplifyMul(diff, node, tree_idx); break;
        case OP_DIV: curr_changed = simplifyDiv(diff, node, tree_idx); break;

        case OP_POW: curr_changed = simplifyPow(diff, node, tree_idx); break;
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

    fprintf(diff->tex_dump.file, "Начинаем оптимизировать %zu производную:\n", tree_idx);
    bool changed = true;
    while (changed) {
        FoldStatus status = foldConstants(diff, diff->forest.trees[tree_idx].root, tree_idx);
        bool simplified = simplifyOperations(diff, diff->forest.trees[tree_idx].root, tree_idx);
        changed = simplified || status == FOLD_OPTIMIZED;
    }  

    TREE_DUMP(diff, tree_idx, STATUS_OK, "source tree");
    fprintf(diff->tex_dump.file, "После оптимизации получаем:\n");
    printExpression(diff, tree_idx);
}