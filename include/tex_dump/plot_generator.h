#ifndef PLOT_GENERATOR_H_
#define PLOT_GENERATOR_H_


#include "diff/diff_defs.h"

#include "status.h"


extern const char* GNUPLOT_OUTPUT_FILENAME;
extern const char* GNUPLOT_IMAGES_DIRECTORY;
extern const char* GNUPLOT_DATA_FILENAME;
extern const char* GNUPLOT_SCRIPT_FILENAME;


OperationStatus generatePlot(Differentiator* diff, const char* output_filename, size_t tree_count, ...);


#endif // PLOT_GENERATOR_H_