#ifndef DIFF_H_
#define DIFF_H_


#include "diff/diff_defs.h"
#include "status.h"


void printErrorStatus(OperationStatus status);


OperationStatus diffCalculateDerivative(Differentiator* diff, size_t var_idx);


OperationStatus diffConstructor(Differentiator* diff, const int argc, const char** argv);


void diffDestructor(Differentiator* diff);


#endif // DIFF_H_