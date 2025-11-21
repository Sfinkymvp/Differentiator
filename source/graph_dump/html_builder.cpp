#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>


#include "graph_dump/html_builder.h"
#include "graph_dump/graph_generator.h"
#include "graph_dump/graph_defs.h"
#include "diff/diff_defs.h"
#include "tree/tree.h"


static const char* DUMP_DIRECTORY = "images";


static void convertDotToSvg(const char* dot_file, const char* svg_file)
{
    assert(dot_file);
    assert(svg_file);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tsvg %s -o %s",
             dot_file, svg_file);

    system(command);
}


static void writeTreeInfo(Differentiator* diff, BinaryTree* tree, DumpInfo* info)
{
    assert(diff); assert(diff->forest.trees); assert(tree); /*assert(tree->identifier);*/
    assert(tree->origin.name); assert(tree->origin.file); assert(tree->origin.function);
    assert(info); assert(info->message); assert(info->file); assert(info->function); 

    fprintf(diff->dump_state.dump_file, "\t<h1>TREE DUMP #%03d</h1>\n", diff->dump_state.image_counter);
    fprintf(diff->dump_state.dump_file, "\t<h2>Dump {%s:%d} called from %s()</h2>\n",
            info->file, info->line, info->function);
    fprintf(diff->dump_state.dump_file, "\t<h2>Tree \"%s\" {%s:%d} created in %s()</h2>\n",
            tree->origin.name, tree->origin.file,
            tree->origin.line, tree->origin.function);
    fprintf(diff->dump_state.dump_file, "\t<h3>STATUS:   NONE</h3>\n");
    fprintf(diff->dump_state.dump_file, "\t<h3>MESSAGE: %s</h3>\n", info->message);
}


static void createHtmlDump(Differentiator* diff, BinaryTree* tree, DumpInfo* info, const char* image)
{
    assert(diff); assert(tree); assert(info); assert(info); assert(image);

    fprintf(diff->dump_state.dump_file, "<html>\n");
    fprintf(diff->dump_state.dump_file, "<style>\n");
    fprintf(diff->dump_state.dump_file, "body {font-family: monospace;}\n");
    fprintf(diff->dump_state.dump_file, "</style>\n");
    fprintf(diff->dump_state.dump_file, "<body>\n");

    writeTreeInfo(diff, tree, info);

    fprintf(diff->dump_state.dump_file, "<div style=\"overflow-x: auto; white-space: nowrap;\">\n");
    fprintf(diff->dump_state.dump_file, "<img src=\"tree_graph_%03d.svg\" "
            "style=\"zoom:0.65; -moz-transform:scale(0.1); -moz-transform-origin:top left;\">\n",
            diff->dump_state.image_counter);
    fprintf(diff->dump_state.dump_file, "</div>\n");

    fprintf(diff->dump_state.dump_file, "<hr style=\"margin: 40px 0; border: 2px solid #ccc;\">\n");
    fprintf(diff->dump_state.dump_file, "</body>\n");
    fprintf(diff->dump_state.dump_file, "</html>\n");
}


void treeDump(Differentiator* diff, BinaryTree* tree, OperationStatus status, const char* file, 
              const char* function, int line, const char* format, ...)
{
    assert(diff); assert(tree); assert(diff->dump_state.dump_file);
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

    snprintf(graph_dot_file, BUFFER_SIZE * 2, "%s/tree_graph_%03d.dot",
             diff->dump_state.directory, diff->dump_state.image_counter);
    snprintf(graph_svg_file, BUFFER_SIZE * 2, "%s/tree_graph_%03d.svg",
             diff->dump_state.directory, diff->dump_state.image_counter);

    generateGraph(diff, tree, graph_dot_file);
    convertDotToSvg(graph_dot_file, graph_svg_file);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm %s", graph_dot_file);
    system(command);

    createHtmlDump(diff, tree, &info, graph_svg_file);

    diff->dump_state.image_counter++;
}


void openDumpFile(Differentiator* diff)
{
    assert(diff);

    static int dump_counter = 1; 

    snprintf(diff->dump_state.directory, BUFFER_SIZE, "%s/tree_dump_%03d",
             DUMP_DIRECTORY, dump_counter);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm -rf %s && mkdir -p %s",
             diff->dump_state.directory, diff->dump_state.directory);
    system(command);

    char filename[BUFFER_SIZE * 2] = {};
    snprintf(filename, BUFFER_SIZE * 2, "%s/tree_dump_%03d.html",
             diff->dump_state.directory, dump_counter);

    diff->dump_state.dump_file = fopen(filename, "w");
    assert(diff->dump_state.dump_file);

    dump_counter++;
}