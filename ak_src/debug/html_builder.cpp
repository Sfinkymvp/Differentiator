#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>


#include "debug/html_builder.h"
#include "debug/graph_generator.h"
#include "akinator/tree.h"


const char* DUMP_DIRECTORY = "images";


static void convertDotToSvg(const char* dot_file, const char* svg_file)
{
    assert(dot_file);
    assert(svg_file);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tsvg %s -o %s",
             dot_file, svg_file);

    system(command);
}


static void writeTreeInfo(BinaryTree* tree, DumpInfo* info)
{
    assert(tree); assert(info); assert(info->message);
    assert(info->file); assert(info->function);
    assert(tree->debug.dump.file);

    fprintf(tree->debug.dump.file, "\t<h1>TREE DUMP #%03d</h1>\n", tree->debug.dump.image_counter);
    fprintf(tree->debug.dump.file, "\t<h2>Dump {%s:%d} called from %s()</h2>\n",
            info->file, info->line, info->function);
    fprintf(tree->debug.dump.file, "\t<h2>Tree \"%s\" {%s:%d} created in %s()</h2>\n",
            tree->debug.creation.name, tree->debug.creation.file,
            tree->debug.creation.line, tree->debug.creation.function);
    fprintf(tree->debug.dump.file, "\t<h3>STATUS:   %s</h3>\n", status_messages[info->status]);
    fprintf(tree->debug.dump.file, "\t<h3>MESSAGE: %s</h3>\n", info->message);
}


static void createHtmlDump(BinaryTree* tree, DumpInfo* info, const char* image)
{
    assert(tree); assert(info); assert(info->message);
    assert(info->file); assert(info->function);
    assert(tree->debug.dump.file); assert(image);

    fprintf(tree->debug.dump.file, "<html>\n");
    fprintf(tree->debug.dump.file, "<style>\n");
    fprintf(tree->debug.dump.file, "body {font-family: monospace;}\n");
    fprintf(tree->debug.dump.file, "</style>\n");
    fprintf(tree->debug.dump.file, "<body>\n");

    writeTreeInfo(tree, info);

    fprintf(tree->debug.dump.file, "<div style=\"overflow-x: auto; white-space: nowrap;\">\n");
    fprintf(tree->debug.dump.file, "<img src=\"tree_graph_%03d.svg\" "
            "style=\"zoom:0.65; -moz-transform:scale(0.1); -moz-transform-origin:top left;\">\n",
            tree->debug.dump.image_counter);
    fprintf(tree->debug.dump.file, "</div>\n");

    fprintf(tree->debug.dump.file, "<hr style=\"margin: 40px 0; border: 2px solid #ccc;\">\n");
    fprintf(tree->debug.dump.file, "</body>\n");
    fprintf(tree->debug.dump.file, "</html>\n");
}


void treeDump(BinaryTree* tree, TreeStatus status, const char* file, 
              const char* function, int line, const char* format, ...)
{
    assert(tree); assert(tree->debug.dump.file);
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
             tree->debug.dump.directory, tree->debug.dump.image_counter);
    snprintf(graph_svg_file, BUFFER_SIZE * 2, "%s/tree_graph_%03d.svg",
             tree->debug.dump.directory, tree->debug.dump.image_counter);

    generateGraph(tree, graph_dot_file);
    convertDotToSvg(graph_dot_file, graph_svg_file);

    char command[BUFFER_SIZE * 3] = {};
    snprintf(command, BUFFER_SIZE * 3, "rm %s", graph_dot_file);
    system(command);

    createHtmlDump(tree, &info, graph_svg_file);

    tree->debug.dump.image_counter++;
}
