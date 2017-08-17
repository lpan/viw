#ifndef ERROR_H
#define ERROR_H

#include "state.h"

#define AUTO_RELEASE_LOG_BLOCK(block) { \
    FILE *handle = e_open_log_file("debug.log"); \
    block \
    fclose(handle); \
}

#define E_LOG_ERR(string) {\
    assert(handle); \
    e_log_message(handle, string);\
}

const char * e_get_error_message(int error_code);
FILE * e_open_log_file(const char *filename);
void e_log_message(FILE *handle, const char *message);
void e_close(FILE *handle);

#endif
