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
    sigset_t set;
    sigset_t oset;
    sigaddset(&set, SIGINT);
    /* 把set中的信号加入到信号屏蔽字中 */
    if(sigprocmask(SIG_BLOCK, &set, &oset) != 0){
        printf("sigprocmask failed\n");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, sig_recv);
    while (1)
    {
        /* 输入ctrl + \ 发送SIGQUIT信号退出 */
        printf("wait input\n");
        sleep(1);
    }
    return 0;
}

