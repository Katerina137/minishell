#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024

int main(void) {
    char input[MAX_INPUT];

    while (1) {
        printf("minishell> ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '\0') {
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            break;
        }

        pid_t pid = fork();

        if (pid == 0) {
            /* child: replace this process with the command */
            char *args[] = { input, NULL };
            execvp(args[0], args);

            /* only reached if execvp failed */
            perror("minishell");
            exit(1);
        } else {
            /* parent: wait for the child to finish */
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}