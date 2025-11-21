#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "diff/diff_dsl.h"
#include "diff/diff.h"
#include "diff/diff_var_table.h"
#include "diff/diff_defs.h"
#include "tree/tree.h"
#include "status.h"


#define nL node->left
#define nR node->right


#define dL diffNode(nL, diff_var)
#define dR diffNode(nR, diff_var)
#define cL copyNode(nL)
#define cR copyNode(nR)


#define CNUM(val) createNum(val)


#define ADD(left, right) MakeAdd(left, right)
#define SUB(left, right) MakeSub(left, right)
#define MUL(left, right) MakeMul(left, right)
#define DIV(left, right) MakeDiv(left, right)
#define POW(left, right) MakePow(left, right)
#define SIN(arg) MakeSin(arg)
#define COS(arg) MakeCos(arg)


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

TreeNode* createNum(double value) {
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (node == NULL)
        return NULL;

    node->type = NODE_NUM;
    node->value.num_val = value;
    return node;
}


TreeNode* MakeAdd(TreeNode* left, TreeNode* right) 
{
    return createOp(OP_ADD, left, right);
}


TreeNode* MakeSub(TreeNode* left, TreeNode* right)
{   
    return createOp(OP_SUB, left, right);
}


TreeNode* MakeMul(TreeNode* left, TreeNode* right)
{
    return createOp(OP_MUL, left, right); 
}


TreeNode* MakeDiv(TreeNode* left, TreeNode* right)
{
    return createOp(OP_DIV, left, right);
}


TreeNode* MakePow(TreeNode* left, TreeNode* right)
{
    return createOp(OP_POW, left, right);
}


TreeNode* MakeSin(TreeNode* arg)
{
    return createOp(OP_SIN, NULL, arg);
}


TreeNode* MakeCos(TreeNode* arg) 
{
    return createOp(OP_COS, NULL, arg);
}


static TreeNode* diffOp(TreeNode* node, size_t diff_var)
{
    assert(node);

    switch (node->value.op) {
        case OP_ADD:  return ADD(dL, dR);
        case OP_SUB:  return SUB(dL, dR);
        case OP_MUL:  return ADD(MUL(dL, cR), MUL(cL, dR));
        case OP_DIV:  return DIV(SUB(MUL(dL, cR), MUL(cL, dR)),
                      POW(cR, CNUM(2)));
        case OP_POW:  return NULL;
        case OP_SIN:  return MUL(COS(cR), dR);
        case OP_COS:  return MUL(MUL(CNUM(-1), SIN(cR)), dR);
        case OP_NONE: return NULL;
        default:      return NULL;
    }
}


TreeNode* diffNode(TreeNode* node, size_t diff_var)
{
    assert(node);

    switch (node->type) {
        case NODE_NUM: return CNUM(0);
        case NODE_VAR:
            if (node->value.var_idx == diff_var)
                return CNUM(1);
            else
                return CNUM(0);
        case NODE_OP: return diffOp(node, diff_var);
        default: return NULL;
    }    
}


#undef nL
#undef nR

#undef dL
#undef dR
#undef cL
#undef cR

#undef CNUM

#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef POW
#undef SIN
#undef COS