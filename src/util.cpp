#include <cstdarg>
#include <cstdint>

#include "util.hpp"

error_cleanup* set_cleanup = NULL;

void warnf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    if (set_cleanup != NULL) {
        set_cleanup();
    }
    exit(-1);
}

void error_set_cleanup(error_cleanup* cb_cleanup)
{
    if (set_cleanup != NULL) {
        warnf("re-set cleanup, lost: %p\n", set_cleanup);
    }
    set_cleanup = cb_cleanup;
}
