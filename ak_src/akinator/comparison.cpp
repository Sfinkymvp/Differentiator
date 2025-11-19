#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>


#include "akinator/comparison.h"
#include "akinator/tree.h"
#include "stack/stack.h"
#include "akinator/colors.h"


static TreeStatus findPathToNode(Node* node, Stack_t* stack, const char* data)
{
    assert(stack); assert(data);

    StackError error = SUCCESS;
    if (node == NULL)
        return TREE_NOT_FOUND;
    if (strcmp(node->data, data) == 0) {
        error = stackPush(stack, (PathStep){node, ANSWER_NONE});
        RETURN_IF_STACK_ERROR(error);
        return TREE_OK;
    }

    TreeStatus status = findPathToNode(node->left, stack, data);
    if (status == TREE_OK) {
        error = stackPush(stack, (PathStep){node, ANSWER_YES});
        RETURN_IF_STACK_ERROR(error);
        return TREE_OK;
    }
    
    status = findPathToNode(node->right, stack, data);
    if (status == TREE_OK) {
        error = stackPush(stack, (PathStep){node, ANSWER_NO});
        RETURN_IF_STACK_ERROR(error);
        return TREE_OK;
    }

    return TREE_NOT_FOUND;
}


static TreeStatus printGeneralProperties(Stack_t* object_path_1, Stack_t* object_path_2)
{
    assert(object_path_1); assert(object_path_2);
    assert(object_path_1->data); assert(object_path_2->data);

    PathStep step_1 = {};
    PathStep step_2 = {};

    StackError error = stackPop(object_path_1, &step_1);
    RETURN_IF_STACK_ERROR(error);
    error = stackPop(object_path_2, &step_2);
    RETURN_IF_STACK_ERROR(error);

    printf("They both ");
    while (step_1.node == step_2.node && step_1.answer == step_2.answer) {
        if (step_1.answer == ANSWER_YES)
            printf(WHITE("%s"), step_1.node->data);
        else
            printf(WHITE("not %s"), step_1.node->data);
        if (step_1.answer != ANSWER_NONE && step_2.answer != ANSWER_NONE) {
            error = stackPop(object_path_1, &step_1);
            RETURN_IF_STACK_ERROR(error);
            error = stackPop(object_path_2, &step_2);
            RETURN_IF_STACK_ERROR(error);
            printf(", ");
        } else {
            break;
        }
    }
    printf(". ");

    error = stackPush(object_path_1, step_1);
    RETURN_IF_STACK_ERROR(error);
    error = stackPush(object_path_2, step_2);
    RETURN_IF_STACK_ERROR(error);

    return TREE_OK;
}


static TreeStatus printUniqueProperties(Stack_t* object_path, const char* data)
{
    assert(object_path); assert(object_path->data);

    PathStep step = {};
    StackError error = stackPop(object_path, &step);
    RETURN_IF_STACK_ERROR(error);

    printf("%s ", data);
    while (step.answer != ANSWER_NONE) {
        assert(step.answer != ANSWER_YES || step.answer != ANSWER_NO);
        if (step.answer == ANSWER_YES)
            printf(WHITE("%s"), step.node->data);
        else
            printf(WHITE("not %s"), step.node->data);

        error = stackPop(object_path, &step);
        RETURN_IF_STACK_ERROR(error);
        if (step.answer != ANSWER_NONE)
            printf(", ");
    }
    printf(". ");

    return TREE_OK;
}


static TreeStatus printNodesComparision(Stack_t* object_path_1, Stack_t* object_path_2,
                                        const char* data_1, const char* data_2)
{
    assert(object_path_1); assert(object_path_2);
    assert(object_path_1->data); assert(object_path_2->data);

   // printf("General properties of both objects:\n");
    TreeStatus status = printGeneralProperties(object_path_1, object_path_2);
    RETURN_IF_NOT_OK(status);

   // printf("\nUnique properties of 1 object:\n");        
    status = printUniqueProperties(object_path_1, data_1);
    RETURN_IF_NOT_OK(status);

   // printf("\nUnique properties of 2 object:\n");
    status = printUniqueProperties(object_path_2, data_2);
    RETURN_IF_NOT_OK(status);
    printf("\n");

    return TREE_OK;
}


TreeStatus compareNodes(BinaryTree* tree, const char* data_1, const char* data_2)
{
    assert(tree); assert(data_1); assert(data_2);

    Stack_t stack_1 = {};
    Stack_t stack_2 = {};

    StackError error = stackCtor(&stack_1, START_CAPACITY);
    RETURN_IF_STACK_ERROR(error);
    error = stackCtor(&stack_2, START_CAPACITY);
    RETURN_IF_STACK_ERROR(error);

    if (findPathToNode(tree->root, &stack_1, data_1) == TREE_NOT_FOUND ||
        findPathToNode(tree->root, &stack_2, data_2) == TREE_NOT_FOUND) {
        error = stackDtor(&stack_1);
        RETURN_IF_STACK_ERROR(error);
        error = stackDtor(&stack_2);
        RETURN_IF_STACK_ERROR(error);

        return TREE_NOT_FOUND;
    }

    printf(CYAN("\nComparison of nodes '") WHITE("%s") CYAN("' and '") WHITE("%s")
           CYAN("'\n"), data_1, data_2);
    TreeStatus status = printNodesComparision(&stack_1, &stack_2, data_1, data_2);

    error = stackDtor(&stack_1);
    RETURN_IF_STACK_ERROR(error);
    error = stackDtor(&stack_2);
    RETURN_IF_STACK_ERROR(error);

    return status;
}


TreeStatus defineNode(BinaryTree* tree, const char* data)
{
    assert(tree); assert(data);

    Stack_t stack = {};

    StackError error = stackCtor(&stack, START_CAPACITY);
    RETURN_IF_STACK_ERROR(error);

    TreeStatus status = findPathToNode(tree->root, &stack, data);
    if (status != TREE_OK) {
        error = stackDtor(&stack);
        RETURN_IF_STACK_ERROR(error);

        return status;
    }

    printf(CYAN("\nNode '") WHITE("%s") CYAN("' definition:\n"), data);
    status = printUniqueProperties(&stack, data);
    printf("\n");
    
    error = stackDtor(&stack);
    RETURN_IF_STACK_ERROR(error);

    return status;
}

