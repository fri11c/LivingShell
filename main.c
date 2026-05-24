#include "command.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    char input[100];
    bool running = true;

    printf("\033[2J\033[H");

    while (running == true) {
        

        printf("$");
        
        fgets(input, 100, stdin);

        int parsed_size = strcspn(input, "\n");
        input[parsed_size] = '\0';
        
        Command cmd = parse_command(input);

        if (cmd == CMD_NOT_FOUND) {
            printf("LSH: %s: Command does not exist.\n", input);
        }
        int exit_code = run_command(cmd);

        if (exit_code < 0) {
            running = false;
        }
    }
}