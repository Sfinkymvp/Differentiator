#ifndef PLOT_GENERATOR_H_
#define PLOT_GENERATOR_H_


#include "diff/diff_defs.h"

#include "status.h"


extern const char* GNUPLOT_OUTPUT_FILENAME;
extern const char* GNUPLOT_IMAGES_DIRECTORY;
extern const char* GNUPLOT_DATA_FILENAME;
extern const char* GNUPLOT_SCRIPT_FILENAME;


OperationStatus generatePlot(Differentiator* diff, TreeNode* root, char* output_filename,
    size_t image_counter, bool is_taylor);


#endif // PLOT_GENERATOR_H_