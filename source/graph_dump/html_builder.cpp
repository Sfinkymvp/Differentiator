#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "graph_dump/html_builder.h"
#include "graph_dump/graph_generator.h"

#include "diff/diff_defs.h"

#include "status.h"

#include "tree/tree.h"


#define GRAPH_FILE diff->graph_dump.file
#define DIRECTORY  diff->graph_dump.directory


static const char* GRAPH_DUMP_DIRECTORY = "images";


typedef struct {
    OperationStatus status;
    const char* message;
    const char* file;
    const char* function;
    int line;
} DumpInfo;


static void createHtmlDump(Differentiator* diff, BinaryTree* tree, DumpInfo* info, const char* image);
static void writeTreeInfo(Differentiator* diff, BinaryTree* tree, DumpInfo* info);
static void convertDotToSvg(const char* dot_file, const char* svg_file);


void openGraphDumpFile(Differentiator* diff)
{
    assert(diff);

    static int dump_counter = 1; 
    snprintf(DIRECTORY, BUFFER_SIZE, "%s/tree_dump_%03d",
        GRAPH_DUMP_DIRECTORY, dump_counter);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm -rf %s && mkdir -p %s",
             DIRECTORY, DIRECTORY);
    system(command);

    char filename[BUFFER_SIZE * 2] = {};
    snprintf(filename, BUFFER_SIZE * 2, "%s/tree_dump_%03d.html",
             DIRECTORY, dump_counter);

    GRAPH_FILE = fopen(filename, "w");
    assert(GRAPH_FILE);
    dump_counter++;
}


void treeDump(Differentiator* diff, size_t tree_idx, OperationStatus status, const char* file, 
              const char* function, int line, const char* format, ...)
{
    assert(diff); assert(GRAPH_FILE); assert(diff->var_table.variables);
    assert(diff->forest.trees); assert(tree_idx <= diff->forest.count);
    assert(file); assert(function); assert(format);

    char message[BUFFER_SIZE] = {};
    if (format[0] != '\0') {
        va_list args;
        va_start(args, format);
        vsnprintf(message, BUFFER_SIZE, format, args);
        va_end(args);
    }
 
    DumpInfo info = {status, message, file, function, line};
    char graph_dot_file[BUFFER_SIZE * 2] = {};
    char graph_svg_file[BUFFER_SIZE * 2] = {};

    if (diff->tex_dump.print_steps) {
        snprintf(graph_dot_file, BUFFER_SIZE * 2, "%s/tree_graph_%03zu.dot",
                DIRECTORY, diff->graph_dump.image_counter);
        snprintf(graph_svg_file, BUFFER_SIZE * 2, "%s/tree_graph_%03zu.svg",
                DIRECTORY, diff->graph_dump.image_counter);

        generateGraph(diff, tree_idx, graph_dot_file);
        convertDotToSvg(graph_dot_file, graph_svg_file);

        char command[BUFFER_SIZE * 3] = {};
        snprintf(command, BUFFER_SIZE * 3, "rm %s", graph_dot_file);
        system(command);
    }

    snprintf(graph_svg_file, BUFFER_SIZE * 2, "tree_graph_%03zu.svg", diff->graph_dump.image_counter);
    createHtmlDump(diff, &diff->forest.trees[tree_idx], &info, graph_svg_file);

    diff->graph_dump.image_counter++;
}


static void createHtmlDump(Differentiator* diff, BinaryTree* tree, DumpInfo* info, const char* image)
{
    assert(diff); assert(tree); assert(info); assert(info); assert(image);

    fprintf(GRAPH_FILE, "<html>\n");
    fprintf(GRAPH_FILE, "<style>\n");
    fprintf(GRAPH_FILE, "body {font-family: monospace;}\n");
    fprintf(GRAPH_FILE, "</style>\n");
    fprintf(GRAPH_FILE, "<body>\n");

    writeTreeInfo(diff, tree, info);

    fprintf(GRAPH_FILE, "<div style=\"overflow-x: auto; white-space: nowrap;\">\n");
    if (diff->tex_dump.print_steps) {
        fprintf(GRAPH_FILE, "<img src=\"%s\" "
            "style=\"zoom:0.65; -moz-transform:scale(0.1); -moz-transform-origin:top left;\">\n",
            image);
    }
    fprintf(GRAPH_FILE, "</div>\n");

    fprintf(GRAPH_FILE, "<hr style=\"margin: 40px 0; border: 2px solid #ccc;\">\n");
    fprintf(GRAPH_FILE, "</body>\n");
    fprintf(GRAPH_FILE, "</html>\n");
}


static void writeTreeInfo(Differentiator* diff, BinaryTree* tree, DumpInfo* info)
{
    assert(diff); assert(diff->forest.trees); assert(tree);
    assert(tree->origin.name); assert(tree->origin.file); assert(tree->origin.function);
    assert(info); assert(info->message); assert(info->file); assert(info->function); 
    assert(info->status < ERROR_COUNT);

    fprintf(GRAPH_FILE, "\t<h1>TREE DUMP #%03zu</h1>\n", diff->graph_dump.image_counter);
    fprintf(GRAPH_FILE, "\t<h2>Dump {%s:%d} called from %s()</h2>\n",
        info->file, info->line, info->function);
    fprintf(GRAPH_FILE, "\t<h2>Tree \"%s\" {%s:%d} created in %s()</h2>\n",
        tree->origin.name, tree->origin.file,
        tree->origin.line, tree->origin.function);
    fprintf(GRAPH_FILE, "\t<h3>STATUS: %s </h3>\n", ErrorTable[info->status].status_string);
    fprintf(GRAPH_FILE, "\t<h3>MESSAGE: %s</h3>\n", info->message);
}


static void convertDotToSvg(const char* dot_file, const char* svg_file)
{
    assert(dot_file);
    assert(svg_file);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tsvg %s -o %s",
             dot_file, svg_file);

    system(command);
}