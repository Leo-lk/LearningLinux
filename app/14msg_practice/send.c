#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "msgprac.h"


typedef struct {
    long int cmd_type;
    char data[FRAME_LEN];
} some_data;

int main(int argc, char **argv)
{
    int ret = 0;
    char tempbuf[FRAME_LEN] = {0};
    int running = 1;
    some_data data;

    int msgid = msgget((key_t)KEY, 0666 | IPC_CREAT);
    if(msgid < 0)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    data.cmd_type = 1;

    while(running)
    {
        fprintf(stderr, "input data\n");
        if(fgets(tempbuf, FRAME_LEN, stdin) < 0)
        {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
        
        strcpy(data.data, tempbuf);

        ret = msgsnd(msgid, (void *)&data, FRAME_LEN, 0);
        if (ret == -1)
        {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
        if (strncmp(tempbuf, "end", 3) == 0)
        {
            running = 0;
        }
    }
}

