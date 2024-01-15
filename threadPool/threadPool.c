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
    /* 强转类型转换 */
    threadpool_t *pool = (threadpool_t *)arg;
    /* 本质是个消费者 */
    while (1)
    {
        pthread_mutex_lock(&(pool->mutexpool));
        while (pool->queueSize == 0)
        {
            /* 等待一个条件变量 生产者发出已经生产好的信号 */
            pthread_cond_wait(&(pool->notEmpty), &(pool->mutexpool));
        }
        /* 意味着队列有任务 */
        task_t tempTask = pool->taskQueue[pool->queueFront];
        /* 循环队列 */
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        /* 任务数减1 */
        pool->queueSize--;
        /* 解锁 */
        pthread_mutex_unlock(&(pool->mutexpool));

        /* 发送信号给生产者 继续生产 */
        pthread_cond_signal(&(pool->notFull));

        /* 为了提升性能 要加一把专门用来维护busy的锁 */
        pthread_mutex_lock(&(pool->mutexBusy));
        pool->busyThreadNums++;
        pthread_mutex_unlock(&(pool->mutexBusy));

        /* 执行钩子 */
        tempTask.worker_hander(tempTask.arg);

        pthread_mutex_lock(&(pool->mutexBusy));
        pool->busyThreadNums--;
        pthread_mutex_unlock(&(pool->mutexBusy));

    }
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

        /* 初始化时，忙碌的线程数为0 */


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
                ret = pthread_create(&(pool->threadId[idx]), NULL, threadHander, pool);
                if (ret != 0)
                {
                    perror("thread create error");
                    break;
                }
            }
        }
        if (ret != 0)
        {
            break;
        }
        pool->liveThreadNums = pool->minThreads;

        /* 初始化锁资源 */
        pthread_mutex_init(&(pool->mutexpool), NULL);
        pthread_mutex_init(&(pool->mutexBusy), NULL);

        /* 初始化条件变量 */
        if (pthread_cond_init(&(pool->notEmpty), NULL) != 0 || pthread_cond_init(&(pool->notFull), NULL) != 0)
        {
            perror("thread cond error");
            break;
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

    /* 回收锁资源 */
    pthread_mutex_destroy(&(pool->mutexpool));
    pthread_mutex_destroy(&(pool->mutexBusy));
    /* 释放条件变量的资源 */
    pthread_cond_destroy(&(pool->notEmpty));
    pthread_cond_destroy(&(pool->notFull));

   
    return UNKNOW_ERROR;

}

/* 线程池添加任务 */
int threadPoolAddTask(threadpool_t *pool, void *(*worker_hander)(void *arg), void *arg)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    /* 加锁 */
    pthread_mutex_lock(&(pool->mutexpool));

    /* 任务队列满了 */
    if (pool->queueSize == pool->queueCapacity)
    {
        pthread_cond_wait(&(pool->notFull), &(pool->mutexpool));
    }
    /* 程序到这个地方 一定有空位置放任务 */
    /* 将任务放到队列的队尾  */
    pool->taskQueue[pool->queueRear].worker_hander = worker_hander;
    pool->taskQueue[pool->queueRear].arg = arg;
    /* 向后移动 */
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    /* 任务数加1 */
    pool->queueSize++;
    /* 解锁 */
    pthread_mutex_unlock(&(pool->mutexpool));
    /* 发信号 */
    pthread_cond_signal(&(pool->notEmpty));


}


/* 线程池的销毁 */
int threadPoolDestory(threadpool_t *pool)
{

}