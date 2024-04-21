#ifndef C_CALENDAR_DEBUG_H
#define C_CALENDAR_DEBUG_H

#ifndef OPEN_DEBUG
#define OPEN_DEBUG 1
#endif
//打开调试功能
#if defined(OPEN_DEBUG) && OPEN_DEBUG == 1

#include <stdio.h>

#define Trace(stream, level, ...)   fprintf(stream, "[%s] %s: %s, %s(), %-4d> ", level, __TIMESTAMP__, __FILE__, __FUNCTION__, __LINE__); \
                                    fprintf(stream, __VA_ARGS__)
//调试输出
#define Debug(...)                  Trace(stdout, "Debug", __VA_ARGS__)
//错误输出
#define Error(...)                  Trace(stdout, "Error", __VA_ARGS__)
//信息输出
#define Message(...)                Trace(stdout, "Message", __VA_ARGS__)
//记录日志
#define Log(log_file_path, ...) \
    do { \
        FILE * _log_file_= fopen(log_file_path,"a+"); \
        if (_log_file_) { \
            Trace(_log_file_, "Log", __VA_ARGS__); \
        } \
        fclose(_log_file_); \
    } while(0)
#else

//调试输出
#define Debug(...)
//错误输出
#define Error(...)
//信息输出
#define Message(...)
//记录日志
#define Log(...)

#endif

#endif //C_CALENDAR_DEBUG_H
