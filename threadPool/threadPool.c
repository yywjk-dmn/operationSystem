#include "threadPool.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

#define DEFAULT_MIN_THREADS 5
#define DEFAULT_MAX_THREADS 10
#define DEFAULT_QUEUE_CAPACITY 100

enum STSTUS_CODE
{
    ON_SUCCESS,
    NULL_PTR,
    MALLOC_ERROR,
    ACCESS_INVALID,
    UNKNOW_ERROR,
};

void * threadHander(void *arg)
{
    pthread_exit(NULL);

}
/* 线程池的初始化 */
int threadPoolInit(threadpool_t *pool, int minThreads, int maxThreads, int queueCapacity)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    do
    {
        /* 判断线程合法性 */
        if (minThreads < 0 || maxThreads < 0 || minThreads >= maxThreads)
        {
            minThreads = DEFAULT_MIN_THREADS;
            maxThreads = DEFAULT_MAX_THREADS;
        }
        /* 更新线程池属性 */
        pool->minThreads = minThreads;
        pool->maxThreads = maxThreads;

        /* 判断任务容量合法性 */
        if (queueCapacity <= 0)
        {
            queueCapacity = DEFAULT_QUEUE_CAPACITY;
        }
        /* 更新任务容量属性 */
        pool->queueCapacity = queueCapacity;
        pool->queueFront = 0;
        pool->queueRear = 0;
        pool->queueSize = 0;
        pool->taskQueue = (task_t *)malloc(sizeof(task_t) * pool->queueCapacity);
        if (pool->queueCapacity == NULL)
        {
            perror("malloc error");
            break;
        }
        /* 清除脏数据 */
        memset(pool->queueCapacity, 0, sizeof(task_t) * pool->queueCapacity);
      

        /* 为线程ID分配堆空间 */
        pool->threadId = (pthread_t)malloc(sizeof(pthread_t) * maxThreads);
        if (pool->threadId == NULL)
        {
            perror("malloc error");
            exit(-1);

        }
        /* 清除脏数据 */
        memset(pool->threadId, 0, sizeof(pthread_t) * maxThreads);

        int ret = 0;
        /* 创建线程 */
        for (int idx = 0; idx < pool->minThreads; idx++)
        {
            if (pool->threadId[idx] == 0)
            {
                ret = pthread_create(&(pool->threadId[idx]), NULL, threadHander, NULL);
                if (ret != 0)
                {
                    perror("thread create error");
                    break;
                }
            }
            if (ret != 0)
            {

            }
           
        }
        return ON_SUCCESS;
    }
    while (0);
    /* 回收任务资源 */
    if (pool->queueCapacity != NULL)
    {
        free(pool->queueCapacity);
        pool->queueCapacity = NULL;

    }
    /* 回收线程资源 */
    for (int idx = 0 ; idx < pool->minThreads; idx++)
    {
        if (pool->threadId[idx] != 0)
        {
            pthread_join(pool->threadId[idx], NULL);
        }

    }

    /* 程序执行到这里 上面一定有执行错误的地方 */
    if (pool->threadId != NULL)
    {
        free(pool->threadId);
        pool->threadId = NULL;
    }
   
    return UNKNOW_ERROR;

}

/* 线程池的销毁 */
int threadPoolDestory(threadpool_t *pool)
{

}