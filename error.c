#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdio.h>

#ifdef __GNUC__
__attribute__ ((format(__printf__, 1, 2)))
#endif
noreturn void die(const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    vfprintf(stderr, format, vargs);
    va_end(vargs);
    exit(EXIT_FAILURE);
}

#ifdef __GNUC__
__attribute__ ((format(__printf__, 1, 2)))
#endif
void dont_die(const char *format, ...)
{
    va_list vargs;
    va_start(vargs, format);
    vfprintf(stderr, format, vargs);
    va_end(vargs);
}
