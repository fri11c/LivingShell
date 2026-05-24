#ifndef COMMAND_H
#define COMMAND_H


typedef enum  {
        CMD_QUIT,
        CMD_CLEAR,
        CMD_NOT_FOUND,
} Command;

Command parse_command(char input[]);
int run_command(Command cmd);

#endif