#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "parser.h"
#include "executor.h"
#include "builtins.h"

#define NYX_INPUT_SIZE 1024
#define HISTORY_FILE ".nyxhistory"

// Handle SIGCHLD to avoid zombie processes
void sigchld_handler(int signo) {
    (void)signo;  // Prevent unused parameter warning
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);
}

char *get_path_in_home(const char *filename) {
    const char *home = getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;

    char *path = malloc(PATH_MAX);
    snprintf(path, PATH_MAX, "%s/%s", home, filename);
    return path;
}

// Execute .nyxrc commands on startup
void run_nyxrc() {
    const char *home = getenv("HOME");
    if (!home) return;

    char rc_path[512];
    snprintf(rc_path, sizeof(rc_path), "%s/.nyxrc", home);

    if (access(rc_path, F_OK) != 0) return;

    pid_t pid = fork();
    if (pid == 0) {
        // Child: Run .nyxrc using bash
        execl("/bin/bash", "bash", rc_path, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent: wait for .nyxrc to complete
        waitpid(pid, NULL, 0);
    } else {
        perror("fork failed");
    }
}

int main() {
    char *input;
    char *history_path = get_path_in_home(HISTORY_FILE);

    // Load history and .nyxrc
    read_history(history_path);
    run_nyxrc();

    // Register SIGCHLD handler
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {
        // Dynamic prompt
        char cwd[PATH_MAX];
        char hostname[HOST_NAME_MAX];
        struct passwd *pw = getpwuid(getuid());

        getcwd(cwd, sizeof(cwd));
        gethostname(hostname, sizeof(hostname));

        char prompt[PATH_MAX + HOST_NAME_MAX + 100];
        snprintf(prompt, sizeof(prompt),
                 "\033[1;34m┌───(%s@%s)\033[0m-\033[1;32m[%s]\033[0m\n\033[1;34m└─$\033[0m ",
                 pw->pw_name, hostname, cwd);

        input = readline(prompt);

        if (input == NULL) {
            printf("\n");
            break;
        }

        if (strlen(input) == 0) {
            free(input);
            continue;
        }

        add_history(input);
        append_history(1, history_path);

        if (handle_builtin(input)) {
            free(input);
            continue;
        }

        int background = 0;
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '&') {
            background = 1;
            input[len - 1] = '\0';
            while (len > 1 && input[len - 2] == ' ') {
                input[len - 2] = '\0';
                len--;
            }
        }

        char **args = parse_input(input);
        if (args == NULL) {
            free(input);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            execute_command(args);
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            if (background) {
                printf("[bg] PID %d running in background\n", pid);
            } else {
                waitpid(pid, NULL, 0);
            }
        } else {
            perror("fork");
        }

        free_args(args);
        free(input);
    }

    write_history(history_path);
    free(history_path);
    return 0;
}
