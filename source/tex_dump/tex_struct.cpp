#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>

#include "tex_dump/tex_struct.h"
#include "tex_dump/tex_expression.h"
#include "tex_dump/plot_generator.h"

#include "diff/diff_defs.h"
#include "diff/diff_evaluate.h"
#include "diff/diff.h"
#include "diff/diff_process.h"

#include "tree/tree.h"


static void printTitle(Differentiator* diff);
static void openTexDumpFile(Differentiator* diff);


static const char* TEX_DIRECTORY = "tex";
static const char* TEX_FILENAME = "tex/differentiation";


void texInit(Differentiator* diff)
{
    assert(diff); assert(diff->forest.trees);

    openTexDumpFile(diff);
    assert(TEX_FILE);

    printTex(diff, 
        "\\documentclass[12pt,a4paper]{extreport}\n"
        "\\input{%s/style}\n", TEX_DIRECTORY);

    printTitle(diff);

    printTex(diff, 
        "\\begin{document}\n"
        "\\maketitle\n"
        "\\tableofcontents\n"
        "\\newpage\n\n");
}


static void printTitle(Differentiator* diff)
{
    assert(diff);

    printTex(diff,
        "\\title{\n"
        "\\small{\\textsc{Институт системного дифференцирования и прикладной математики}}\\\\\n"
        "\\vspace{3cm}\n"
        "\\textbf{\\Huge Автоматический расчленитель функций}\\\\\n"
        "\\vspace{0.5cm}\n"
        "\\large{или как превратить красивую формулу в хаос из скобок}\\\\\n"
        "\\vspace{0.2cm}\n"
        "}\n");

    printTex(diff, "\\author{\n"
    "\\textbf{Главный архитектор:}\\\\\n"
    "sfinkymvp (повелитель деревьев)\\\\\n"
    "\\vspace{0.5cm}\n"
    "\\textbf{Моральная поддержка:}\\\\\n"
    "Отл 2 за тесты по бжд и 25 посещений\\\\\n"
    "\\vspace{0.5cm}\n"
    "\\textbf{Спонсор исследования:}\\\\\n"
    "Горящий дедлайн и сегфолт в 6 утра\n"
    "}\n");

    printTex(diff,
        "\\date{\\today\\\\\n\\small{(день, когда это наконец (не)заработало)}}\n"
    "}\n");
}


void printIntroduction(Differentiator* diff)
{
    assert(diff); assert(diff->forest.count != 0);

    printTex(diff, 
        "\\chapter{Введение}\n"
        "В данной научно-исследовательской работе была предпринята попытка "
        "обучить таракана с кухни основам математического анализа. "
        "В процессе обучения таракан превзошел недогадливых физтехов и смог "
        "зафачить их по теме непрерывности функции.\n\n");
   
    printTex(diff,
        "Приступим к изучению возможно самой сложной ранее невиданной функции, в каком то из измерений она выглядит так:\n");
    printExpression(diff, 0);

    printTex(diff,
        "Пока таракан фачил недогадливого физтеха и его друга всероса, он пробежался по столу и нарисовал "
        "какую-то непонятную линию:\n");
    printPlot(diff, 0);
}


void printPlot(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->forest.trees); assert(tree_idx < diff->forest.count);

    char output_file[BUFFER_SIZE] = "";
    snprintf(output_file, BUFFER_SIZE, "%s/%s_%03zu", GNUPLOT_IMAGES_DIRECTORY,
        GNUPLOT_OUTPUT_FILENAME, tree_idx);
    generatePlot(diff, output_file, 1, tree_idx);

    if (tree_idx == 0) {
        printTex(diff,
            "\\subsection{График функции}\n"
            "\\begin{figure}[H]\n"
            "\\centering\n"
            "\\includegraphics[width=0.8\\textwidth]{%s}\n"
            "\\caption{График функции}\n", output_file);
    } else {
        printTex(diff,
            "\\subsection{График производной}\n"
            "\\begin{figure}[H]\n"
            "\\centering\n"
            "\\includegraphics[width=0.8\\textwidth]{%s}\n"
            "\\caption{График %zu-й производной}\n", output_file, tree_idx);
    }
    printTex(diff, "\\end{figure}\n\n");
}


OperationStatus texClose(Differentiator* diff)
{
    assert(diff);

    printTex(diff, "\\end{document}\n");

    assert(fclose(TEX_FILE) == 0);
    TEX_FILE = NULL;

    char command[BUFFER_SIZE * 2] = {}; 
    snprintf(command, BUFFER_SIZE * 2, "xelatex -interaction=batchmode -output-directory=%s %s > /dev/null",
        TEX_DIRECTORY, diff->tex_dump.filename);
    int result = system(command);
    result = system(command);
    if (result != 0) {
        return STATUS_SYSTEM_CALL_ERROR;
    }

    snprintf(command, BUFFER_SIZE * 2, "rm %s.toc %s.log %s.aux %s.out", TEX_FILENAME,
        TEX_FILENAME, TEX_FILENAME, TEX_FILENAME);
    result = system(command);
    if (result != 0) {
        return STATUS_SYSTEM_CALL_ERROR;
    }
    
    return STATUS_OK;
}


static void openTexDumpFile(Differentiator* diff)
{
    assert(diff);

    snprintf(diff->tex_dump.filename, BUFFER_SIZE, "%s.tex", TEX_FILENAME);

    TEX_FILE = fopen(diff->tex_dump.filename, "w");
    assert(TEX_FILE);
}

