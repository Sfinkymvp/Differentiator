#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>

#include "tex_dump/tex_expression.h"

#include "diff/diff_defs.h"


static void printNode(Differentiator* diff, TreeNode* node);
static void printOperator(Differentiator* diff, TreeNode* node);
static void printAdd(Differentiator* diff, TreeNode* node);

static bool needParentheses(TreeNode* node);
static bool isBinaryOperator(OpType op);
static size_t getOperatorPriority(TreeNode* node);


void printExpression(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->forest.trees); assert(tree_idx <= diff->forest.count);

    printTex(diff, "\\begin{dmath*}\n%n\n\\end{dmath*}\n\n", diff->forest.trees[tree_idx].root);
}


void printTex(Differentiator* diff, const char* format, ...)
{
    assert(diff); assert(diff->tex_dump.file); assert(format);

    va_list args = {};
    va_start(args, format);
    
    const char* current = format;
    const char* next = strchr(current, '%');
    if (next == NULL) {
        vfprintf(TEX_FILE, current, args);
        va_end(args);
        return;
    }
    fprintf(TEX_FILE, "%.*s", (int)(next - current), current);
    do {
        const char* temp = strchr(next + 1, '%');
        current = next;
        next = temp;

        if (current[1] == 'n') {
            TreeNode* node = va_arg(args, TreeNode*);
            assert(node);
            assert(node->type >= 0);
            printNode(diff, node);
            if (next) {
                fprintf(TEX_FILE, "%.*s", (int)(next - current - 2), current + 2);
            } else {
                fprintf(TEX_FILE, "%s", current + 2);
            }
        } else {
            vfprintf(TEX_FILE, current, args);
            break;
        }
    } while (next);

    va_end(args);
}


static void printNode(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(diff->var_table.variables); assert(node);

    bool need_parentheses = needParentheses(node);

    if (node == diff->highlight_node) {
        printTex(diff, "{\\color{red} ");
    }
    if (need_parentheses) {
        printTex(diff, "(");
    }

    switch (node->type) {
        case NODE_OP: {
            printOperator(diff, node);
            break;
        }
        case NODE_VAR: {
            printTex(diff, "%s", diff->var_table.variables[node->value.var_idx].name);
            break;
        }
        case NODE_NUM: {
            printTex(diff, "%g", node->value.num_val);
            break;
        }
        default: {
            fprintf(stderr, "Critical error: unknown node type %d\n", node->type);
            break;
        }
    }

    if (need_parentheses) {
        printTex(diff, ")");
    }
    if (node == diff->highlight_node) {
        printTex(diff, "}");
    }
}


static void printOperator(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); assert(node->type == NODE_OP);
    assert(node->value.op != OP_NONE);

    switch (node->value.op) {
        case OP_ADD: printAdd(diff, node); break;
        case OP_SUB: printTex(diff, "%n - %n", node->left, node->right); break;
        case OP_MUL: printTex(diff, "%n \\cdot %n", node->left, node->right); break;
        case OP_DIV: printTex(diff, "\\frac{%n}{%n}", node->left, node->right); break;

        case OP_POW: printTex(diff, "{%n}^{%n}", node->left, node->right); break;
        case OP_LOG: printTex(diff, "\\log_{%n}{%n}", node->left, node->right); break;

        case OP_SIN: printTex(diff, "\\sin{%n}", node->right); break;
        case OP_COS: printTex(diff, "\\cos{%n}", node->right); break;
        case OP_TAN: printTex(diff, "\\tan{%n}", node->right); break;
        case OP_COT: printTex(diff, "\\cot{%n}", node->right); break;

        case OP_ASIN: printTex(diff, "\\arcsin{%n}", node->right); break;
        case OP_ACOS: printTex(diff, "\\arccos{%n}", node->right); break;
        case OP_ATAN: printTex(diff, "\\arctan{%n}", node->right); break;
        case OP_ACOT: printTex(diff, "\\arccot{%n}", node->right); break;

        case OP_SINH: printTex(diff, "\\sinh{%n}", node->right); break;
        case OP_COSH: printTex(diff, "\\cosh{%n}", node->right); break;
        case OP_TANH: printTex(diff, "\\tanh{%n}", node->right); break;
        case OP_COTH: printTex(diff, "\\coth{%n}", node->right); break;

        case OP_ASINH: printTex(diff, "\\operatorname{asinh}{%n}", node->right); break;
        case OP_ACOSH: printTex(diff, "\\operatorname{acosh}{%n}", node->right); break;
        case OP_ATANH: printTex(diff, "\\operatorname{atanh}{%n}", node->right); break;
        case OP_ACOTH: printTex(diff, "\\operatorname{acoth}{%n}", node->right); break;

        case OP_NONE: fprintf(stderr, "Error: OP_NONE detected in tex print\n"); break;
        default: fprintf(stderr, "Critical error: Unknown op type %d\n", node->value.op); break;
    }
}


static void printAdd(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); assert(diff->var_table.count != 0);

    bool is_remainder = node->right == NULL;

    size_t diff_var_idx = diff->args.derivative_info.diff_var_idx;
    printTex(diff, "%n", node->left);
    if (is_remainder) {
        if (fabs(diff->args.taylor_info.center) < EPS) {
            printTex(diff, " + o(%s^{%zu})", diff->var_table.variables[diff_var_idx],
                diff->args.derivative_info.order);
        } else {
            printTex(diff, " + o((%s - %g)^{%zu})", diff->var_table.variables[diff_var_idx],
                diff->args.taylor_info.center, diff->args.derivative_info.order);
        }
    } else {
        printTex(diff, " + %n", node->right);
    }
}


static bool needParentheses(TreeNode* node)
{
    if (node == NULL || node->parent == NULL) {
        return false;
    }

    if (node->type == NODE_VAR) {
        return false;
    }

    if (node->type == NODE_NUM) {
        return node->value.num_val < 0;
    }

    OpType parent_op = node->parent->value.op;
    OpType node_op = node->value.op;
    if (!isBinaryOperator(parent_op)) {
        if (node->type == NODE_OP && isBinaryOperator(node_op)) {
            return true;
        }
        return false;
    }

    size_t node_priority = getOperatorPriority(node);
    size_t parent_priority = getOperatorPriority(node->parent);
    
    if (parent_priority > node_priority) {
        return true;
    }
    if (parent_priority < node_priority) {
        return false;
    }

    if (parent_op == OP_SUB) {
        if (node->parent->right == node) {
            return true;
        }
    }
    if (parent_op == OP_DIV) {
        if (node->parent->right == node) {
            return true;
        }
    }
    if (parent_op == OP_POW) {
        if (node->parent->left == node) {
            return true;
        }
    }

    return false;
}


static bool isBinaryOperator(OpType op)
{
    switch (op) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_POW:
        case OP_LOG: return true;
        default:     return false;
    }
}


static size_t getOperatorPriority(TreeNode* node)
{
    assert(node);
    
    OpType op = node->value.op;
    if (op == OP_ADD || op == OP_SUB) {
        return 1;
    } else if (op == OP_MUL || op == OP_DIV) {
        return 2;
    } else if (op == OP_POW) {
        return 3;
    } else {
        return 4;
    }
}

