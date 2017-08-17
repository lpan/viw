#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

/**
 * returns error message string corresponds to the given error code 
 * TODO: support more error types
 */
const char * e_get_error_message(int error_code){
    switch(error_code){
        case 37:
            return "E37: No write since last change (add ! to override)";
        default: 
            return "";
    }
}

FILE * e_open_log_file(const char *filename){
    FILE *handle = fopen(filename, "a");
    if(handle == NULL){
        fprintf(stderr, "An error has occured: Unable to open log file %s\n", filename);
        exit(1);
    }
    return handle;
}

void e_log_message(FILE *handle, const char *message){
    assert(handle);
    time_t rawtime; 
    struct tm * time_info;
    time(&rawtime);
    time_info = localtime(&rawtime);
    char *time_string = asctime(time_info);
    time_string[strlen(time_string) - 1] = 0;
    fprintf(handle, "[%s] %s\n\r", time_string, message);
}

void e_close(FILE *handle){
    fclose(handle);
}