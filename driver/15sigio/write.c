#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "beep.h"


int main(int argc, char **argv)
{
	int fd, len;
	char buf[64] = "kekekeke";
	fd = open("/dev/hellodevice",O_RDWR);
	if(fd<0)
	{
		perror("open fail \n");
		return fd;
	}
	printf("before write \n ");
	len = write(fd, buf, strlen(buf));
	printf("write ok : len = %d\n", len);
	
	close(fd);
    return 0;
}
