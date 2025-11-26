#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "tex_dump/tex.h"

#include "diff/diff_defs.h"
#include "diff/diff_process.h"

#include "tree/tree.h"


static const char* TEX_DUMP_DIRECTORY = "tex";


void printOperator(Differentiator* diff, TreeNode* node)
{
    assert(diff); assert(diff->tex_dump.file); assert(node);
    assert(node->type == NODE_OP); assert(node->value.op != OP_NONE);
    assert(node->right);

    switch (node->value.op) {
        case OP_ADD:
            fprintf(diff->tex_dump.file, "(");
            printNode(diff, node->left);
            fprintf(diff->tex_dump.file, " + ");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, ")");
            break;
        case OP_SUB:
            printNode(diff, node->left);
            fprintf(diff->tex_dump.file, " - ");
            printNode(diff, node->right);
            break;
        case OP_MUL:
            printNode(diff, node->left);
            fprintf(diff->tex_dump.file, " \\cdot ");
            printNode(diff, node->right);
            break;
        case OP_DIV:
            fprintf(diff->tex_dump.file, " \\frac{");
            printNode(diff, node->left);
            fprintf(diff->tex_dump.file, "}{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        
        case OP_POW:
            fprintf(diff->tex_dump.file, "\\left( ");
            printNode(diff, node->left);
            fprintf(diff->tex_dump.file, "\\right) ^{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_LOG:
            fprintf(diff->tex_dump.file, "\\log_{");
            printNode(diff, node->left);
            fprintf(diff->tex_dump.file, "}{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        
        case OP_SIN:
            fprintf(diff->tex_dump.file, "\\sin{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_COS:
            fprintf(diff->tex_dump.file, "\\cos{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_TAN:
            fprintf(diff->tex_dump.file, "\\tan{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_COT:
            fprintf(diff->tex_dump.file, "\\cot{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;

        case OP_ASIN:
            fprintf(diff->tex_dump.file, "\\arcsin{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_ACOS:
            fprintf(diff->tex_dump.file, "\\arccos{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_ATAN:
            fprintf(diff->tex_dump.file, "\\arctan{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_ACOT:
            fprintf(diff->tex_dump.file, "\\arccot{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;

        case OP_SINH:
            fprintf(diff->tex_dump.file, "\\sinh{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_COSH:
            fprintf(diff->tex_dump.file, "\\cosh{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_TANH:
            fprintf(diff->tex_dump.file, "\\tanh{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_COTH:
            fprintf(diff->tex_dump.file, "\\coth{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;

        case OP_ASINH:
            fprintf(diff->tex_dump.file, "\\operatorname{asinh}{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_ACOSH:
            fprintf(diff->tex_dump.file, "\\operatorname{acosh}{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_ATANH:
            fprintf(diff->tex_dump.file, "\\operatorname{atanh}{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
            break;
        case OP_ACOTH:
            fprintf(diff->tex_dump.file, "\\operatorname{acoth}{");
            printNode(diff, node->right);
            fprintf(diff->tex_dump.file, "} ");
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
    assert(diff); assert(diff->tex_dump.file);
    assert(diff->var_table.variables); assert(node);

    switch (node->type) {
        case NODE_OP:
            printOperator(diff, node);
            break;
        case NODE_VAR:
            fprintf(diff->tex_dump.file, "%s", diff->var_table.variables[node->value.var_idx].name);
            break;
        case NODE_NUM:
            fprintf(diff->tex_dump.file, "%g", node->value.num_val);
            break;
        default:
            fprintf(stderr, "Critical error: unknown node type %d\n", node->type);
            break;
    }
}


void printPaintedTree(Differentiator* diff, TreeNode* node, TreeNode* color_node)
{
    assert(diff); assert(diff->tex_dump.file); assert(diff->var_table.variables);
    assert(node); assert(color_node);

//    if (node == color_node)
        fprintf(diff->tex_dump.file, "{\\color{red} ");

    printf("node: %p, color_node: %p\n", node, color_node);
    switch (node->type) {
        case NODE_OP:
            printOperator(diff, node);
            break;
        case NODE_VAR:
            fprintf(diff->tex_dump.file, "%s", diff->var_table.variables[node->value.var_idx].name);
            break;
        case NODE_NUM:
            fprintf(diff->tex_dump.file, "%g", node->value.num_val);
            break;
        default:
            fprintf(stderr, "Critical error: unknown node type %d\n", node->type);
            break;
    }

 //   if (node == color_node) 
        fprintf(diff->tex_dump.file, "} ");
}


void printExpression(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->tex_dump.file); assert(diff->forest.trees);
    assert(tree_idx <= diff->forest.count);

    fprintf(diff->tex_dump.file, "\\begin{align}\n");
    printNode(diff, diff->forest.trees[tree_idx].root);
    fprintf(diff->tex_dump.file, "\n\\end{align}\n\n");
}


TreeNode* printAndDifferentiate(Differentiator* diff, TreeNode* node, size_t var_idx)
{
    assert(diff); assert(diff->forest.trees); assert(diff->tex_dump.file); assert(node); 

    fprintf(diff->tex_dump.file, "\\text{Differentiation:}\n");
    fprintf(diff->tex_dump.file, "\\begin{align}\n");
    fprintf(diff->tex_dump.file, "\\left( ");
    printNode(diff, node);
    fprintf(diff->tex_dump.file, " \\right)' = ");

    TreeNode* new_node = diffOp(diff, node, var_idx);
    printNode(diff, new_node);

    fprintf(diff->tex_dump.file, "\\end{align}\n\n");
    return new_node;
}


static void openTexDumpFile(Differentiator* diff)
{
    assert(diff);

    snprintf(diff->tex_dump.filename, BUFFER_SIZE, "%s/main.tex",
             TEX_DUMP_DIRECTORY);

    diff->tex_dump.file = fopen(diff->tex_dump.filename, "w");
    assert(diff->tex_dump.file);
}


void texInit(Differentiator* diff)
{
    assert(diff);

    openTexDumpFile(diff);
    assert(diff->tex_dump.file);

    fprintf(diff->tex_dump.file,
        "\\documentclass[14pt,a4paper]{extreport}\n"
        "\\input{%s/style}\n", TEX_DUMP_DIRECTORY);

    printTitle(diff);

    fprintf(diff->tex_dump.file,
        "\\begin{document}\n"
        "\\maketitle\n"
        "\\tableofcontents\n"
        "\\newpage\n\n");
}


void texClose(Differentiator* diff)
{
    assert(diff); assert(diff->tex_dump.file);

    fprintf(diff->tex_dump.file, "\\end{document}\n");

    assert(fclose(diff->tex_dump.file) == 0);
    diff->tex_dump.file = NULL;

    char command[BUFFER_SIZE * 2] = {};
    snprintf(command, BUFFER_SIZE * 2, "xelatex -interaction=batchmode %s", diff->tex_dump.filename);
    system(command);
}


void printTitle(Differentiator* diff)
{
    assert(diff); assert(diff->tex_dump.file);

    fprintf(diff->tex_dump.file,
        "\\title{Дифференцирование n раз дифференцируемой "
        "в точке пересечения матана и алгема сложной непрерывной функции}\n"
        "\\author{Сгенерировано программой студента Московского Физико-Технического Университета}\n"
        "\\date{Документ сгенерирован: \\today}\n");
}


void printIntroduction(Differentiator* diff)
{
    assert(diff); assert(diff->tex_dump.file); assert(diff->forest.count != 0);

    fprintf(diff->tex_dump.file,
        "\\chapter{Введение}\n"
        "В этой дипломной работе студентом честно предоставлено вручную \n"
        "написанное исследование функции. Далее вы можете увидеть исходную функцию:\n");
    
    printExpression(diff, 0);

    fprintf(diff->tex_dump.file,
        "\\chapter{Ход вычислений}\n");
}