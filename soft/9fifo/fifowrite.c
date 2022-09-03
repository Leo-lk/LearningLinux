#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int buffer = 0;

int main(int argc, char **argv)
{
    mode_t mode;
    int fd;
    pid_t pid = getpid();

    if(argc < 2){
        printf("argc num error\n");
        exit(EXIT_FAILURE);
    }

    /* 打开fifo */
    fd = open(argv[1], O_WRONLY);
    if(fd < 0){
        perror("open error");
        exit(EXIT_FAILURE);
    }
    /* 写数据并显示 */
    while(1) {
        buffer++;
        sleep(1);
        if(write(fd, &buffer, 1) < 0){
            perror("write");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    /* 关闭 */
    close(fd);
    exit(EXIT_SUCCESS);
}
