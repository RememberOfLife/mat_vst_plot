#pragma once

#include <cstdio>
#include <cstdint>
#include <cstdlib>

void warnf(const char* fmt, ...);
void errorf(const char* fmt, ...);
typedef void error_cleanup(void);
void error_set_cleanup(error_cleanup* cb_cleanup);
