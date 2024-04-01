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
	int fd;
	fd = open("/dev/hellodevice",O_RDWR);
	if(fd<0)
	{
		perror("open fail \n");
		return fd;
	}
	printf("open ok \n ");
	
	sleep(50);

	close(fd);
    return 0;
}
