#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "tex_dump/plot_generator.h"

#include "diff/diff_defs.h"
#include "diff/diff_evaluate.h"
#include "diff/diff_var_table.h"

#include "status.h"


const char* GNUPLOT_OUTPUT_FILENAME =  "plot_output";
const char* GNUPLOT_IMAGES_DIRECTORY = "tex/images";
const char* GNUPLOT_DATA_FILENAME =    "plot_data";
const char* GNUPLOT_SCRIPT_FILENAME =  "plot_script";


static OperationStatus generatePlotScript(Differentiator* diff, TreeNode* root,
    const char* data_filename, const char* script_filename, const char* output_filename);
static OperationStatus generatePlotData(Differentiator* diff, TreeNode* root,
    const char* data_filename, const char* script_filename, const char* output_filename);
static OperationStatus generateTaylorPlotScript(Differentiator* diff, TreeNode* function_root, TreeNode* taylor_root,
    const char* function_data_filename, const char* taylor_data_filename, const char* script_filename, const char* output_filename);
static OperationStatus generateTaylorPlotData(Differentiator* diff, TreeNode* root,
    const char* data_filename, const char* script_filename, const char* output_filename);
static void createDirectory(const char* directory);
static void generateFilenames(size_t image_counter, char* data_filename,
                              char* script_filename);


OperationStatus generatePlot(Differentiator* diff, TreeNode* root, char* output_filename,
    size_t image_counter, bool is_taylor)
{
    assert(diff); assert(root); assert(output_filename);

    char data_filename[BUFFER_SIZE] = "";
    char script_filename[BUFFER_SIZE] = "";

    createDirectory(GNUPLOT_IMAGES_DIRECTORY);
    generateFilenames(image_counter, data_filename, script_filename);

    OperationStatus status = STATUS_OK; 
    if (is_taylor) {
        char function_data_filename[BUFFER_SIZE] = "";
        char function_script_filename[BUFFER_SIZE] = "";
        generateFilenames(0, function_data_filename, function_script_filename);

        status = generateTaylorPlotScript(diff, diff->forest.trees[0].root, root,
            function_data_filename, data_filename, script_filename, output_filename);
    } else {
        status = generatePlotScript(diff, root, data_filename, script_filename, output_filename);
    }
    if (status != STATUS_OK) {
        fprintf(stderr, "generatePlot can't generate Plot!\n");
        return status;
    }

    char command[BUFFER_SIZE] = "";
    snprintf(command, BUFFER_SIZE * 2, "gnuplot %s", script_filename);
    printf("gnuplot compile %s\n", script_filename);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Gnuplot didn't run successful!\n");
    }
    //snprintf(command, BUFFER_SIZE, "rm %s && rm %s", data_filename, script_filename);
    result = system(command);
    if (result != 0) {
        fprintf(stderr, "Failed to remove gnuplot files!\n");
    }

    return STATUS_OK;
}


static OperationStatus generatePlotScript(Differentiator* diff, TreeNode* root,
    const char* data_filename, const char* script_filename, const char* output_filename)
{
    assert(diff); assert(root);
    assert(data_filename); assert(script_filename); assert(output_filename);

    OperationStatus status = generatePlotData(diff, root, data_filename, script_filename,
    output_filename);
    if (status != STATUS_OK) return status;

    FILE* script_file = fopen(script_filename, "w");
    if (script_file == NULL) return STATUS_IO_FILE_OPEN_ERROR;

    fprintf(script_file, "set terminal pdfcairo enhanced font 'Arial,12'\n"); 
    fprintf(script_file, "set output '%s.pdf'\n", output_filename);
    fprintf(script_file, "set xlabel 'X Axis'\n");
    fprintf(script_file, "set ylabel 'Y Axis'\n");
    fprintf(script_file, "plot '%s' using 1:2 with lines\n", data_filename);
    fprintf(script_file, "quit\n");

    if (fclose(script_file) != 0) return STATUS_IO_FILE_CLOSE_ERROR;

    return STATUS_OK;
}


static OperationStatus generatePlotData(Differentiator* diff, TreeNode* root,
    const char* data_filename, const char* script_filename, const char* output_filename)
{
    assert(diff); assert(root);
    assert(data_filename); assert(script_filename); assert(output_filename);

    FILE* data_file = fopen(data_filename, "w");
    if (data_file == NULL) return STATUS_IO_FILE_OPEN_ERROR;

    for (double x = -5; x <= 5; x += 0.05) {
        setVariableValue(diff, diff->args.derivative_info.diff_var, x);
        double y = evaluateNode(diff, root);
        if (!isnan(y)) {
            fprintf(data_file, "%f %f\n", x, y);
        }
    }

    if (fclose(data_file) != 0) return STATUS_IO_FILE_CLOSE_ERROR;

    return STATUS_OK;
}


static OperationStatus generateTaylorPlotScript(Differentiator* diff, TreeNode* function_root, TreeNode* taylor_root,
    const char* function_data_filename, const char* taylor_data_filename, const char* script_filename, const char* output_filename)
{
    assert(diff); assert(function_root); assert(taylor_root);
    assert(function_data_filename); assert(taylor_data_filename); assert(script_filename); assert(output_filename);

    OperationStatus status = generateTaylorPlotData(diff, function_root, function_data_filename, script_filename, output_filename);
    if (status != STATUS_OK) return status;
    status = generateTaylorPlotData(diff, taylor_root, taylor_data_filename, script_filename, output_filename);
    if (status != STATUS_OK) return status;

    FILE* script_file = fopen(script_filename, "w");
    if (script_file == NULL) return STATUS_IO_FILE_OPEN_ERROR;

    fprintf(script_file, "set terminal pdfcairo enhanced font 'Arial,12'\n"); 
    fprintf(script_file, "set output '%s.pdf'\n", output_filename);
    fprintf(script_file, "set xlabel 'X Axis'\n");
    fprintf(script_file, "set ylabel 'Y Axis'\n");
    fprintf(script_file, "plot '%s' using 1:2 with lines title 'Функция', '%s' using 1:2 with lines title 'Приближение'\n",
        function_data_filename, taylor_data_filename);
    fprintf(script_file, "quit\n");

    if (fclose(script_file) != 0) return STATUS_IO_FILE_CLOSE_ERROR;

    return STATUS_OK;
}


static OperationStatus generateTaylorPlotData(Differentiator* diff, TreeNode* root,
    const char* data_filename, const char* script_filename, const char* output_filename)
{
    assert(diff); assert(root);
    assert(data_filename); assert(script_filename); assert(output_filename);

    FILE* data_file = fopen(data_filename, "w");
    if (data_file == NULL) return STATUS_IO_FILE_OPEN_ERROR;

    for (double x = -5; x <= 5; x += 0.05) {
        setVariableValue(diff, diff->args.derivative_info.diff_var, x);
        double y = evaluateNode(diff, root);
        if (!isnan(y)) {
            fprintf(data_file, "%f %f\n", x, y);
        }
    }

    if (fclose(data_file) != 0) return STATUS_IO_FILE_CLOSE_ERROR;

    return STATUS_OK;
}


static void createDirectory(const char* directory)
{
    assert(directory);

    char command[BUFFER_SIZE] = "";
    snprintf(command, BUFFER_SIZE, "mkdir -p %s", directory);
    system(command);
}


static void generateFilenames(size_t image_counter, char* data_filename,
                              char* script_filename)
{
    assert(data_filename); assert(script_filename);

    snprintf(data_filename, BUFFER_SIZE, "%s/%s_%03zu", 
             GNUPLOT_IMAGES_DIRECTORY, GNUPLOT_DATA_FILENAME, image_counter);
    snprintf(script_filename, BUFFER_SIZE, "%s/%s_%03zu", 
             GNUPLOT_IMAGES_DIRECTORY, GNUPLOT_SCRIPT_FILENAME, image_counter);
}

