#include "command.h"
#include "./lib/error_handler/error_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAX_ARGS 64
char *remove_quotes(char *token, int len) {
    char *token_buffer = malloc(len +1);
    strncpy(token_buffer, token, len);
    if ((token_buffer[0] == '\'' && token_buffer[len-1] == '\'') || (token_buffer[0] == '\"' && token_buffer[len-1] == '\"')) {
        int i = 0, j = 0;

        while (token_buffer[i] == '\'' || token_buffer[i] == '\"') i++;

        while ((token_buffer[j++] = token_buffer[i++]));

        token_buffer[len -2] = '\0';
    }
    return token_buffer;
}
char **parse_input(char *input) {
    char **argv = malloc(sizeof(char*) * MAX_ARGS);
    int argc = 0;

    char *p = input;

    bool in_s_quotes = false;
    bool in_d_quotes = false;

    while (*p != '\0') {

        // skip spaces
        while (*p == ' ' || *p == '\n')
            p++;

        if (*p == '\0')
            break;


        
        // start of token
        char *start = p;

        if ((in_s_quotes || in_d_quotes)  && (*p == '\0' || *p == '\n')) {
            break;
        }
        // move until space or end
        while ((*p != ' ' || in_s_quotes || in_d_quotes) && *p != '\n' && *p != '\0' ) {
            if (*p == '\"') 
                in_d_quotes = !in_d_quotes;
            if (*p == '\'')
                in_s_quotes = !in_s_quotes;
    
            p++;
        }

        int len = p - start;

        char *token = malloc(len + 1);
        strncpy(token, remove_quotes(start, len), len);

        
        token[len] = '\0';

        argv[argc++] = token;
        
    }
    argv[argc] = NULL; 
    
    return argv;
}
char **parse_command(char input[]) {
    char **args = malloc(sizeof(char) * MAX_ARGS);
    args = parse_input(input);
    if (strcmp(args[0], "quit") == 0) {
        args[0] = "LSH";
        args[1] = "quit";
    }
    else if (strcmp(args[0], "clear") == 0) {
        args[0] = "LSH";
        args[1] = "clear";
    }
    else if (strcmp(args[0], "cd") == 0) {
        args[2] = args[1];
        args[0] = "LSH";
        args[1] = "cd";
    }
    return args;      
}

int run_command(char **args) {
    if (strcmp(args[0], "LSH") == 0) {
        // internal command
        char *available_commands[] = {"quit", "clear", "cd", NULL};
        
        COMMAND id;
        for (int i = 0; available_commands[i] != NULL; i++) {
            if(strcmp(args[1], available_commands[i]) == 0) {
               id = i;
            }
        }
    
        switch(id) {
            case QUIT:  
                return -1;
                break;
            case CLEAR:  
                printf("\033[2J\033[H");
                return 0;
                break;
            case CD:
                if(chdir(args[2]) < 0) {
                   
                    return 0;
                    error(ERROR, args[2], false);
                    
                }
                return 0;
                break;
        }
        return 0;
    }
    else {
        pid_t pid = fork(); 

        if (pid == 0) {
            execvp(args[0], args); 
            error(ERROR, args[0], true);
            exit(1);
        } 
        else if (pid > 0) { 
            wait(NULL);
        } 
        else {
            error(ERROR, args[0], true);
        }
    }
    return 0;
}