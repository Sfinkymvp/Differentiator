#ifndef STATUS_H_
#define STATUS_H_


typedef enum {
// Успешный статус
    STATUS_OK = 0,
// Ошибки дерева
    STATUS_TREE_ROOT_HAS_PARENT,
    STATUS_TREE_MISSING_PARENT,
    STATUS_TREE_PARENT_CHILD_MISMATCH,
    STATUS_TREE_INVALID_BRANCH_STRUCTURE,
// Ошибки системного характера
    STATUS_SYSTEM_OUT_OF_MEMORY,
// Ошибки аргументов командной строки
    STATUS_CLI_UNKNOWN_OPTION,
// Ошибки связанные с синтаксисом
    STATUS_PARSER_INVALID_IDENTIFIER,
// Ошибки ввода/вывода
    STATUS_IO_FILE_OPEN_ERROR,
    STATUS_IO_FILE_EMPTY,
    STATUS_IO_FILE_READ_ERROR,
    STATUS_IO_FILE_CLOSE_ERROR
} OperationStatus;


#define RETURN_IF_STATUS_NOT_OK(status) \
    do {                                \
        if (status != STATUS_OK)        \
            return status;              \
    } while (0)


#endif // STATUS_H_