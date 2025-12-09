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

    if (node == diff->highlight_node) {
        printTex(diff, "{\\color{red} ");
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

    if (node == diff->highlight_node) {
        printTex(diff, "}");
    }
}


static void printOperator(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node); assert(node->type == NODE_OP);
    assert(node->value.op != OP_NONE); assert(node->right);

    switch (node->value.op) {
        case OP_ADD: printAdd(diff, node); break;
        case OP_SUB: printTex(diff, "%n - %n", node->left, node->right); break;
        case OP_MUL: printTex(diff, "%n \\cdot %n", node->left, node->right); break;
        case OP_DIV: printTex(diff, "\\frac{%n}{%n}", node->left, node->right); break;

        case OP_POW: printTex(diff, "{(%n)}^{%n}", node->left, node->right); break;
        case OP_LOG: printTex(diff, "\\log_{%n}{%n}", node->left, node->right); break;

        case OP_SIN: printTex(diff, "\\sin{(%n)}", node->right); break;
        case OP_COS: printTex(diff, "\\cos{(%n)}", node->right); break;
        case OP_TAN: printTex(diff, "\\tan{(%n)}", node->right); break;
        case OP_COT: printTex(diff, "\\cot{(%n)}", node->right); break;

        case OP_ASIN: printTex(diff, "\\arcsin{(%n)}", node->right); break;
        case OP_ACOS: printTex(diff, "\\arccos{(%n)}", node->right); break;
        case OP_ATAN: printTex(diff, "\\arctan{(%n)}", node->right); break;
        case OP_ACOT: printTex(diff, "\\arccot{(%n)}", node->right); break;

        case OP_SINH: printTex(diff, "\\sinh{(%n)}", node->right); break;
        case OP_COSH: printTex(diff, "\\cosh{(%n)}", node->right); break;
        case OP_TANH: printTex(diff, "\\tanh{(%n)}", node->right); break;
        case OP_COTH: printTex(diff, "\\coth{(%n)}", node->right); break;

        case OP_ASINH: printTex(diff, "\\operatorname{asinh}{(%n)}", node->right); break;
        case OP_ACOSH: printTex(diff, "\\operatorname{acosh}{(%n)}", node->right); break;
        case OP_ATANH: printTex(diff, "\\operatorname{atanh}{(%n)}", node->right); break;
        case OP_ACOTH: printTex(diff, "\\operatorname{acoth}{(%n)}", node->right); break;

        case OP_NONE: fprintf(stderr, "Error: OP_NONE detected in tex print\n"); break;
        default: fprintf(stderr, "Critical error: Unknown op type %d\n", node->value.op); break;
    }
}


static void printAdd(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(node);

    bool is_remainder = node->right && node->right->type == NODE_NUM &&
        fabs(node->right->value.num_val) < EPS;

    printTex(diff, "%n", node->left);
    if (is_remainder) {
        if (fabs(diff->args.taylor_info.center) < EPS) {
            printTex(diff, " + o(x^{%zu})", diff->args.derivative_info.order);
        } else {
            printTex(diff, " + o((x - %g)^{%zu})", diff->args.taylor_info.center,
                diff->args.derivative_info.order);
        }
    } else {
        printTex(diff, " + %n", node->right);
    }
}