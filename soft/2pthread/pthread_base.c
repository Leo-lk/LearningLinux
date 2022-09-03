#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_t pth;

char string[] = "hello";

void * func(void *arg)
{
    printf("in function\n");
    sleep(2);
    strcpy(arg, "bye");
    /* 退出线程 */
    pthread_exit("thread return");
}

int main(int argc, char **argv)
{
    void * s2;
    printf("start creat thread\n");
    /* 创建线程 */
    if (pthread_create(&pth, NULL, func, (void *)string)) {
		printf("pthread_create() failed\n");
        /* 退出 */
		exit(EXIT_FAILURE);
	}
    /* 等待子线程pth结束，取返回值 */
    pthread_join(pth, &s2);
    /* 子线程的返回值 */
    printf("in main th : %s\n", (char *)s2);
    /* 子线程修改的字符串 */
    printf("value : %s\n", string);
    /* 退出 */
    exit(EXIT_SUCCESS);
}

