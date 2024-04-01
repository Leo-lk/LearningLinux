#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "../beep.h"


int main(int argc, char **argv)
{
	int fd, num, ret;
	fd = open("/dev/hellodevice",O_RDWR);
	if(fd<0)
	{
		perror("open fail \n");
		return fd;
	}
	printf("open ok \n ");

	ioctl(fd,DEV_FIFO_CLEAN);
	
	ret = ioctl(fd,DEV_FIFO_GETVALUE,&num);
	if(ret < 0)
	{
		perror("ioctl");
		return ret;
	}
	printf("num = %d \n",num);

	num = 77;
	ioctl(fd,DEV_FIFO_SETVALUE,&num);

	close(fd);
    return 0;
}
