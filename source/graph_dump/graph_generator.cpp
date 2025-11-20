#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "graph_dump/graph_generator.h"
#include "diff/diff_defs.h"
#include "tree/tree.h"


#define CONVERT_TO_STRING(name) case name: return #name;


static const char* opTypeToString(OpType op)
{
    switch (op) {
        CONVERT_TO_STRING(OP_ADD);
        CONVERT_TO_STRING(OP_SUB);
        CONVERT_TO_STRING(OP_MUL);
        CONVERT_TO_STRING(OP_DIV);
        CONVERT_TO_STRING(OP_POW);
        CONVERT_TO_STRING(OP_NONE);
        default: return "UNKNOWN OPERATION";
    }
}


static const char* nodeTypeToString(NodeType type)
{
    switch (type) {
        CONVERT_TO_STRING(NODE_OP);
        CONVERT_TO_STRING(NODE_VAR);
        CONVERT_TO_STRING(NODE_NUM);
        default: return "UNKNOWN TYPE";
    }
}


#undef CONVERT_TO_STRING


static void generateNode(Differentiator* diff, TreeNode* node, FILE* graph_file, int rank, int* counter)
{
    assert(diff); assert(diff->var_table.variables); assert(node); assert(graph_file); assert(counter);
   
    int id = ++(*counter); 

    fprintf(graph_file, "\tnode_%d [shape=Mrecord, fontname=\"Monospace\", ", id);
        
    switch (node->type) {
        case NODE_OP:  fprintf(graph_file, "fillcolor=\"#FCCC94\", color=\"#F87C08\", "); break;
        case NODE_VAR:  fprintf(graph_file, "fillcolor=\"#80DBED\", color=\"#1286DF\", "); break;
        case NODE_NUM: fprintf(graph_file, "fillcolor=\"#87EA00\", color=\"#589800\", "); break;
        default:       fprintf(graph_file, "fillcolor=\"#FF0700\", color=\"#A60400\", "); break;
    }

    fprintf(graph_file, "penwidth=2.0, style=filled, label="
                         "\"{<pointer>%p | Type: %s | Value: ", node, nodeTypeToString(node->type));
    switch (node->type) {
        case NODE_OP:  fprintf(graph_file, "%s", opTypeToString(node->value.op));    break;
        case NODE_VAR: fprintf(graph_file, "%d ('%s')", (int)node->value.var_idx,
                               diff->var_table.variables[node->value.var_idx].name); break;
        case NODE_NUM: fprintf(graph_file, "%.4lf", node->value.num_val);            break;
        default:       fprintf(graph_file, "UNKNOWN_TYPE");                          break;
    }

    fprintf(graph_file, " | {<left>");
    if (node->left)  fprintf(graph_file, "%p", node->left);
    else             fprintf(graph_file, "nil");

    fprintf(graph_file, " | <right>");

    if (node->right) fprintf(graph_file, "%p", node->right);
    else             fprintf(graph_file, "nil");

    fprintf(graph_file, "}}\"];\n");

    if (node->left) {
        fprintf(graph_file, "\tnode_%d:left -> node_%d:n [rank=%d];\n", 
                id, *counter + 1, rank);
        generateNode(diff, node->left, graph_file, rank + 1, counter);
    }

    if (node->right) {
        fprintf(graph_file, "\tnode_%d:right -> node_%d:n [rank=%d];\n", 
                id, *counter + 1, rank);
        generateNode(diff, node->right, graph_file, rank + 1, counter);
    }

}


void generateGraph(Differentiator* diff, BinaryTree* tree, const char* graph_filename)
{
    assert(diff); assert(tree); assert(tree->root); assert(graph_filename);

    FILE* graph_file = fopen(graph_filename, "w");
    assert(graph_file);

    fprintf(graph_file, "digraph Tree {\n");
    fprintf(graph_file, "\trankdir=TB\n");
    fprintf(graph_file, "\tgraph[splines=line];\n");

    int counter = 0;
    int rank = 0;
    generateNode(diff, tree->root, graph_file, rank, &counter);

    fprintf(graph_file, "}\n\n");

    assert(fclose(graph_file) == 0);
}