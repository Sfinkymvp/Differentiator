#ifndef DIFF_DEFS_H_
#define DIFF_DEFS_H_


#include "tree/tree_defs.h"


const size_t START_ELEMENT_COUNT = 4;
const double EPS = 1e-7;


#define TEX_FILE diff->tex_dump.file


typedef struct {
    FILE* file;
    char directory[BUFFER_SIZE];
    size_t image_counter;
} GraphDumpState;


typedef struct {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
} PlotRange;


typedef struct {
    FILE* file;
    char filename[BUFFER_SIZE];
    PlotRange range;
} TexDumpState;


typedef struct {
    char* name;
    double value;
} Variable;


typedef struct {
    Variable* variables;
    size_t capacity;
    size_t count; 
} VarTable;


typedef struct {
    size_t order;
    size_t diff_var_idx;
    const char* diff_var_s;
    bool compute; 
} DerivativeInfo;


typedef struct {
    bool decomposition;
    double center;
} TaylorInfo;


typedef struct {
    const char* input_file;
    bool infix_input;
    DerivativeInfo derivative_info;
    TaylorInfo taylor_info;
    bool simple_graph;
} CmdArgs;


typedef struct {
    BinaryTree* trees;
    size_t capacity;
    size_t count;
} Forest;


typedef struct {
    Forest forest;
    VarTable var_table;
    CmdArgs args; 
    GraphDumpState graph_dump;
    TexDumpState tex_dump;
    const TreeNode* highlight_node;
} Differentiator;


#endif // DIFF_DEFS_H_