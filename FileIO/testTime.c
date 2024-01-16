#include <stdio.h>
#include <time.h>
#include <unistd.h>

/* 天使标记是否存在 */
int g_debug = 0;

/* 日志：就是文件 */
/* 日志的初始化 */
void log_init()
{
    time_t now;

    /* 避免野指针 */
    struct tm *ptm = NULL;

#define DEBUG_FLAG "./my_debug.flag"

    /* access函数 成功返回0 */
    if (access(DEBUG_FLAG, F_OK) == 0)
    {
        g_debug = 1;
    }

    if (!g_debug)
    {
        return;
    }

#define DEBUG_FILE "./my_debug.log"

}

/* 日志的关闭 */
void log_close()
{



}

int main()
{
    struct  tm * info = localtime(NULL);
    printf("%02d %02d %02d:%02d\n",info->tm_mon+1,\
        info->tm_mday,info->tm_hour,info->tm_min);
    


    return 0;
}