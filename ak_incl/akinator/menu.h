#ifndef INCLUDE_AKINATOR_MENU_H_
#define INCLUDE_AKINATOR_MENU_H_


#include "./data.h"


TreeStatus akinatorDefine(BinaryTree* tree);


TreeStatus akinatorCompare(BinaryTree* tree);


UserChoice parseUserChoice();


TreeStatus processUserChoice(BinaryTree* tree, UserChoice choice);


#endif // INCLUDE_AKINATOR_MENU_H_
