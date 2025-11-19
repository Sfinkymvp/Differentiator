#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack/error.h"
#include "stack/stack.h"
#ifdef HASH_ENABLED
#include "stack_hash.h"
#endif // HASH_ENABLED


const char* ErrorString[] = {
    "SUCCESS (error 0)",
    "NULL_PTR (error 1)",
    "OUT_OF_MEMORY (error 2)",
    "STACK_UNDERFLOW (error 3)",
    "STACK_OVERFLOW (error 4)",
    "CANARY_CORRUPTED (error 5)",
    "POISON_CORRUPTED (error 6)",
    "HASH_CORRUPTED (error 7)",
    "STRUCT_CANARY_CORRUPTED (error 8)",
    "STRUCT_HASH_CORRUPTED (error 9)"};


#ifdef DEBUG
StackError _stackAssert(Stack_t* stack,
                        const char* function,
                        const char* file,
                        int line)
{
    StackError error_code = stackVerify(stack);

    if (error_code != SUCCESS) {
        fprintf(stderr, "StackAssert triggered in %s (%s:%d)\n",
                function, file, line);
#ifdef DUMP
        stackDump(stack, error_code);
#endif // DUMP
#ifdef HARD
        if (stack != NULL)
            stackDtor(stack);
        abort();
#endif // HARD
    }
    return error_code;
}
#else
StackError _stackAssert(Stack_t* stack)
{
    StackError error_code = stackVerify(stack);

    if (error_code != SUCCESS) {
#ifdef DUMP
        stackDump(stack, error_code);
#endif // DUMP
#ifdef HARD
        if (stack != NULL)
            stackDtor(stack);
        abort();
#endif // HARD
    }
    return error_code;
}
#endif // DEBUG


StackError stackVerify(const Stack_t* stack)
{
    if (stack == NULL || stack->data == NULL)
        return NULL_PTR;
#ifdef STRUCT_PROTECT
    if (stack->left_canary != STRUCT_LEFT_CANARY ||
        stack->right_canary != STRUCT_RIGHT_CANARY)
        return STRUCT_CANARY_CORRUPTED;
    if (!checkStructHash(stack))
        return STRUCT_HASH_CORRUPTED;
#endif // STRUCT_PROTECT
#ifdef CANARY
    if (stack->data[-1] != LEFT_CANARY ||
        stack->data[stack->capacity] != RIGHT_CANARY)
        return CANARY_CORRUPTED;
#endif // CANARY
    if (stack->size > stack->capacity)
        return STACK_OVERFLOW;
#ifdef HASH
    if (!checkStackHash(stack))
        return HASH_CORRUPTED; 
#endif // HASH
#ifdef POISON
    for (size_t index = stack->size; index < stack->capacity; index++)
        if (stack->data[index] != POISON_VALUE)
            return POISON_CORRUPTED;
#endif // POISON

    return SUCCESS;
}


void stackDump(const Stack_t* stack, StackError error_code)
{
    if (stack == NULL) {
        fprintf(stderr, "Stack [NULL POINTER], (%s)\n", ErrorString[error_code]);
        return;
    }

#ifdef DEBUG
    printDebugInfo(stack, error_code);
#else
    fprintf(stderr, "Stack [%p] (%s)\n", stack, ErrorString[error_code]);
#endif // DEBUG
#ifdef STRUCT_PROTECT
    printStructCanaryState(stack); 
#endif // STRUCT_PROTECT

    fprintf(stderr, "size = %zu\n", stack->size);
    fprintf(stderr, "capacity = %zu\n\n", stack->capacity);

    if (stack->data == NULL) {
        fprintf(stderr, "data[NULL POINTER]\n");
        return;
    }

    if (error_code != STRUCT_CANARY_CORRUPTED &&
        error_code != STRUCT_HASH_CORRUPTED) {
#ifdef CANARY
        printStackCanaryState(stack);
#endif // CANARY
        fprintf(stderr, "data[%p]\n", stack->data);
#ifdef DEBUG 
        printStackData(stack);
#endif // DEBUG
    }
}


#ifdef STRUCT_PROTECT
void printStructCanaryState(const Stack_t* stack)
{
    assert(stack != NULL);

    if (stack->left_canary == STRUCT_LEFT_CANARY)
        fprintf(stderr, "STRUCT_LEFT_CANARY OK: 0x%lX\n", stack->left_canary);
    else
        fprintf(stderr, "STRUCT_LEFT_CANARY NOT OK: 0x%lX (expected 0x%lX)\n",
                stack->left_canary, STRUCT_LEFT_CANARY);
        
    if (stack->right_canary == STRUCT_RIGHT_CANARY)
        fprintf(stderr, "STRUCT_RIGHT_CANARY OK: 0x%lX\n\n", stack->right_canary);
    else
        fprintf(stderr, "STRUCT_RIGHT_CANARY NOT OK: 0x%lX (expected 0x%lX)\n\n", 
                stack->right_canary, STRUCT_RIGHT_CANARY);
}
#endif // STRUCT_PROTECT


#ifdef CANARY
void printStackCanaryState(const Stack_t* stack)
{
    assert(stack != NULL);
    assert(stack->data != NULL);

    if (stack->data[-1] == LEFT_CANARY)
        fprintf(stderr, "STACK_LEFT_CANARY OK: 0x%X\n", (unsigned int)stack->data[-1]);
    else
        fprintf(stderr, "STACK_LEFT_CANARY NOT OK: 0x%X (expected 0x%X)\n",
                (unsigned int)stack->data[-1], (unsigned int)LEFT_CANARY);
        
    if (stack->data[stack->capacity] == RIGHT_CANARY)
        fprintf(stderr, "STACK_RIGHT_CANARY OK: 0x%X\n", (unsigned int)stack->data[stack->capacity]);
    else
        fprintf(stderr, "STACK_RIGHT_CANARY NOT OK: 0x%X (expected 0x%X)\n", 
                (unsigned int)stack->data[stack->capacity], (unsigned int)RIGHT_CANARY);
}
#endif // CANARY


#ifdef DEBUG
void printDebugInfo(const Stack_t* stack, StackError error_code)
{
    fprintf(stderr, "Stack '%s' [%p] (%s)\n", 
            stack->debug_info.variable_name, stack, ErrorString[error_code]);
    fprintf(stderr, "Created in %s() at %s:%d\n\n", stack->debug_info.function_name,
            stack->debug_info.file_name, stack->debug_info.line);
}
#endif // DEBUG


#ifdef DEBUG
void printStackData(const Stack_t* stack)
{
    fprintf(stderr, "{\n");

    size_t index = 0;
    for (; index < stack->size; index++)
        fprintf(stderr, "\t*[%zu] = %p\n", index, &stack->data[index]);

#ifdef POISON
    for (; index < stack->capacity; index++) {
        if (stack->data[index] == POISON_VALUE)
            fprintf(stderr, "\t [%zu] = POISON\n", index);
        else
            fprintf(stderr, "\t [%zu] = CORRUPTED (not POISON)\n", index);
    }
#endif // POISON
    fprintf(stderr, "}\n");
}
#endif // DEBUG
