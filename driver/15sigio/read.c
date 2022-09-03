#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>


int fd, len;
char buf[64] = "123456879";

void read_func(int signo)
{
	printf("before read \n ");
	len = read(fd, buf, 64);
	printf("read ok : %s   len = %d\n", buf, len);
}

int main(int argc, char **argv)
{
	int flag;
	fd = open("/dev/hellodevice",O_RDWR);
	if(fd<0)
	{
		perror("open fail \n");
		return fd;
	}
	
	fcntl(fd, F_SETOWN, getpid());
	flag = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flag|FASYNC);
	signal(SIGIO, read_func);
	while(1);

	close(fd);
    return 0;
}
