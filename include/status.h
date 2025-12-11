#ifndef STATUS_H_
#define STATUS_H_


typedef enum {
// Successful status
    STATUS_OK = 0,
// Tree Errors
    STATUS_TREE_ROOT_HAS_PARENT,
    STATUS_TREE_MISSING_PARENT,
    STATUS_TREE_PARENT_CHILD_MISMATCH,
    STATUS_TREE_INVALID_BRANCH_STRUCTURE,
// Differentiation Errors
    STATUS_DIFF_CALCULATE_ERROR,
    STATUS_DIFF_UNKNOWN_VARIABLE,
// System Errors
    STATUS_SYSTEM_OUT_OF_MEMORY,
    STATUS_SYSTEM_CALL_ERROR,
// Command Line Argument Errors
    STATUS_CLI_UNKNOWN_OPTION,
// Syntax Errors
    STATUS_PARSER_INVALID_IDENTIFIER,
// I/O Errors
    STATUS_IO_INVALID_USER_INPUT,
    STATUS_IO_FILE_OPEN_ERROR,
    STATUS_IO_FILE_EMPTY,
    STATUS_IO_FILE_READ_ERROR,
    STATUS_IO_FILE_CLOSE_ERROR
} OperationStatus;


typedef struct {
    OperationStatus status;
    const char* status_string;
    const char* error_message;
} ErrorInfo;


const size_t ERROR_COUNT = STATUS_IO_FILE_CLOSE_ERROR + 1;
extern const ErrorInfo ErrorTable[ERROR_COUNT];


#define RETURN_IF_STATUS_NOT_OK(status) \
    do {                                \
        if (status != STATUS_OK)        \
            return status;              \
    } while (0)


#endif // STATUS_H_