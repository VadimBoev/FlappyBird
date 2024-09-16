#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <stdint.h>

void Log(const char* fmt, ...);

//#if defined(__aarch64__)
uint64_t getTickCount();
//#else
//uint32_t getTickCount();
//#endif

#endif // UTILS_H