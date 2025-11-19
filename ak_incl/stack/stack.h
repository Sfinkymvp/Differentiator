#ifndef INCLUDE_STACK_STACK_H_
#define INCLUDE_STACK_STACK_H_


#include "./data.h"
#include "./error.h"


#ifdef DEBUG
/// Макрос для инициализации отладочной информации стека
/// @param name Имя переменной, которая будет содержать стек
#define INIT(name) .debug_info = {#name, __func__, __FILE__, __LINE__}
#ifdef STRUCT_PROTECT
/// Макрос для объявления и инициализации стека
/// Определен в режиме STRUCT_PROTECT
/// @param name Имя переменной
#define INIT_STACK(name) Stack_t name = {.left_canary = STRUCT_LEFT_CANARY, INIT(name), \
                                         .right_canary = STRUCT_RIGHT_CANARY}
#else
/// Макрос для объявления и инициализации стека \n
/// Определен в режиме DEBUG
/// @param name Имя переменной
#define INIT_STACK(name) Stack_t name = {INIT(name)}
#endif // STRUCT_PROTECT in DEBUG
#endif // DEBUG


/// Создает стек указанной емкости
/// @param stack Указатель на структуру стека
/// @param start_capacity Начальная емкость
/// @return Код ошибки
StackError stackCtor(Stack_t* stack, size_t start_capacity);


/// Освобождает память стека
/// @param stack Указатель на структуру стека
/// @return Код ошибки
StackError stackDtor(Stack_t* stack);


/// Пушит значение в стек
/// @param stack Указатель на структуру стека
/// @param value Значение
/// @return Код ошибки
StackError stackPush(Stack_t* stack, Element_t value);


/// Берет значение из стека
/// @param stack Указатель на структуру стека
/// @param value Указатель на переменную для записи значения
/// @return Код ошибки
StackError stackPop(Stack_t* stack, Element_t* value);


#endif // INCLUDE_STACK_STACK_H_
