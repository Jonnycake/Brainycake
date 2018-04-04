#include <stdio.h>
#include <stdarg.h>

void write_log(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    printf("[%d] ", time(NULL));
    vprintf(msg, args);

    va_end(args);
}
