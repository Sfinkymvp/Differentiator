#ifndef TEX_STRUCT_H_
#define TEX_STRUCT_H_


#include <stdarg.h>

#include "diff/diff_defs.h"
#include "tex_dump/tex_expression.h"
#include "tex_dump/plot_generator.h"

#include "status.h"


void texInit(Differentiator* diff);


void printIntroduction(Differentiator* diff);


void printPlot(Differentiator* diff, size_t tree_idx);


void printTaylorSeries(Differentiator* diff, const char* output_filename, size_t tree_idx);


OperationStatus texClose(Differentiator* diff);


#endif // TEX_STRUCT_H_