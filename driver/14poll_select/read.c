#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


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
	printf("before read \n ");
	len = read(fd, buf, 64);
	printf("read ok : %s   len = %d\n", buf, len);

	close(fd);
    return 0;
}
