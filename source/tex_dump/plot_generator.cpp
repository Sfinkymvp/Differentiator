#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>

#include "tex_dump/plot_generator.h"

#include "diff/diff_defs.h"
#include "diff/diff_evaluate.h"
#include "diff/diff_var_table.h"

#include "status.h"


const char* GNUPLOT_OUTPUT_FILENAME =  "plot_output";
const char* GNUPLOT_IMAGES_DIRECTORY = "tex/images";
const char* GNUPLOT_DATA_FILENAME =    "plot_data";
const char* GNUPLOT_SCRIPT_FILENAME =  "plot_script";


double GNUPLOT_SHIFT = 0.001;


static OperationStatus processPlotting(Differentiator* diff, const char* output_filename,
    size_t* tree_indexes, size_t tree_count);

static OperationStatus generatePlotData(Differentiator* diff, size_t tree_idx);

static OperationStatus generatePlotScript(Differentiator* diff, const char* output_filename,
    const char* script_filename, size_t* tree_indexes, size_t tree_count);
static void printScriptInfo(Differentiator* diff, const char* output_filename,
    FILE* script_file, size_t* tree_indexes, size_t tree_count);

static OperationStatus finishPlotting(const char* script_filename);

static OperationStatus deleteScriptFile(const char* script_filename);
static OperationStatus deleteDataFile(size_t tree_idx);

static void createDirectory(const char* directory);


OperationStatus generatePlot(Differentiator* diff, const char* output_filename, size_t tree_count, ...)
{
    assert(diff); assert(diff->var_table.variables); assert(diff->forest.trees);
    assert(output_filename);

    createDirectory(GNUPLOT_IMAGES_DIRECTORY);
    size_t* tree_indexes = (size_t*)calloc(tree_count, sizeof(size_t));
    if (tree_indexes == NULL) {
        return STATUS_SYSTEM_OUT_OF_MEMORY;
    }

    va_list args = {};
    va_start(args, tree_count);
    for (size_t index = 0; index < tree_count; index++) {
        tree_indexes[index] = va_arg(args, size_t);
    }
    va_end(args);

    OperationStatus status = processPlotting(diff, output_filename, tree_indexes, tree_count);
    free(tree_indexes);

    return status;
}


static OperationStatus processPlotting(Differentiator* diff, const char* output_filename,
    size_t* tree_indexes, size_t tree_count)
{
    assert(diff); assert(diff->forest.trees); assert(output_filename); assert(tree_indexes); 

    static size_t script_counter = 0;
    OperationStatus status = STATUS_OK;

    for (size_t index = 0; index < tree_count; index++) {
        status = generatePlotData(diff, tree_indexes[index]);
        if (status != STATUS_OK) {
            break;
        }
    }

    if (status == STATUS_OK) {
        char script_filename[BUFFER_SIZE * 2] = "";
        snprintf(script_filename, BUFFER_SIZE * 2, "%s/%s_%03zu",
            GNUPLOT_IMAGES_DIRECTORY, GNUPLOT_SCRIPT_FILENAME, script_counter);
        script_counter++;

        status = generatePlotScript(diff, output_filename, script_filename, tree_indexes, tree_count);
        if (status == STATUS_OK) {
            status = finishPlotting(script_filename);
        }

        deleteScriptFile(script_filename);
    }

    for (size_t index = 0; index < tree_count; index++) {
        deleteDataFile(tree_indexes[index]);
    }

    return status;
}


static OperationStatus generatePlotData(Differentiator* diff, size_t tree_idx)
{
    assert(diff); assert(diff->forest.trees);

    char data_filename[BUFFER_SIZE * 2] = "";
    snprintf(data_filename, BUFFER_SIZE * 2, "%s/%s_%03zu", GNUPLOT_IMAGES_DIRECTORY,
        GNUPLOT_DATA_FILENAME, tree_idx);

    FILE* data_file = fopen(data_filename, "w");
    if (data_file == NULL) {
        return STATUS_IO_FILE_OPEN_ERROR;
    }

    for (double x = diff->tex_dump.range.x_min; x <= diff->tex_dump.range.x_max; x += GNUPLOT_SHIFT) {
        setVariableValue(diff, diff->args.derivative_info.diff_var_idx, x);
        double y = evaluateNode(diff, diff->forest.trees[tree_idx].root);
        if (!isnan(y)) {
            fprintf(data_file, "%f %f\n", x, y);
        } else {
            fprintf(data_file, "\n");
        }
    }

    if (fclose(data_file) != 0) {
        return STATUS_IO_FILE_CLOSE_ERROR;
    }
    return STATUS_OK;
}


static OperationStatus generatePlotScript(Differentiator* diff, const char* output_filename,
    const char* script_filename, size_t* tree_indexes, size_t tree_count)
{
    assert(diff); assert(diff->forest.trees); assert(output_filename);
    assert(script_filename); assert(tree_indexes); 

    FILE* script_file = fopen(script_filename, "w");
    if (script_file == NULL) {
        return STATUS_IO_FILE_OPEN_ERROR;
    }

    printScriptInfo(diff, output_filename, script_file, tree_indexes, tree_count);

    if (fclose(script_file) != 0) { 
        return STATUS_IO_FILE_CLOSE_ERROR;
    }
    return STATUS_OK;
}


static void printScriptInfo(Differentiator* diff, const char* output_filename,
    FILE* script_file, size_t* tree_indexes, size_t tree_count)
{
    assert(diff); assert(diff->forest.trees); assert(output_filename); 
    assert(script_file); assert(tree_indexes); 

    fprintf(script_file, 
        "set terminal pdfcairo enhanced font 'Helvetica,12'\n"
        "set output '%s.pdf'\n"
        "set xlabel 'X Axis'\n"
        "set xrange [%g:%g]\n"
        "set ylabel 'Y Axis'\n"
        "set yrange [%g:%g]\n"
        "set border 3\n"
        "set tics out\n"
        "set grid xtics ytics ls 1 lc 'gray' dt 3\n", output_filename,
        diff->tex_dump.range.x_min, diff->tex_dump.range.x_max,
        diff->tex_dump.range.y_min, diff->tex_dump.range.y_max);

    for (size_t index = 0; index < tree_count; index++) {
        if (index == 0) {
            fprintf(script_file, "plot '%s/%s_%03zu' using 1:2 with lines",
                GNUPLOT_IMAGES_DIRECTORY, GNUPLOT_DATA_FILENAME, tree_indexes[0]);
        } else {
            fprintf(script_file, ", '%s/%s_%03zu' using 1:2 with lines",
                GNUPLOT_IMAGES_DIRECTORY, GNUPLOT_DATA_FILENAME, tree_indexes[index]);
        }

        if (tree_indexes[index] == 0) {
            fprintf(script_file, " title 'Функция'");
        } else if (tree_indexes[index] == diff->forest.count) {
            fprintf(script_file, " title 'Разложение'");

            double x = diff->args.taylor_info.center;
            setVariableValue(diff, diff->args.derivative_info.diff_var_idx, x);
            double y = evaluateNode(diff, diff->forest.trees[tree_indexes[index]].root);

            double slope = evaluateNode(diff, diff->forest.trees[1].root);
            fprintf(script_file, 
                ", \\\n    \"\" using (%lf):(%lf) with points pt 3 ps 0.8 lc 'red' "
                "title 'Центр разложения'", x, y);

            fprintf(script_file, ", \\\n %lf * (x - %lf) + %lf with lines dt 3 "
                "lc 'black' title 'Касательная'", slope, x, y);
        } else {
            fprintf(script_file, " title '%zu-я производная'", tree_indexes[index]);
        }
    }

    fprintf(script_file, "\nquit\n");
}


static OperationStatus finishPlotting(const char* script_filename)
{
    assert(script_filename);

    char command[BUFFER_SIZE * 2] = "";
    snprintf(command, BUFFER_SIZE * 2, "gnuplot %s", script_filename);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Gnuplot error with file '%s'!", script_filename);
        return STATUS_SYSTEM_CALL_ERROR;
    }

    return STATUS_OK;
}


static OperationStatus deleteScriptFile(const char* script_filename)
{
    char command[BUFFER_SIZE * 2] = "";
    snprintf(command, BUFFER_SIZE * 2, "rm %s", script_filename);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "An error occurred while deleting the file %s!", script_filename);
        return STATUS_SYSTEM_CALL_ERROR;
    }

    return STATUS_OK;
}


static OperationStatus deleteDataFile(size_t tree_idx)
{
    char command[BUFFER_SIZE * 2] = "";
    snprintf(command, BUFFER_SIZE * 2, "rm %s/%s_%03zu", GNUPLOT_IMAGES_DIRECTORY,
        GNUPLOT_DATA_FILENAME, tree_idx);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "An error occurred while deleting the file %s/%s_%zu!", 
            GNUPLOT_IMAGES_DIRECTORY, GNUPLOT_DATA_FILENAME, tree_idx);
        return STATUS_SYSTEM_CALL_ERROR;    
    }

    return STATUS_OK;
}


static void createDirectory(const char* directory)
{
    assert(directory);

    char command[BUFFER_SIZE] = "";
    snprintf(command, BUFFER_SIZE, "mkdir -p %s", directory);
    system(command);
}