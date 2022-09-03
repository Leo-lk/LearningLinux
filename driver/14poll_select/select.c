#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	int fd, len;
	fd_set readfd;
	fd_set writefd;
	char buf[64] = "kekekeke";
	fd = open("/dev/hellodevice",O_RDWR | O_NONBLOCK);
	if(fd<0)
	{
		perror("open fail \n");
		return fd;
	}
	while(1)
	{
		FD_ZERO(&readfd);
		FD_ZERO(&writefd);
		FD_SET(fd,&readfd);
		FD_SET(fd,&writefd);

		select(fd+1,&readfd,&writefd,NULL,NULL);
		if(FD_ISSET(fd,&readfd))
		{
			//can read
			printf("can read\n");
			
		}
		if(FD_ISSET(fd,&writefd))
		{
			//can write
			printf("can write\n");
		}
		sleep(1);
	}

	close(fd);
    return 0;
}
