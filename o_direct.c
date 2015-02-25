#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>


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

static int posix_flags(const char *mode)
{
    int flags = 0;
    int i;

    switch (*mode) {
        case 'r':
            flags = O_RDONLY;
            break;
        case 'w':
            flags = O_WRONLY;
            flags |= (O_CREAT|O_TRUNC);
            break;
        case 'a':
            flags = O_WRONLY;
            flags |= (O_CREAT|O_APPEND);
            break;
    }

    for (i = 0; i < 7 && *++mode; ++i) {
        switch (*mode) {
            case '+':
                flags |= O_RDWR;
                continue;
            case 'x':
                flags |= O_EXCL;
                continue;
            case 'e':
                flags |= O_CLOEXEC;
                continue;
        }
    }

    return flags;
}
FILE *fopen(const char *path, const char *mode)
{
    int pflags = posix_flags(mode);
    /**
     * XXX: leaked fd, and custom @mode
     */
    int fd = open(path, pflags, 0600);
    FILE *f = fdopen(fd, mode);
    return f;
}
