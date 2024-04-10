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
	/* 将本进程的pid告诉内核 */
	fcntl(fd, F_SETOWN, getpid());
	/* 获取当前的进程状态 */
	flag = fcntl(fd, F_GETFL);
	/* 开启异步通知：触发驱动中file_operation的*_fasync函数，初始化异步通知结构体 */
	fcntl(fd, F_SETFL, flag|FASYNC);
	/* 注册信号处理函数 */
	signal(SIGIO, read_func);
	/* 等待信号触发 */
	while(1);

	close(fd);
    return 0;
}
