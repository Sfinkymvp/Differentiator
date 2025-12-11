#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "diff/diff_optimize.h"
#include "diff/diff_defs.h"
#include "diff/diff_evaluate.h"
#include "diff/diff.h"

#include "tree/tree.h"

#include "tex_dump/tex_struct.h"

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


static FoldStatus foldConstants(Differentiator* diff, TreeNode* node, size_t tree_idx);

static bool simplifyOperations(Differentiator* diff, TreeNode* node, size_t tree_idx);
static bool simplifyDispatcher(Differentiator* diff, TreeNode* node, size_t tree_idx);

static bool simplifyAdd(Differentiator* diff, TreeNode* node, size_t tree_idx);
static bool simplifySub(Differentiator* diff, TreeNode* node, size_t tree_idx);
static bool simplifyMul(Differentiator* diff, TreeNode* node, size_t tree_idx);
static bool simplifyDiv(Differentiator* diff, TreeNode* node, size_t tree_idx);
static bool simplifyPow(Differentiator* diff, TreeNode* node, size_t tree_idx);

static bool setNodeToChild(Differentiator* diff, TreeNode* node, size_t tree_idx, bool is_left);
static void replaceWithChild(TreeNode* parent, TreeNode* child);
static bool setNodeToNum(Differentiator* diff, TreeNode* node, size_t tree_idx, double num);
static bool isNum(TreeNode* node, double num);


typedef bool (*simplifierFunc)(Differentiator* diff, TreeNode* node, size_t tree_idx);
const simplifierFunc SIMPLIFIERS[OP_MAX_COUNT] = {
    [OP_ADD] = simplifyAdd,
    [OP_SUB] = simplifySub,
    [OP_MUL] = simplifyMul,
    [OP_DIV] = simplifyDiv,
    [OP_POW] = simplifyPow
};


void optimizeTree(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->forest.trees); assert(tree_idx <= diff->forest.count);

    if (diff->tex_dump.print_steps) {
        printTex(diff, "\\subsection{Оптимизация}\n");
    }
    bool changed = true;
    while (changed) {
        FoldStatus status = foldConstants(diff, diff->forest.trees[tree_idx].root, tree_idx);
        bool simplified = simplifyOperations(diff, diff->forest.trees[tree_idx].root, tree_idx);
        changed = simplified || status == FOLD_OPTIMIZED;
    }  

    TREE_DUMP(diff, tree_idx, STATUS_OK, "source tree");
// Если tree_idx == diff->forest.count, то в дереве разложение, а его оптимизацию можно не выводить
    if (tree_idx < diff->forest.count) {
        printTex(diff, 
            "\\subsection{Результат оптимизации}\n");
        printExpression(diff, tree_idx);
    }
}


static FoldStatus foldConstants(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff);
    
    if (!node)
        return FOLD_CONST;

    switch (node->type) {
        case NODE_OP: {
            FoldStatus left_res =  foldConstants(diff, NL, tree_idx);
            FoldStatus right_res = foldConstants(diff, NR, tree_idx);

            if ((left_res == FOLD_OPTIMIZED || left_res == FOLD_CONST) &&
                (right_res == FOLD_OPTIMIZED || right_res == FOLD_CONST)) {
                setNodeToNum(diff, node, tree_idx, evaluateNode(diff, node));
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


static bool simplifyOperations(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff);
    
    if (!node) {
        return false;
    }

    bool changed = false;
    if (simplifyOperations(diff, NL, tree_idx) ||
        simplifyOperations(diff, NR, tree_idx)) {
        changed = true;
    }
    if (node->type != NODE_OP) {
        return changed;
    }
    bool curr_changed = simplifyDispatcher(diff, node, tree_idx);    

    return changed || curr_changed;
}


static bool simplifyDispatcher(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node->type == NODE_OP); assert(node->value.op >= 0);
    assert(node->value.op < OP_MAX_COUNT);

    simplifierFunc function = SIMPLIFIERS[node->value.op];
    if (function) {
        return function(diff, node, tree_idx);
    }

    return false;
}


static bool simplifyAdd(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL)) {
        return setNodeToChild(diff, node, tree_idx, false);
    }
    if (ZERO(NR)) {
        return setNodeToChild(diff, node, tree_idx, true);
    }

    return false;
}


static bool simplifySub(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NR)) {
        return setNodeToChild(diff, node, tree_idx, true);
    }

    return false;
}


static bool simplifyMul(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL) || ZERO(NR)) {
        return setNodeToNum(diff, node, tree_idx, 0);
    }
    if (ONE(NL)) {
        return setNodeToChild(diff, node, tree_idx, false);
    }
    if (ONE(NR)) {
        return setNodeToChild(diff, node, tree_idx, true);
    }

    return false;
}


static bool simplifyDiv(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL)) {
        return setNodeToNum(diff, node, tree_idx, 0);
    }
    if (ONE(NR)) {
        return setNodeToChild(diff, node, tree_idx, true);
    }

    return false;
}


static bool simplifyPow(Differentiator* diff, TreeNode* node, size_t tree_idx)
{
    assert(diff); assert(node);

    if (ZERO(NL)) {
        return setNodeToNum(diff, node, tree_idx, 0);
    }
    if (ONE(NL) || ZERO(NR)) {
        return setNodeToNum(diff, node, tree_idx, 1);
    }
    if (ONE(NR)) {
        return setNodeToChild(diff, node, tree_idx, true);
    }

    return false;
}


static bool setNodeToChild(Differentiator* diff, TreeNode* node, size_t tree_idx, bool is_left)
{
    assert(diff); assert(node);

    diff->highlight_node = node;
    if (diff->tex_dump.print_steps) {
        printTex(diff, "\\begin{dmath*}\n"
            "%n = ", diff->forest.trees[tree_idx].root);
    }

    if (is_left) {
        deleteBranch(NR);
        NR = NULL;
        replaceWithChild(node, NL);
    } else {
        deleteBranch(NL);
        NL = NULL;
        replaceWithChild(node, NR);
    }

    if (diff->tex_dump.print_steps) {
    printTex(diff, "%n\n"
        "\\end{dmath*}\n\n", diff->forest.trees[tree_idx].root);
    }
    diff->highlight_node = NULL;

    return true;
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


static bool setNodeToNum(Differentiator* diff, TreeNode* node, size_t tree_idx, double num)
{
    assert(diff); assert(node);

    diff->highlight_node = node;
    if (diff->tex_dump.print_steps) {
        printTex(diff, "\\begin{dmath*}\n"
            "%n = ", diff->forest.trees[tree_idx].root);
    }

    deleteBranch(NL);
    NL = NULL;
    deleteBranch(NR);
    NR = NULL;
    node->type = NODE_NUM;
    node->value.num_val = num; 

    if (diff->tex_dump.print_steps) {
        printTex(diff, "%n\n"
            "\\end{dmath*}\n\n", diff->forest.trees[tree_idx].root);
    }
    diff->highlight_node = NULL;

    return true;
}


static bool isNum(TreeNode* node, double num)
{
    if (!node) {
        return false;
    }

    return node->type == NODE_NUM && (fabs(node->value.num_val - num) < EPS);
}