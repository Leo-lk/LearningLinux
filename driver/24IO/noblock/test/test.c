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

#define FILE_NAME           "/dev/localdevice0"

int fd, len;
/* 定义要关注的描述符集合 */
fd_set readfd;
char buf[64];
struct timeval timeout;

int main(int argc, char *argv[])
{
    int ret;
    
    /* 声明使用非阻塞的方式访问 */
    fd = open(FILE_NAME, O_RDWR | O_NONBLOCK);
    if (fd<0) {
        perror("open fail \n");
        return fd;
    }
    /* 循环读取 */
    while(1) {
        /* 将描述符集所有位清零 */
        FD_ZERO(&readfd);
        /* 将文件加入读监视描述符集 */
        FD_SET(fd, &readfd);
        /* 在循环中需要重新设置，不然select返回后会清楚timeout */
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        /* 如果timeout为NULL，select会在此等待 */
        ret = select(fd+1, &readfd, NULL, NULL, &timeout);
        if (ret < 0) {
            perror("select");
            return -1;
        }else if (ret){
            /* fd是否在可读描述符集中 */
            if(FD_ISSET(fd, &readfd))
            {
                //can read
                read(fd, buf, 1);
                printf("can read, %d\n", buf[0]);
            }
        }else {
            printf("timeout\n");
        }
        
        sleep(1);
    }
}