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

//#if defined(__aarch64__)
uint64_t getTickCount()
//#else
//uint32_t getTickCount()
//#endif
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

//#if defined(__aarch64__)
    uint64_t time = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
//#else
 //   uint32_t time = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
//#endif 
    return time;
}