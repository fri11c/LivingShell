#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

Command parse_command(char input[]) {
    if (strcmp(input, "quit") == 0) {
        return CMD_QUIT;
    }
    else if (strcmp(input, "clear") == 0) {
        return CMD_CLEAR;
    }
    else {
        char *args[] = {input, NULL};
            pid_t pid = fork();

    if (pid == 0) {
        execvp(args[0], args);
        
        exit(1);
    } 
    else if (pid > 0) {
        wait(NULL);
        
    } 
    else {
        perror("fork failed");
    }
        
    }
}

int run_command(Command cmd) {
    switch (cmd) {
        case CMD_QUIT:
            return -1;
        break;
        case CMD_CLEAR:
            printf("\033[2J\033[H");
            return 1;
        break;
        default:
            return 0;
        break;
    }
}