#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/* 使用无名管道模仿cat命令 */
int main(int argc, char **argv)
{
    /* 定义管道文件描述符 */
    int pipefd[2];
    int fd;
    pid_t pid;
    char buf1, buf2;
    /* 输入检测 */
    if(argc != 2){
        fprintf(stderr, "usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* 初始化pipe */
    if (pipe(pipefd) == -1)
    {
        printf("pipe failed\n");
        exit(EXIT_FAILURE);
    }
    if((pid = fork()) == -1){
        printf("fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        /* 子进程关闭 写管道 */
        close(pipefd[1]);
        /* 从管道中读取数据，显示到屏幕上 */
        while (read(pipefd[0], &buf1, 1) > 0)
            write(STDOUT_FILENO, &buf1, 1);
        write(STDOUT_FILENO, "\n", 1);
        /* 关闭读管道 */
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
    else{
        /* 子进程关闭 读管道 */
        close(pipefd[0]);
        /* 打开指定的文件 */
        if((fd = open(argv[1], O_RDONLY)) < 0){
            printf("open %s failed\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        /* 从指定文件中读数据，写入管道中 */
        while(read(fd, &buf2, 1) > 0)
            write(pipefd[1], &buf2, 1);
        close(fd);
        close(pipefd[1]);
        /* 等待子进程结束并销毁 */
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
