#ifndef DIFF_CMD_ARGS_H
#define DIFF_CMD_ARGS_H_


#include "diff/diff_defs.h"

#include "status.h"


OperationStatus parseArgs(Differentiator* diff, const int argc, const char** argv);


#endif // DIFF_CMD_ARGS_H_