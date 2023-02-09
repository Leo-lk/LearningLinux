#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct {
    long int my_msg_type;
    char some_text[BUFSIZ];
} my_msg_st;

int main()
{
    int running = 1;
    int msgid;
    my_msg_st some_data;
    /* 0表示取出消息队列中的第一个消息 */
    long int msg_to_receive = 0;
    /* 获取key指向的消息队列的标识符 */
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    while(running)
    { 
        /* 获取消息队列中的一个数据块 */
        if (msgrcv(msgid, (void *)&some_data, BUFSIZ, msg_to_receive, 0) == -1)
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        printf("You wrote: %s", some_data.some_text);
        if (strncmp(some_data.some_text, "end", 3) == 0)
            running = 0;
    }

    if (msgctl(msgid, IPC_RMID, 0) == -1)
    {
        perror("msgctl(IPC_RMID)");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}






