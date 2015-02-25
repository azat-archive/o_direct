#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>


int (*open_orig)(const char *pathname, int flags, mode_t mode) = NULL;

__attribute__((constructor)) static void open_init(void)
{
    open_orig = dlsym(RTLD_NEXT, "open");
}

int open(const char *pathname, int flags, ...)
{
    mode_t mode;
    flags |= O_DIRECT;
    /** Doh */
    if (flags & O_CREAT) {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, int);
        va_end(arg);
    }
    return open_orig(pathname, flags, mode);
}
