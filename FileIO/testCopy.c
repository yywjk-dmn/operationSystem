#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int main()
{
    /* 打开要复制的文件 */
    int fd1 = open("./testOpen.c", O_RDWR);
    if (fd1 == -1)
    {
        perror("open error");
    }
    int fd2 = open("./copyTestOpen.txt", O_RDWR | O_CREAT);
    if (fd2 == -1)
    {
        perror("open error");
    }


    /* 读文件,缓冲区 */
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer) - 1);
    /* 文件打开 那文件指针默认在文件开头 */
    int readLen = 0;
    while (1)
    {
        read(fd1, buffer, sizeof(buffer) - 1);
        write(fd2, buffer, readLen);

        if (readLen < BUFFER_SIZE || readLen == 0)
        {
            break;
        }
    }

    return 0;
}