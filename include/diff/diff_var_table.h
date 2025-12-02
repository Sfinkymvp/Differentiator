#ifndef DIFF_VAR_TABLE_H_
#define DIFF_VAR_TABLE_H_


#include "diff/diff_defs.h"
#include "status.h"


OperationStatus addVariable(Differentiator* diff, size_t* var_idx, const char* variable, size_t var_len);


#endif // DIFF_VAR_TABLE_H_