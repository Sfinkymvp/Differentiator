#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "graph_dump/graph_generator.h"
#include "diff/diff_defs.h"
#include "tree/tree.h"


static void generateNode(Differentiator* diff, TreeNode* node,
    FILE* graph_file, int rank, int* counter);
static void printNodeAttributes(Differentiator* diff, TreeNode* node, FILE* graph_file, int id);

static void generateSimpleNode(Differentiator* diff, TreeNode* node,
    FILE* graph_file, int rank, int* counter);
static void printSimpleNodeAttributes(Differentiator* diff, TreeNode* node, FILE* graph_file, int id);

static void printNodeColor(TreeNode* node, FILE* graph_file);
static void printNodeData(Differentiator* diff, TreeNode* node, FILE* graph_file);
static const char* nodeTypeToString(NodeType type);


void generateGraph(Differentiator* diff, size_t tree_idx, const char* graph_filename)
{
    assert(diff); assert(diff->forest.trees); assert(tree_idx <= diff->forest.count);
    assert(diff->forest.trees[tree_idx].root); assert(graph_filename);

    FILE* graph_file = fopen(graph_filename, "w");
    assert(graph_file);

    fprintf(graph_file, "digraph Tree {\n");
    fprintf(graph_file, "\trankdir=TB\n");
    fprintf(graph_file, "\tgraph[splines=line];\n");

    int counter = 0;
    int rank = 0;
    if (diff->args.simple_graph) {
        generateSimpleNode(diff, diff->forest.trees[tree_idx].root, graph_file, rank, &counter);
    } else {
        generateNode(diff, diff->forest.trees[tree_idx].root, graph_file, rank, &counter);
    }
    fprintf(graph_file, "}\n\n");

    assert(fclose(graph_file) == 0);
}


static void generateNode(Differentiator* diff, TreeNode* node,
    FILE* graph_file, int rank, int* counter)
{
    assert(diff); assert(diff->var_table.variables); assert(node);
    assert(graph_file); assert(counter);
   
    int id = ++(*counter); 

    printNodeAttributes(diff, node, graph_file, id);
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


static void printNodeAttributes(Differentiator* diff, TreeNode* node, FILE* graph_file, int id)
{
    assert(diff); assert(node); assert(graph_file);

    fprintf(graph_file, "\tnode_%d [shape=Mrecord, fontname=\"Monospace\", ", id);
    printNodeColor(node, graph_file);
    fprintf(graph_file, "penwidth=2.0, style=filled, label="
        "\"{<pointer>%p | Type: %s | Value: ", node, nodeTypeToString(node->type));
    printNodeData(diff, node, graph_file);

    if (node->left && node->right) {
        fprintf(graph_file, " | {<left>%p | <right>%p", node->left, node->right);
    } else if (node->left) {
        fprintf(graph_file, " | {<left>%p | <right>nil", node->left);
    } else if (node->right) {
        fprintf(graph_file, " | {<left>nil | <right>%p", node->right);
    } else {
        fprintf(graph_file, " | {<left>nil | <right>nil");
    }
    fprintf(graph_file, "}}\"];\n");

}


static void generateSimpleNode(Differentiator* diff, TreeNode* node,
    FILE* graph_file, int rank, int* counter)
{
    assert(diff); assert(diff->var_table.variables); assert(node);
    assert(graph_file); assert(counter);
  
    int id = ++(*counter); 
    printSimpleNodeAttributes(diff, node, graph_file, id);
    if (node->left) {
        fprintf(graph_file, "\tnode_%d -> node_%d [rank=%d];\n", id, *counter + 1, rank);
        generateSimpleNode(diff, node->left, graph_file, rank + 1, counter);
    }
    if (node->right) {
        fprintf(graph_file, "\tnode_%d -> node_%d [rank=%d];\n", id, *counter + 1, rank);
        generateSimpleNode(diff, node->right, graph_file, rank + 1, counter);
    }
}


static void printSimpleNodeAttributes(Differentiator* diff, TreeNode* node, FILE* graph_file, int id)
{
    assert(diff); assert(node); assert(graph_file);

    fprintf(graph_file, "\tnode_%d [shape=\"box\", fontname=\"Monospace\", ", id);
    printNodeColor(node, graph_file);
    fprintf(graph_file, "penwidth=2.0, style=filled, label=\"");
    printNodeData(diff, node, graph_file);
    fprintf(graph_file, "\"];\n");
}


static void printNodeColor(TreeNode* node, FILE* graph_file)
{
    assert(node); assert(graph_file);

    switch (node->type) {
        case NODE_OP:  fprintf(graph_file, "fillcolor=\"#FCCC94\", color=\"#F87C08\", "); break;
        case NODE_VAR: fprintf(graph_file, "fillcolor=\"#80DBED\", color=\"#1286DF\", "); break;
        case NODE_NUM: fprintf(graph_file, "fillcolor=\"#87EA00\", color=\"#589800\", "); break;
        default:       fprintf(graph_file, "fillcolor=\"#FF0700\", color=\"#A60400\", "); break;
    }
}


static void printNodeData(Differentiator* diff, TreeNode* node, FILE* graph_file)
{
    assert(diff); assert(diff->var_table.variables); assert(node); assert(graph_file);

    switch (node->type) {
        case NODE_OP:  fprintf(graph_file, "%s", OP_TABLE[node->value.op].symbol); break;
        case NODE_VAR: fprintf(graph_file, "'%s'", diff->var_table.variables[node->value.var_idx].name); break;
        case NODE_NUM: fprintf(graph_file, "%g", node->value.num_val); break;
        default:       fprintf(graph_file, "UNKNOWN_TYPE"); break;
    }
}


static const char* nodeTypeToString(NodeType type)
{
    switch (type) {
        case NODE_OP:  return "NODE_OP";
        case NODE_VAR: return "NODE_VAR";
        case NODE_NUM: return "NODE_NUM";
        default:       return "UNKNOWN TYPE";
    }
}