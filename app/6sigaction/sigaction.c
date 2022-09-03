#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

struct sigaction act;

void sig_recv(int argc)
{
    printf("recv sig : %d\n", argc);
}

int main(int argc, char **argv)
{
    pid_t pid;
    act.sa_handler = sig_recv;
    /* 一次生效后重置默认为处理函数 */
    act.sa_flags = SA_RESETHAND;
    sigemptyset(&act.sa_mask);
    /* 改变信号处理(ctrl + c 发送SIGINT) */
    sigaction(SIGINT, &act, 0);

    while (1)
    {
        /* 等待键盘输入ctrl + c */
        printf("wait\n");
        sleep(1);
    }
}

