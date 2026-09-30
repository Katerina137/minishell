#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS  64

/* return values for handle_builtin */
#define NOT_BUILTIN   0
#define BUILTIN_DONE  1
#define BUILTIN_EXIT -1

/* split input on spaces and tabs into a NULL-terminated array */
int parse_input(char *input, char **args) {
    int count = 0;
    char *token = strtok(input, " \t");

    /* leave room for the NULL that execvp needs at the end */
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t");
    }

    args[count] = NULL;
    return count;
}

int handle_builtin(char **args, int *exit_code) {
    /* built-in: leave the shell, with an optional exit code */
    if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) {
            *exit_code = atoi(args[1]);
        }
        return BUILTIN_EXIT;
    }

    /* built-in: must run in the shell itself, a child can't move its parent */
    if (strcmp(args[0], "cd") == 0) {
        char *path = args[1] ? args[1] : getenv("HOME");
        if (path == NULL || chdir(path) != 0) {
            perror("cd");
        }
        return BUILTIN_DONE;
    }

    /* built-in: list the available built-ins */
    if (strcmp(args[0], "help") == 0) {
        printf("minishell built-in commands:\n");
        printf("  cd [dir]     change directory (home if no dir)\n");
        printf("  help         show this message\n");
        printf("  exit [code]  quit the shell\n");
        return BUILTIN_DONE;
    }

    return NOT_BUILTIN;
}

int main(void) {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    int exit_code = 0;

    while (1) {
        printf("minishell> ");
        fflush(stdout);  /* prompt has no newline, so force it to show */

        /* NULL means EOF (Ctrl+D) or a read error: leave cleanly */
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        /* remove the trailing newline that fgets keeps */
        input[strcspn(input, "\n")] = '\0';

        /* ignore empty lines, including lines that are only spaces */
        if (parse_input(input, args) == 0) {
            continue;
        }

        /* built-ins run in the shell process, no fork needed */
        int builtin = handle_builtin(args, &exit_code);
        if (builtin == BUILTIN_EXIT) {
            break;
        }
        if (builtin == BUILTIN_DONE) {
            continue;
        }

        pid_t pid = fork();

        if (pid == 0) {
            /* child: replace this process with the command */
            execvp(args[0], args);

            /* only reached if execvp failed */
            perror("minishell");
            exit(1);
        } else {
            /* parent: wait for the child to finish */
            waitpid(pid, NULL, 0);
        }
    }

    return exit_code;
}