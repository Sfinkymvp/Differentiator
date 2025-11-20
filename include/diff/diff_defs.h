#ifndef DIFF_DEFS_H_
#define DIFF_DEFS_H_


#include "tree/tree_defs.h"


const size_t START_ELEMENT_COUNT = 4;


typedef struct {
    FILE* dump_file;
    char directory[BUFFER_SIZE];
    int image_counter;
} DumpState;


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
    DumpState dump_state;
} Differentiator;


#endif // DIFF_DEFS_H_