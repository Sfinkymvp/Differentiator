#include <stdio.h>


#include "akinator/tree.h"
#include "akinator/colors.h"


int main(const int argc, const char** argv)
{
    TREE_INIT(tree);
    int exit_code = 0;

    TreeStatus status = treeConstructor(&tree, argc, argv);
    if (status != TREE_OK) {
        printStatusMessage(status);
        exit_code = 1;
    }

    if (exit_code == 0) {
        printf(CYAN("Hello, I'm akinator!\n"));
      
        UserChoice choice = CHOICE_GUESS;
        while (choice != CHOICE_EXIT_SAVE && choice != CHOICE_EXIT_NO_SAVE) {
            choice = parseUserChoice();

            status = processUserChoice(&tree, choice);
            if (status != TREE_OK) {
                printStatusMessage(status);
                exit_code = 1;
                break;
            }
        }
    }

    printf(CYAN("\nAkinator says goodbye!\n"));
    treeDestructor(&tree);
    return exit_code;
}
