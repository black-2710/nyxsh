#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

#define MAX_TOKENS 64
#define DELIM " \t\r\n"

char **parse_input(char *input) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (!tokens) {
        perror("nyxsh: malloc");
        return NULL;
    }

    int position = 0;
    char *token = strtok(input, DELIM);
    while (token != NULL && position < MAX_TOKENS - 1) {
        tokens[position++] = strdup(token);
        token = strtok(NULL, DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

void free_args(char **args) {
    if (!args) return;
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

