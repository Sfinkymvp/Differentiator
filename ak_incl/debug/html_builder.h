#ifndef INCLUDE_DEBUG_HTML_BUILDER_H_
#define INCLUDE_DEBUG_HTML_BUILDER_H_


#include "../akinator/tree.h"


void treeDump(BinaryTree* tree, TreeStatus status, const char* file, 
              const char* function, int line, const char* format, ...);


#endif // INCLUDE_DEBUG_HTML_BUILDER_H_
