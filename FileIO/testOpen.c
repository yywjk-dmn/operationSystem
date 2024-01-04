#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>



int main()
{
    
    /* 打开文件 */
    /* 有读写权限和如果没有这个文件就创建一个文件的权限 */
    int fd = open("./hello.txt", O_RDWR | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("open error");
    }


    return 0;
}