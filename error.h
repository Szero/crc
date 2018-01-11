#ifndef ERROR_H
#define ERROR_H
#define DIE_VERBOSELY(format, ...) die("%s:%d in %s()\n" format,\
    __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DIE(format, ...) die(format, __VA_ARGS__)
#define DONT_DIE(format, ...) dont_die(format, __VA_ARGS__)
void die(const char*, ...);
void dont_die(const char*, ...);
#endif /* ERROR_H */
