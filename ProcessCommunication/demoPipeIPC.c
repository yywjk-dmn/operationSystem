#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>

#define PIPE_SIZE 2

int main()
{
    int pipefd[PIPE_SIZE];
    memset(pipefd, 0, sizeof(int) * PIPE_SIZE);

    /* 创建管道 */
    pipe(pipefd);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        _exit(-1);
    }
    if (pid == 0)
    {
        /* 关闭读端 */
        close(pipefd[0]);
        /* 子进程 */
        int num = 200;
        write(pipefd[1], (void *)&num, sizeof(int));

        /* 子进程自己写 自己读 就会出现僵尸进程 所以要关闭子进程的读端 */
        // int childNum = 0;
        // read(pipefd[0], (void *)&childNum, sizeof(int));
        // printf("parent process: childNum :%d\n", childNum);

        /* 关闭写端，避免资源的浪费 */
        close(pipefd[1]);

    }
    else if (pid > 0)
    {
        sleep(1);
        /* 关闭写端 */
        close(pipefd[1]);
        
        /* 父进程 */
        int readNum = 0;
        read(pipefd[0], (void *)&readNum, sizeof(int));

        printf("parent process: readNUm :%d\n", readNum);

        /* 关闭读端，避免资源浪费 */
        close(pipefd[0]);

    }



    return 0;
}