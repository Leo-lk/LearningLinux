#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void sig_recv(int argc)
{
    printf("recv sig : %d\n", argc);
}

int main(int argc, char **argv)
{
    pid_t pid;
    
    /* 在子进程中发信号 */
    if ((pid = fork()) == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        puts("in child");
        /* 等待信号重定向 */
        sleep(5);
        /* 向父进程发信号 */
        kill(getppid(), SIGALRM);
        exit(EXIT_SUCCESS);
    }
    /* 改变信号处理 */
    signal(SIGALRM, sig_recv);
    /* 等待子进程完成 */
    pause();
    printf("finish main \n");
    /* 退出 */
    exit(EXIT_SUCCESS);
}

