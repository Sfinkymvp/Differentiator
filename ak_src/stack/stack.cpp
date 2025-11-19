#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack/stack.h"
#include "stack/error.h"
#ifdef HASH_ENABLED
#include "stack_hash.h"
#endif // HASH_ENABLED


static StackError stackExpand(Stack_t* stack)
{
    StackError error_code = stackAssert(stack);
    if (error_code != SUCCESS)
        return error_code;

#ifdef CANARY
    void* temp = realloc(stack->data - 1, (stack->capacity * 2 + 2) * sizeof(Element_t));
#else
    void* temp = realloc(stack->data, stack->capacity * 2 * sizeof(Element_t));
#endif // CANARY
    if (temp == NULL)
        return OUT_OF_MEMORY;
    
    stack->data = (Element_t*)temp;
    stack->capacity *= 2;
#ifdef CANARY
    stack->data += 1;
    stack->data[stack->capacity] = RIGHT_CANARY;
#endif // CANARY
#ifdef POISON
    for (size_t index = stack->capacity / 2;
        index < stack->capacity; index++)
        stack->data[index] = POISON_VALUE;
#endif // POISON
#ifdef HASH_ENABLED
    updateHashes(stack);
#endif // HASH_ENABLED

    return SUCCESS;
}


static StackError stackShrink(Stack_t* stack)
{
    StackError error_code = stackAssert(stack);
    if (error_code != SUCCESS)
        return error_code;

#ifdef CANARY
    void* temp = realloc(stack->data - 1, (stack->capacity / 2 + 2) * sizeof(Element_t));
#else
    void* temp = realloc(stack->data, (stack->capacity / 2) * sizeof(Element_t));
#endif // CANARY
    if (temp == NULL)
        return OUT_OF_MEMORY;

    stack->data = (Element_t*)temp;
    stack->capacity /= 2;
#ifdef CANARY
    stack->data += 1;
    stack->data[stack->capacity] = RIGHT_CANARY;
#endif // CANARY
#ifdef HASH_ENABLED
    updateHashes(stack);
#endif // HASH_ENABLED

    return SUCCESS;
}


StackError stackCtor(Stack_t* stack, size_t start_capacity)

{
    if (stack == NULL)
        return NULL_PTR;

#ifdef CANARY
    stack->data = (Element_t*)calloc(start_capacity + 2, sizeof(Element_t));
#else
    stack->data = (Element_t*)calloc(start_capacity, sizeof(Element_t));
#endif // CANARY
    if (stack->data == NULL)
        return OUT_OF_MEMORY;

    stack->capacity = start_capacity;
    stack->size = 0;
#ifdef CANARY
    stack->data += 1;
    stack->data[-1] = LEFT_CANARY;
    stack->data[stack->capacity] = RIGHT_CANARY;
#endif // CANARY
#ifdef POISON
    for (size_t index = 0; index < stack->capacity; index++)
        stack->data[index] = POISON_VALUE;
#endif // POISON
#ifdef HASH_ENABLED
    updateHashes(stack);
#endif // HASH_ENABLED

    return SUCCESS;
}


StackError stackDtor(Stack_t* stack)
{
    if (stack == NULL)
        return NULL_PTR;

    if (stack->data == NULL)
        return SUCCESS;

#ifdef CANARY
    free(--stack->data);
#else
    free(stack->data);
#endif // CANARY
    stack->data = NULL;

    return SUCCESS;
}


StackError stackPush(Stack_t* stack, Element_t value)
{
    StackError error_code = stackAssert(stack);

    if (error_code != SUCCESS) 
        return error_code; 

    if (stack->size == stack->capacity) {
        error_code = stackExpand(stack);
        if (error_code != SUCCESS)
            return error_code;
    }

    stack->data[stack->size++] = value;
#ifdef HASH_ENABLED
    updateHashes(stack);
#endif // HASH_ENABLED

    return stackAssert(stack);
}


StackError stackPop(Stack_t* stack, Element_t* value)
{
    if (value == NULL)
        return NULL_PTR;

    StackError error_code = stackAssert(stack);
    if (error_code != SUCCESS)
        return error_code;
    if (stack->size == 0)
        return STACK_UNDERFLOW;

    *value = stack->data[--stack->size];
#ifdef POISON
    stack->data[stack->size] = POISON_VALUE;
#endif // POISON
#ifdef HASH_ENABLED
    updateHashes(stack);
#endif // HASH_ENABLED

    if (stack->size <= stack->capacity / 4 &&
        stack->capacity > START_CAPACITY)
        stackShrink(stack);

    return stackAssert(stack);
}
