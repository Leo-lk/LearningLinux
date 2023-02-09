#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "msgprac.h"


typedef struct {
    long cmd_type;
    char data[FRAME_LEN];
} some_data;

int main(int argc, char **argv)
{
    int ret = 0;
    char tempbuf[FRAME_LEN] = {0};
    int running = 1;

    int msgid = msgget((key_t)KEY, 0666 | IPC_CREAT);
    if (msgid < 0)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    some_data data;

    while(running)
    {
        ret = msgrcv(msgid, &data, FRAME_LEN, 0, 0);
        if (ret < 0)
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "recv data:\n");
        fprintf(stderr, "%s\n", data.data);
        if (strncmp(data.data, "end", 3) == 0)
        {
            running = 0;
        }
    }

    ret = msgctl(msgid, IPC_RMID, 0);
    if (ret < 0)
    {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "delete msg\n");
}

