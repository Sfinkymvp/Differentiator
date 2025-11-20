#ifndef DIFF_H_
#define DIFF_H_


#include "diff/diff_defs.h"
#include "status.h"


OperationStatus diffConstructor(Differentiator* diff);


void diffDestructor(Differentiator* diff);


#endif // DIFF_H_