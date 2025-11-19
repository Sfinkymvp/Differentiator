#ifndef INCLUDE_STACK_DATA_H_
#define INCLUDE_STACK_DATA_H_


#include "../akinator/tree.h"


/// Начальная емкость стека
const int START_CAPACITY = 3;


/// Тип данных в стеке
typedef PathStep Element_t;


/// Зеленый цвет для вывода в поток
#define GR "\x1b[32m"
/// Цвет по умолчанию для вывода в поток
#define DF "\x1b[0m"


#if defined(HASH) || defined(STRUCT_PROTECT)
#define HASH_ENABLED
#endif


#ifdef POISON
/// Значение яда в режиме POISON
const Element_t POISON_VALUE = 0x1DF1E1D;
#endif // POISON
#ifdef CANARY
/// Значение левой канарейки стека в режиме CANARY
const Element_t LEFT_CANARY = 0xFF1C1A1;
/// Значение правой канарейки стека в режиме CANARY
const Element_t RIGHT_CANARY = 0xFF51DE5;
#endif // CANARY
#ifdef STRUCT_PROTECT
/// Значение левой канарейки структуры стека в режиме STRUCT_PROTECT
const size_t STRUCT_LEFT_CANARY = 0xFF10AD;
/// Значение правой канарейки структуры стека в режиме STRUCT_PROTECT
const size_t STRUCT_RIGHT_CANARY = 0x0DE55A;
#endif // STRUCT_PROTECT


/// Коды ошибок для операций со стеком
typedef enum {
    SUCCESS = 0,                 ///< Ошибок нет
    NULL_PTR,                    ///< Указатель на стек или структуру равен NULL
    OUT_OF_MEMORY,               ///< Ошибка выделения памяти
    STACK_UNDERFLOW,             ///< Попытка использования пустого стека
    STACK_OVERFLOW,              ///< Превышение вместимости стека
    CANARY_CORRUPTED,            ///< Испорчены канарейки стека
    POISON_CORRUPTED,            ///< Испорчены значения POISON
    HASH_CORRUPTED,              ///< Несовпадение хеша в стеке
    STRUCT_CANARY_CORRUPTED,     ///< Испорчены канарейки в структуре стека
    STRUCT_HASH_CORRUPTED        ///< Несовпадение хеша в структуре стека
} StackError;


#ifdef DEBUG
/// Отладочная информация о месте создания стека
typedef struct {
    const char* variable_name;   ///< Имя переменной стека
    const char* function_name;   ///< Имя функции, где создан стек
    const char* file_name;       ///< Имя файла, где создан стек
    int line;                    ///< Номер строки в файле
} StackDebugInfo;
#endif // DEBUG


typedef struct {
#ifdef STRUCT_PROTECT
    size_t left_canary;          ///< Левая канарейка в структуре
#endif // STRUCT_PROTECT
    Element_t* data;             ///< Указатель на массив данных стека
    size_t size;                 ///< Размер стека
    size_t capacity;             ///< Емкость стека
#ifdef DEBUG
    StackDebugInfo debug_info;   ///< Отладочная информация о создании структуры
#endif // DEBUG
#ifdef HASH
    size_t hash;                 ///< Хеш стека
#endif // HASH
#ifdef STRUCT_PROTECT
    size_t struct_hash;          ///< Хеш структуры
    size_t right_canary;         ///< Правая канарейка в структуре
#endif // STRUCT_PROTECT
} Stack_t;


#endif // INCLUDE_STACK_DATA_H_
