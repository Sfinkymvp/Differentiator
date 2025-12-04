#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>

#include "tex_dump/tex.h"
#include "tex_dump/plot_generator.h"

#include "diff/diff_defs.h"
#include "diff/diff_evaluate.h"
#include "diff/diff.h"
#include "diff/diff_process.h"

#include "tree/tree.h"


static const char* TEX_DUMP_DIRECTORY = "tex";


void printOperator(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(TEX_FILE); assert(node);
    assert(node->type == NODE_OP); assert(node->value.op != OP_NONE);
    assert(node->right);

    switch (node->value.op) {
        case OP_ADD: printTex(diff, "%n + %n", node->left, node->right); break;
        case OP_SUB: printTex(diff, "%n - %n", node->left, node->right); break;
        case OP_MUL: printTex(diff, "%n \\cdot %n", node->left, node->right); break;
        case OP_DIV: printTex(diff, "\\frac{%n}{%n}", node->left, node->right); break;
        case OP_POW: printTex(diff, "%n^{%n}", node->left, node->right); break;
        case OP_LOG: printTex(diff, "\\log_{%n}{%n}", node->left, node->right); break;
        case OP_SIN:
            fprintf(TEX_FILE, "\\sin{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COS:
            fprintf(TEX_FILE, "\\cos{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_TAN:
            fprintf(TEX_FILE, "\\tan{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COT:
            fprintf(TEX_FILE, "\\cot{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;

        case OP_ASIN:
            fprintf(TEX_FILE, "\\arcsin{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOS:
            fprintf(TEX_FILE, "\\arccos{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ATAN:
            fprintf(TEX_FILE, "\\arctan{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOT:
            fprintf(TEX_FILE, "\\arccot{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;

        case OP_SINH:
            fprintf(TEX_FILE, "\\sinh{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COSH:
            fprintf(TEX_FILE, "\\cosh{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_TANH:
            fprintf(TEX_FILE, "\\tanh{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COTH:
            fprintf(TEX_FILE, "\\coth{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;

        case OP_ASINH:
            fprintf(TEX_FILE, "\\operatorname{asinh}{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOSH:
            fprintf(TEX_FILE, "\\operatorname{acosh}{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ATANH:
            fprintf(TEX_FILE, "\\operatorname{atanh}{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOTH:
            fprintf(TEX_FILE, "\\operatorname{acoth}{");
            printNode(diff, node->right);
            fprintf(TEX_FILE, "} ");
            break;
    
        case OP_NONE:
            fprintf(stderr, "Error: OP_NONE detected in tex print\n");
            break;
        default:
            fprintf(stderr, "Critical error: Unknown op type %d\n", node->value.op);
            break;
    }
}


void printNode(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(TEX_FILE);
    assert(diff->var_table.variables); assert(node);

    switch (node->type) {
        case NODE_OP:
            printOperator(diff, node);
            break;
        case NODE_VAR:
            fprintf(TEX_FILE, "%s", diff->var_table.variables[node->value.var_idx].name);
            break;
        case NODE_NUM:
            fprintf(TEX_FILE, "%g", node->value.num_val);
            break;
        default:
            fprintf(stderr, "Critical error: unknown node type %d\n", node->type);
            break;
    }
}


void printPaintedOperator(Differentiator* diff, TreeNode* node, TreeNode* color_node)
{
    assert(diff); assert(TEX_FILE); assert(node);
    assert(node->type == NODE_OP); assert(node->value.op != OP_NONE);
    assert(node->right); assert(color_node);

    switch (node->value.op) {
        case OP_ADD:
            fprintf(TEX_FILE, "(");
            printPaintedNode(diff, node->left, color_node);
            fprintf(TEX_FILE, " + ");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, ")");
            break;
        case OP_SUB:
            printPaintedNode(diff, node->left, color_node);
            fprintf(TEX_FILE, " - ");
            printPaintedNode(diff, node->right, color_node);
            break;
        case OP_MUL:
            printPaintedNode(diff, node->left, color_node);
            fprintf(TEX_FILE, " \\cdot ");
            printPaintedNode(diff, node->right, color_node);
            break;
        case OP_DIV:
            fprintf(TEX_FILE, " \\frac{");
            printPaintedNode(diff, node->left, color_node);
            fprintf(TEX_FILE, "}{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        
        case OP_POW:
            fprintf(TEX_FILE, "\\left( ");
            printPaintedNode(diff, node->left, color_node);
            fprintf(TEX_FILE, "\\right) ^{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_LOG:
            fprintf(TEX_FILE, "\\log_{");
            printPaintedNode(diff, node->left, color_node);
            fprintf(TEX_FILE, "}{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        
        case OP_SIN:
            fprintf(TEX_FILE, "\\sin{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COS:
            fprintf(TEX_FILE, "\\cos{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_TAN:
            fprintf(TEX_FILE, "\\tan{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COT:
            fprintf(TEX_FILE, "\\cot{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;

        case OP_ASIN:
            fprintf(TEX_FILE, "\\arcsin{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOS:
            fprintf(TEX_FILE, "\\arccos{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ATAN:
            fprintf(TEX_FILE, "\\arctan{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOT:
            fprintf(TEX_FILE, "\\arccot{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;

        case OP_SINH:
            fprintf(TEX_FILE, "\\sinh{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COSH:
            fprintf(TEX_FILE, "\\cosh{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_TANH:
            fprintf(TEX_FILE, "\\tanh{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_COTH:
            fprintf(TEX_FILE, "\\coth{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;

        case OP_ASINH:
            fprintf(TEX_FILE, "\\operatorname{asinh}{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOSH:
            fprintf(TEX_FILE, "\\operatorname{acosh}{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ATANH:
            fprintf(TEX_FILE, "\\operatorname{atanh}{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
        case OP_ACOTH:
            fprintf(TEX_FILE, "\\operatorname{acoth}{");
            printPaintedNode(diff, node->right, color_node);
            fprintf(TEX_FILE, "} ");
            break;
    
        case OP_NONE:
            fprintf(stderr, "Error: OP_NONE detected in tex print\n");
            break;
        default:
            fprintf(stderr, "Critical error: Unknown op type %d\n", node->value.op);
            break;
    }
}


void printPaintedNode(Differentiator* diff, TreeNode* node, TreeNode* color_node)
{
    assert(diff); assert(TEX_FILE); assert(diff->var_table.variables);
    assert(node); assert(color_node);

    if (node == color_node)
        fprintf(TEX_FILE, "{\\color{red} ");

    switch (node->type) {
        case NODE_OP:
            printPaintedOperator(diff, node, color_node);
            break;
        case NODE_VAR:
            fprintf(TEX_FILE, "%s", diff->var_table.variables[node->value.var_idx].name);
            break;
        case NODE_NUM:
            fprintf(TEX_FILE, "%g", node->value.num_val);
            break;
        default:
            fprintf(stderr, "Critical error: unknown node type %d\n", node->type);
            break;
    }

    if (node == color_node) 
        fprintf(TEX_FILE, "} ");
}


void printExpression(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(TEX_FILE); assert(diff->forest.trees);
    assert(tree_idx <= diff->forest.count);

    fprintf(TEX_FILE, "\\begin{dmath*}\n");
    printNode(diff, diff->forest.trees[tree_idx].root);
    fprintf(TEX_FILE, "\n\\end{dmath*}\n\n");
}


void printPlot(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->forest.trees); assert(tree_idx < diff->forest.count);

    char output_file[BUFFER_SIZE] = "";
    snprintf(output_file, BUFFER_SIZE, "%s/%s_%03zu", GNUPLOT_IMAGES_DIRECTORY,
        GNUPLOT_OUTPUT_FILENAME, tree_idx);
    generatePlot(diff, diff->forest.trees[tree_idx].root, output_file, tree_idx, false);

    fprintf(TEX_FILE, "\\subsection{График производной}\n");
    fprintf(TEX_FILE, "\\begin{figure}[H]\n");
    fprintf(TEX_FILE, "\\centering\n");
    fprintf(TEX_FILE, "\\includegraphics[width=0.8\\textwidth]{%s}\n", output_file);
    fprintf(TEX_FILE, "\\caption{График %zu-й производной}\n", tree_idx);
    fprintf(TEX_FILE, "\\end{figure}\n\n");
}


static void openTexDumpFile(Differentiator* diff)
{
    assert(diff);

    snprintf(diff->tex_dump.filename, BUFFER_SIZE, "%s/main.tex",
             TEX_DUMP_DIRECTORY);

    TEX_FILE = fopen(diff->tex_dump.filename, "w");
    assert(TEX_FILE);
}


void texInit(Differentiator* diff)
{
    assert(diff);

    openTexDumpFile(diff);
    assert(TEX_FILE);

    fprintf(TEX_FILE,
        "\\documentclass[12pt,a4paper]{extreport}\n"
        "\\input{%s/style}\n", TEX_DUMP_DIRECTORY);

    printTitle(diff);

    fprintf(TEX_FILE,
        "\\begin{document}\n"
        "\\maketitle\n"
        "\\tableofcontents\n"
        "\\newpage\n\n");
}


void texClose(Differentiator* diff)
{
    assert(diff); assert(TEX_FILE);

    fprintf(TEX_FILE, "\\end{document}\n");

    assert(fclose(TEX_FILE) == 0);
    TEX_FILE = NULL;

    char command[BUFFER_SIZE * 2] = {};
    snprintf(command, BUFFER_SIZE * 2, "xelatex -interaction=batchmode %s > /dev/null", diff->tex_dump.filename);
    system(command);
}


void printTex(Differentiator* diff, const char* format, ...)
{
    assert(diff); assert(format);

    va_list args = {};
    va_start(args, format);

    const char* left_spec_ptr = strchr(format, '%');
    if (left_spec_ptr == NULL) {
        fprintf(TEX_FILE, "%s", format);
        return;
    }
    fprintf(TEX_FILE, "%.*s", (int)(left_spec_ptr - format), format);

    const char* right_spec_ptr = strchr(left_spec_ptr + 1, '%');
    while (right_spec_ptr) {
        TreeNode* node = va_arg(args, TreeNode*);
        assert(node);

        printNode(diff, node);
        fprintf(TEX_FILE, "%.*s", (int)(right_spec_ptr - left_spec_ptr - 2), left_spec_ptr + 2);

        const char* temp = strchr(right_spec_ptr + 1, '%');
        left_spec_ptr = right_spec_ptr;
        right_spec_ptr = temp;
    }

    printNode(diff, va_arg(args, TreeNode*));
    fprintf(TEX_FILE, "%s", left_spec_ptr + 2);
    va_end(args);
}


void printTitle(Differentiator* diff)
{
    assert(diff); assert(TEX_FILE);

    fprintf(TEX_FILE,
        "\\title{Дифференцирование n раз дифференцируемой "
        "в точке пересечения матана и алгема сложной непрерывной функции}\n"
        "\\author{Затехано вручную sfinkymvp}\n"
        "\\date{Документ сгенерирован: \\today}\n");
}


void printIntroduction(Differentiator* diff)
{
    assert(diff); assert(TEX_FILE); assert(diff->forest.count != 0);

    fprintf(TEX_FILE,
        "\\chapter{Введение}\n"
        "В этой дипломной работе студентом честно предоставлено вручную \n"
        "написанное исследование функции. Далее вы можете увидеть исходную функцию:\n");
    
    printExpression(diff, 0);

    printPlot(diff, 0);
}