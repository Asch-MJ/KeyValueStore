/* ***********************************************************************
> File Name:   log.h
> Author:      Asch
> GitHub:      https://github.com/Asch-MJ
> Blog:        https://www.asch.blog/
 ************************************************************************/
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "nty_coroutine.h"

#define LOG_FILE "log.txt"
#define MAX_LOG_SIZE (10 * 1024 * 1024) // 设置最大日志文件大小为1MB

// 如果启用了日志记录，则定义日志相关功能
#if ENABLE_LOG
void log_message(const char *file, int line, const char *fmt, ...);
#define LOG(fmt, ...) log_message(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif

#endif // LOG_H
