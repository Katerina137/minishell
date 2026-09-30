#include <stdio.h>
#include <string.h>

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

        /* remove the trailing newline that fgets keeps */
        input[strcspn(input, "\n")] = '\0';

        /* ignore empty lines */
        if (input[0] == '\0') {
            continue;
        }

        printf("You entered: %s\n", input);
    }

    return 0;
}