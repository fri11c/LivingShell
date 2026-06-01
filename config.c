#include "config.h"
#include "./lib/error_handler/error_handler.h"
#include "./lib/shell_specs/shell_specs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./lib/toml17/tomlc17.h"

#define MAX_ARGS 64

typedef struct {
    const char* key;
    char** target_ptr; 
} ConfigMapping;


char *var_of(char *arg) {
    if (getenv(arg) != NULL) {
        return getenv("USER");
    }
    else if (get_shell_spec(arg) != NULL) {
        return get_shell_spec(arg);
    }
    else {
        return arg;
    }
}

CONFIG get_config(void) {
    CONFIG config;

    char *home = getenv("HOME");

    if (home == NULL) {
        error(ERROR, "Home folder could NOT be automatically identified", true);
    }
    char *file = "/.config/lsh/lsh.toml";

    char *file_path = malloc(strlen(home) + strlen(file) +1);
    sprintf(file_path, "%s%s", home, file);

    toml_result_t result = toml_parse_file_ex(file_path);

    if (!result.ok) {
        toml_free(result);
        error(WARNING, "Parsing of the config file failed", true);
        // TODO: implement fallback
        
        return config;
    }
    

    ConfigMapping mappings[] = {
        {"prompt",          &config.prompt},
        {"welcome_message", &config.welcome_message},
        
        {NULL,              NULL} 
    };

    
     // TODO: use dynamic size

    for (int i = 0; mappings[i].key != NULL; i++) {
        
        toml_datum_t buffer = toml_seek(result.toptab, mappings[i].key);
        

        if (buffer.type == TOML_STRING) {
            
            *mappings[i].target_ptr = malloc(strlen(buffer.u.s) + 1);
            if (*mappings[i].target_ptr != NULL) {
                
                char* temp = malloc(strlen(buffer.u.s) +1);
                strcpy(temp, buffer.u.s);
                
                
                char* token = strtok(temp, "{}");

                int n = 0;
                while (token != NULL) {
                    n += strlen(var_of(token));
                    token = strtok(NULL, "{}");
                }
                strcpy(temp, buffer.u.s);
                char* token2 = strtok(temp, "{}");

                char *config_inc_vars = malloc(n +1);
                strcpy(config_inc_vars, "");

                
                while (token2 != NULL) {
                    sprintf(config_inc_vars + strlen(config_inc_vars), "%s", var_of(token2));
                    token2 = strtok(NULL, "{}");
                }

                strcpy(*mappings[i].target_ptr, config_inc_vars);
                free(config_inc_vars);

            }
        }
        else {
            error(WARNING, "Some of the config types do not match. Standart will be used", false);
        }
    }
    

    toml_free(result);
    
    return config;
}
