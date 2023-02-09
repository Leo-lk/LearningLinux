#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 512

typedef struct{
    long int my_msg_type;
    char some_text[MAX_TEXT];
}my_msg_st;

int main()
{
    int running = 1;
    my_msg_st some_data;
    int msgid;
    char buffer[BUFSIZ];
    /* 获取key指向的消息队列的标识符 */
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    } 
    while(running)
    {
        printf("Enter some text: ");
        if(fgets(buffer, BUFSIZ, stdin)<0)
        {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
        /* 表示命令号/消息类型为1 */
        some_data.my_msg_type = 1;
        strcpy(some_data.some_text, buffer);
        /* 发送一个数据块，该数据块以一个长整型变量开始(消息类型)，第三个参数为some_text大小 */
        if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1)
        {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
        if (strncmp(buffer, "end", 3) == 0)
            running = 0;
    }
    exit(EXIT_SUCCESS);
}