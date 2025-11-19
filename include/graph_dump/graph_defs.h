#ifndef GRAPH_DEFS_H_
#define GRAPH_DEFS_H_


#include "tree/tree_defs.h"


typedef struct {
    OperationStatus status;
    const char* message;
    const char* file;
    const char* function;
    int line;
} DumpInfo;


#endif // GRAPH_DEFS_H_