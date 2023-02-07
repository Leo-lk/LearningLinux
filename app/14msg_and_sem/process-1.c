#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FRAME_LEN   256

typedef struct {
    long cmd_type;
    char data[FRAME_LEN];
} some_data;

int main(int argc, char **argv)
{
    int msgid = msgget(KEY, IPC_CREAT);

    some_data data;

    data.cmd_type = 0;

    while()
    {
        fgets()
    }


    msgsnd(msgid, &data, FRAME_LEN, IPC_NOWAIT);

}

