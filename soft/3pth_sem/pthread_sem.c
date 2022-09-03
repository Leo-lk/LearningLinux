#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_LENGTH  1024
pthread_t pth;
sem_t sem;
char string[MAX_LENGTH];

/* 计算string字符串长度 */
void * func(void *arg)
{
    sem_wait(&sem);
    printf("in function\n");
    /* 查询输入是否有end */
    while(strncmp("end", string, 3) != 0){
        printf("string len = %ld \n", (strlen(string)-1));
        sem_wait(&sem);
    }
    /* 退出线程 */
    pthread_exit("thread return");
}

int main(int argc, char **argv)
{
    void * s2;

    if(sem_init(&sem, 0, 0) != 0){
        printf("create semaphore failed\n");
		exit(EXIT_FAILURE);
    }
    /* 创建线程 */
    if (pthread_create(&pth, NULL, func, (void *)string)) {
		printf("pthread_create() failed\n");
		exit(EXIT_FAILURE);
	}
    printf("input string, return length, input end to finish \n");
    /* 查询输入是否有end */
    while(strncmp("end", string, 3) != 0){
        fgets(string, MAX_LENGTH, stdin);
        sem_post(&sem);
    }
    /* 等待子线程pth结束，取返回值 */
    pthread_join(pth, NULL);
    sem_destroy(&sem);
    /* 退出 */
    exit(EXIT_SUCCESS);
}

