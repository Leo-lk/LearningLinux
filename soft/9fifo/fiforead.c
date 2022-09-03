#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int buffer;

int main(int argc, char **argv)
{
    int fd ;
    pid_t pid = getpid();

    if(argc < 2){
        printf("argc num error\n");
        exit(EXIT_FAILURE);
    }
    /* 打开fifo */
    fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("open error");
        exit(EXIT_FAILURE);
    }
    /* 读数据并显示 */
    while(read(fd, &buffer, 1) > 0) {
        printf("%d read: %d   \n", pid, buffer);
        sleep(1);
    }
    /* 关闭 */
    close(fd);
    exit(EXIT_SUCCESS);
}
