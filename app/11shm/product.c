#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "share.h"
/* 创建一个共享内存段，并将其中的内容显示出来。*/

int main(int argc, char **argv)
{
    int running = 1;
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    char buffer[BUFSIZ];
    int shmid;
    /* 创建共享内存 */
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st),
    0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    /* 获取共享内存的的地址 */
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Memory attached at %X\n", (int)shared_memory);
    shared_stuff = (struct shared_use_st *)shared_memory;

    while(running)
    {
        while(shared_stuff->written_by_you == 1)
        {
            sleep(1);
            printf("waiting for client...\n");
        }
        printf("Enter some text: ");
        /* 获取键盘输入 */
        fgets(buffer, BUFSIZ, stdin);
        strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
        shared_stuff->written_by_you = 1;
        if (strncmp(buffer, "end", 3) == 0)
        {
            running = 0;
        }
    }
}