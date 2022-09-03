#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

pthread_t pth;

void * func(void *arg)
{
    printf("in function\n");
    printf("wait 3s \n");
    /* 查询输入是否有end */
    sleep(3);
    printf("finish func thread\n");
    /* 退出线程 */
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    /* 创建线程属性 */
    pthread_attr_t attr;
    /* 初始化线程属性 */
    if(pthread_attr_init(&attr) != 0){
        printf("pthread_attr_init failed\n");
		exit(EXIT_FAILURE);
    }
    /* 指定线程属性是可分离 */
    if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0){
        printf("pthread_attr_setdetachstate failed\n");
		exit(EXIT_FAILURE);
    }
    /* 创建线程 */
    if (pthread_create(&pth, &attr, func, NULL)) {
		printf("pthread_create() failed\n");
		exit(EXIT_FAILURE);
	}
    pthread_attr_destroy(&attr);
    printf("wait 5s \n");
    sleep(5);
    printf("finish main thread\n");
    /* 退出 */
    exit(EXIT_SUCCESS);
}

