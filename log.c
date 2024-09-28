/* ***********************************************************************
> File Name:   log.c
> Author:      Asch
> GitHub:      https://github.com/Asch-MJ
> Blog:        https://www.asch.blog/
 ************************************************************************/

#ifndef LOG_C
#define LOG_C

#include "log.h"
#include "nty_coroutine.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#if ENABLE_LOG
void log_message(const char *file, int line, const char *fmt, ...)
{
    FILE *file_ptr = fopen(LOG_FILE, "a");
    if (!file_ptr)
    {
        perror("Failed to open log file");
        return;
    }

    // 检查当前日志文件的大小
    fseek(file_ptr, 0, SEEK_END); // 移动到文件末尾
    long file_size = ftell(file_ptr);

    // 如果超过最大限制，则进行回滚处理
    if (file_size >= MAX_LOG_SIZE)
    {
        fclose(file_ptr);                // 先关闭已经打开的文件
        rename(LOG_FILE, "log_old.txt"); // 重命名旧的日志

        // 重新打开新日志文件以便写入
        file_ptr = fopen(LOG_FILE, "a");
        if (!file_ptr)
        {
            perror("Failed to create new log file");
            return;
        }
    }
    else
    {
        rewind(file_ptr); // 如果没有超过限制则返回到开头
    }

    va_list args;        // 定义一个可变参数列表
    va_start(args, fmt); // 初始化args，fmt是最后一个固定参数

    // 获取当前时间并格式化为字符串，包括微秒
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // 将时间戳转换为本地时间结构体
    struct tm *t = localtime(&tv.tv_sec);

    char timestamp[30]; // YYYY-MM-DD HH:MM:SS.micros 格式
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    // 将微秒部分拼接到时间戳后面
    sprintf(timestamp + strlen(timestamp), ".%06ld", tv.tv_usec);

    fprintf(file_ptr, "[%s] [%s:%d]: ", timestamp, file, line);

    vfprintf(file_ptr, fmt, args); // 输出格式化信息到日志中
    fprintf(file_ptr, "\n");

    va_end(args); // 清理工作

    fclose(file_ptr);
}
#endif
#endif
