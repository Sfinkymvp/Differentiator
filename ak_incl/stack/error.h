#ifndef INCLUDE_STACK_ERROR_H_
#define INCLUDE_STACK_ERROR_H_


#include "./data.h"


#ifdef DEBUG
/// Макрос, вызывающий assert с отладочной информацией \n
/// Определен в режиме DEBUG
/// @param stack Указатель на структуру стека
#define stackAssert(stack) _stackAssert(stack, __func__, __FILE__, __LINE__)

#else

/// Макрос, вызывающий assert
/// Определен без режима DEBUG
/// @param stack Указатель на структуру стека
#define stackAssert(stack) _stackAssert(stack)

#endif // DEBUG


/// Строковые константы с информацией о кодах ошибок
extern const char* ErrorString[];


#ifdef DEBUG
/// Проверяет стек для удобной отладки \n
/// Определена в режиме DEBUG \n
/// С режимом HARD вызывает аварийное завершение программы
/// @param stack Указатель на структуру стека
/// @param function Строка с названием функции
/// @param file Строка с названием файла
/// @param line Номер строки
/// @return Код ошибки
StackError _stackAssert(Stack_t* stack,
                        const char* function,
                        const char* file,
                        int line);

#else

/// Проверяет стек \n
/// Определена без режима DEBUG \n
/// С режимом HARD вызывает аварийное завершение программы
/// @param stack Указатель на структуру стека
/// @return Код ошибки
StackError _stackAssert(Stack_t* stack);
#endif // DEBUG


/// Находит ошибки при работе стека
/// @param stack Указатель на структуру стека
/// @return Код ошибки
StackError stackVerify(const Stack_t* stack);


/// Распечатывает информацию о стеке при возникновении ошибки
/// @param stack Указатель на структуру стека
/// @param error_code Код ошибки
void stackDump(const Stack_t* stack, StackError error_code);


#ifdef STRUCT_PROTECT
/// Распечатывает информацию о канарейках в структуре стека \n
/// Определена в режиме STRUCT_PROTECT
/// @param stack Указатель на структуру стека
void printStructCanaryState(const Stack_t* stack);
#endif // STRUCT_PROTECT


#ifdef CANARY
/// Распечатывает информцию о канарейках в стеке \n
/// Определена в режиме CANARY
/// @param stack Указатель на структуру стека
void printStackCanaryState(const Stack_t* stack);
#endif // CANARY


#ifdef DEBUG
/// Распечатывает информацию об объявлении переменной со стеком \n
/// Определена в режиме DEBUG
/// @param stack Указатель на структуру стека
/// @param error_code Код ошибки
void printDebugInfo(const Stack_t* stack, StackError error_code);


/// Распечатывает содержимое стека \n
/// Определена в режиме DEBUG
/// @param stack Указатель на структуру стека
void printStackData(const Stack_t* stack);
#endif // DEBUG


#endif // INCLUDE_STACK_ERROR_H_
