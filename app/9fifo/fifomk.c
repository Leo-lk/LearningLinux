#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    mode_t mode = 0666;
    if(argc < 2){
        printf("argc num error\n");
        exit(EXIT_FAILURE);
    }

    /* 创建一个fifo */
    if(mkfifo(argv[1], mode) < 0){
        printf("mkfifo error\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);

}
