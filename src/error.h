#ifndef ERROR_H
#define ERROR_H

#include "state.h"


const char * e_get_error_message(int error_code);
FILE * e_open_log_file(const char *filename);
void e_log_message(FILE *handle, const char *message);
void e_close(FILE *handle);

#endif
