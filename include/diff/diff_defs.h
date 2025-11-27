#ifndef DIFF_DEFS_H_
#define DIFF_DEFS_H_


#include "tree/tree_defs.h"


const size_t START_ELEMENT_COUNT = 4;


typedef struct {
    FILE* file;
    char directory[BUFFER_SIZE];
    int image_counter;
} GraphDumpState;


typedef struct {
    FILE* file;
    char filename[BUFFER_SIZE];
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
    const char* input_file;
    const char* output_file;
    size_t derivative_order;
    double taylor_center;
    bool simple_graph;
    bool infix_input;
    bool compute_derivative; 
    bool taylor_decomposition;
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
} Differentiator;


#endif // DIFF_DEFS_H_