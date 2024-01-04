#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main()
{
    #if 1
    /* 使用lseek来计算文件大小 */
    int fd  = open("./testWrite.c", O_RDWR);
    if (fd == -1)
    {
        perror("open error");
    }

    off_t offset = lseek(fd, 0, SEEK_END);
    printf("offset:%ld\n", offset);

    #endif

    int fd = open("./hello.txt", O_RDWR);
    



    return 0;
}