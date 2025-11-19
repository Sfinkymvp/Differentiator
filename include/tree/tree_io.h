#ifndef INCLUDE_AKINATOR_IO_H_
#define INCLUDE_AKINATOR_IO_H_


#include "./data.h"


void printSeparator();


TreeStatus treeWriteToDisk(BinaryTree* tree);


TreeStatus treeLoadFromDisk(BinaryTree* tree);


TreeStatus readNode(BinaryTree* tree, Node** node, int* position);


TreeStatus parseArgs(BinaryTree* tree, const int argc, const char** argv);


#endif // INCLUDE_AKINATOR_IO_H_
