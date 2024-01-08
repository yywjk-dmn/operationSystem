#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>

int main()
{
    int num = 100;

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        _exit(-1);
    }

    if (pid == 0)
    {
        printf("child process : num = %d\n", num);

        num += 100;
        printf("child process : num = %d\n", num);

        /* 通信的一种方式 将num的值写到文件中 */
        int fd = open("./num.txt", O_RDWR | O_CREAT, 0644);
        if (fd == -1)
        {
            perror("error");
        }
        /* 写数据 */
        write(fd, (void *)&num, sizeof(num));
        /* 关闭文件 */
        close(fd);


    }
    else if (pid > 0)
    {
        printf("parent process : num = %d\n", num);
        sleep(1);
        printf("parent process : num = %d\n", num);

        int num1 = 0;
        int fd = open("./num.txt", O_RDWR);
        if (fd == -1)
        {
            printf("error");
        }
        /* 读数据 */
        read(fd, (void *)&num1, sizeof(num1));
        /* 关闭文件 */
        close(fd);
        printf("num1:%d\n", num1);


    }

    wait(NULL);

    printf("hello world");



    return 0;
}