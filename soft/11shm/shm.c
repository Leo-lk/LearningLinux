#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


/* 错误上报 */
void error_op(char const * string)
{
    perror(string);
    exit(EXIT_FAILURE);
}


/*  */
int main(int argc, char **argv)
{
    
    exit(EXIT_SUCCESS);
}
