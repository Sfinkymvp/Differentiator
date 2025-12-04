#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "diff/diff_cmd_args.h"
#include "diff/diff_defs.h"

#include "status.h"


static void setTypicalSettings(Differentiator* diff);
static OperationStatus parseInputFile(Differentiator* diff, const int argc, const char** argv, size_t* index);
static OperationStatus parseDerivativeOrder(Differentiator* diff, const int argc, const char** argv, size_t* index);
static OperationStatus parseTaylorDecomposition(Differentiator* diff, const int argc, const char** argv, size_t* index);
static OperationStatus parseDiffVariable(Differentiator* diff, const int argc, const char** argv, size_t* index);


OperationStatus parseArgs(Differentiator* diff, const int argc, const char** argv)
{
    assert(diff); assert(argv);

    setTypicalSettings(diff);

    OperationStatus status = STATUS_OK;
    for (size_t index = 1; index < (size_t)argc && status == STATUS_OK; index++) {
        if (strcmp(argv[index], "--input") == 0) {
            status = parseInputFile(diff, argc, argv, &index);
        } else if (strcmp(argv[index], "--order") == 0) {
            status = parseDerivativeOrder(diff, argc, argv, &index); 
        } else if (strcmp(argv[index], "--taylor") == 0) {
            status = parseTaylorDecomposition(diff, argc, argv, &index); 
        } else if (strcmp(argv[index], "--dvar") == 0) {
            status = parseDiffVariable(diff, argc, argv, &index); 
        } else if (strcmp(argv[index], "--simple_graph") == 0) {
            diff->args.simple_graph = true;
        } else if (strcmp(argv[index], "--infix") == 0) {
            diff->args.infix_input = true;
        } else if (strcmp(argv[index], "--compute") == 0) {
            diff->args.derivative_info.compute = true;
        } else {
            return STATUS_CLI_UNKNOWN_OPTION;
        }
    }

    return status;
}


static void setTypicalSettings(Differentiator* diff)
{
    assert(diff); 

    diff->args.input_file = "../data/test1";
    diff->args.infix_input = false;
    diff->args.derivative_info.order = 1;
    diff->args.derivative_info.diff_var_idx = 0;
    diff->args.derivative_info.diff_var_s = NULL;
    diff->args.derivative_info.compute = false;
    diff->args.taylor_info.decomposition = false;
    diff->args.taylor_info.center = 0;
    diff->args.simple_graph = false;
}


static OperationStatus parseInputFile(Differentiator* diff, const int argc, const char** argv, size_t* index)
{
    assert(diff); assert(argv); assert(index);

    if (*index + 1 < (size_t)argc && argv[*index + 1][0] != '-') {
        diff->args.input_file = argv[*index + 1]; (*index)++;
        return STATUS_OK;
    }

    return STATUS_CLI_UNKNOWN_OPTION;
}


static OperationStatus parseDerivativeOrder(Differentiator* diff, const int argc, const char** argv, size_t* index)
{
    assert(diff); assert(argv); assert(index);

    if (*index + 1 < (size_t)argc && argv[*index + 1][0] != '-') {
        char* end = NULL;
        diff->args.derivative_info.order = strtoull(argv[*index + 1], &end, 10);
        if (*end != '\0') {
            return STATUS_CLI_UNKNOWN_OPTION;
        }
        
        (*index)++;
        return STATUS_OK;
    }

    return STATUS_CLI_UNKNOWN_OPTION;
}


static OperationStatus parseTaylorDecomposition(Differentiator* diff, const int argc, const char** argv, size_t* index)
{
    assert(diff); assert(argv); assert(index);
    
    if (*index + 1 < (size_t)argc && (argv[*index + 1][1] != '-')) {
        char* end = NULL;
        diff->args.taylor_info.center = strtod(argv[*index + 1], &end);
        diff->args.taylor_info.decomposition = true;
        if (*end != '\0') {
            return STATUS_CLI_UNKNOWN_OPTION;
        }
        
        (*index)++;
        return STATUS_OK;
    }

    return STATUS_CLI_UNKNOWN_OPTION;
}


static OperationStatus parseDiffVariable(Differentiator* diff, const int argc, const char** argv, size_t* index)
{
    assert(diff); assert(argv); assert(index);

    if (*index + 1 < (size_t)argc && (argv[*index + 1][0] != '-')) {
        diff->args.derivative_info.diff_var_s = argv[*index + 1];
        (*index)++;
        return STATUS_OK;
    }

    return STATUS_CLI_UNKNOWN_OPTION;
}