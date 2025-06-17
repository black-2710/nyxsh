#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"

int handle_builtin(char *input) {
    if (strcmp(input, "exit") == 0) {
        exit(0);
    }

    if (strcmp(input, "clear") == 0) {
        printf("\033[H\033[J");
        return 1;
    }

    if (strncmp(input, "cd", 2) == 0) {
        char *dir = strtok(input + 2, " \t");
        if (dir == NULL) dir = getenv("HOME");
        if (chdir(dir) != 0) {
            perror("nyxsh: cd");
        }
        return 1;
    }

    return 0;
}

