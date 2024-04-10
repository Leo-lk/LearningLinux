#include <stdio.h>
#include "unistd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <sys/ioctl.h>
#include <unistd.h> 
#include <sys/time.h> 
#include <poll.h>

#define FILE_NAME           "/dev/localdevice0"

int fd;
char buf[64];

int main(int argc, char *argv[])
{
    int ret;
    struct pollfd fds;
    
    /* 声明使用非阻塞的方式访问 */
    fd = open(FILE_NAME, O_RDWR | O_NONBLOCK);
    if (fd<0) {
        perror("open fail \n");
        return fd;
    }
    fds.fd = fd;
    fds.events = POLLIN;
    /* 循环读取 */
    while(1) {
        /* 监控一个事件，超时时间500ms */
        ret = poll(&fds, 1, 500);
        if (ret < 0) {
            perror("select");
            return -1;
        }else if (ret){
            //can read
            read(fd, buf, 1);
            printf("can read, %d\n", buf[0]);
        }else {
            printf("timeout\n");
        }
        
        printf("new\n");
        sleep(1);
    }
}