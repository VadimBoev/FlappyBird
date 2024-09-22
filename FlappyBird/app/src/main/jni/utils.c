#include "utils.h"
#include <android/log.h>
#include <sys/time.h>

void Log(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, "flappy", fmt, args);
    va_end(args);
}

uint64_t getTickCount() 
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / 1000000;
}