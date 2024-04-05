#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <sys/ioctl.h>
#include "../beep.h"


int main(int argc, char *argv[])
{
	int fd, ret;
	char *filename;
	unsigned int cmd;
	unsigned int arg;
	unsigned char str[100];

	if (argc != 2) {
		printf("Error Usage!\r\n");
		return -1;
	}

	filename = argv[1];

	fd = open(filename, O_RDWR);
	if (fd < 0) {
		printf("Can't open file %s\r\n", filename);
		return -1;
	}

	while (1) {
		printf("Input CMD:");
		ret = scanf("%d", &cmd);
		if (ret != 1) {				/* 参数输入错误 */
			fgets(str, sizeof(str), stdin);				/* 防止卡死 */
		}
		if(4 == cmd)				/* 退出APP	 */
			goto out;
		if(cmd == 1)				/* 关闭LED灯 */
			cmd = DEV_CLOSE_TIMER;
		else if(cmd == 2)			/* 打开LED灯 */
			cmd = DEV_OPEN_TIMER;
		else if(cmd == 3) {
			cmd = DEV_MODIFY_TIMER;	/* 设置周期值 */
			printf("Input Timer Period:");
			ret = scanf("%d", &arg);
			if (ret != 1) {			/* 参数输入错误 */
				fgets(str, sizeof(str), stdin);			/* 防止卡死 */
			}
		}
		ioctl(fd, cmd, arg);		/* 控制定时器的打开和关闭 */	
	}

out:
	close(fd);
}