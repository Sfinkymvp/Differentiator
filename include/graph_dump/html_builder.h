#ifndef HTML_BUILDER_H_
#define HTML_BUILDER_H_


#include "diff/diff_defs.h"
#include "status.h"


void treeDump(Differentiator* diff, BinaryTree* tree, OperationStatus status, const char* file, 
              const char* function, int line, const char* format, ...);


void openDumpFile(Differentiator* diff);


#endif // HTML_BUILDER_H_