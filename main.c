#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024

/* return values for handle_builtin */
#define NOT_BUILTIN   0
#define BUILTIN_DONE  1
#define BUILTIN_EXIT -1

int handle_builtin(char *input) {
    if (strcmp(input, "exit") == 0) {
        return BUILTIN_EXIT;
    }

    if (strcmp(input, "cd") == 0) {
        char *home = getenv("HOME");
        if (home == NULL || chdir(home) != 0) {
            perror("cd");
        }
        return BUILTIN_DONE;
    }

    if (strcmp(input, "help") == 0) {
        printf("minishell built-in commands:\n");
        printf("  cd    change to home directory\n");
        printf("  help  show this message\n");
        printf("  exit  quit the shell\n");
        return BUILTIN_DONE;
    }

    return NOT_BUILTIN;
}

int main(void) {
    char input[MAX_INPUT];

    while (1) {
        printf("minishell> ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        /* remove trailing newline that fgets includes */
        input[strcspn(input, "\n")] = '\0';

        /* ignore empty lines */
        if (input[0] == '\0') {
            continue;
        }

        int builtin = handle_builtin(input);
        /* handle_builtin returns BUILTIN_EXIT if the user typed "exit" */
        if (builtin == BUILTIN_EXIT) {
            break;
        }
        if (builtin == BUILTIN_DONE) {
            continue;
        }

        pid_t pid = fork();

        if (pid == 0) {
            char *args[] = { input, NULL };
            execvp(args[0], args);
            perror("minishell");
            exit(1);
        } else {
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}