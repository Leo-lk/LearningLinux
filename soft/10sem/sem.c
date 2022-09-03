#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

union semun
{
	int val;
};
/* 删除信号量 */
int sem_rm(int semid)
{
    /* sem_num:信号量编号，只有一个则取值为0 */
    return semctl(semid, 0, IPC_RMID);
}
/* 设置信号量初始值 */
int sem_set(int semid, int value)
{
    union semun semval;
    semval.val = value;
    return semctl(semid, 0, SETVAL, semval);
}
/*  P:  op=-1
    V:  op=1    */ 
int sem_p(int semid, short op)
{
    struct sembuf  sem_op;
    /* 不使用一组信号量 */
    sem_op.sem_num = 0;
    /* 系统自动释放信号量 */
    sem_op.sem_flg = SEM_UNDO;
    sem_op.sem_op = op;
    return semop(semid, &sem_op, 1);
}
/* 错误上报 */
void error_op(char const * string)
{
    perror(string);
    exit(EXIT_FAILURE);
}


/* 进出临界区打印X或O */
int main(int argc, char **argv)
{
    key_t key = 1234;   //不同的进程可以通过一个key访问一个信号量
    char och = 'O';
    int semid;

    /* 获取/创建信号量,需要一个信号量 */
    if((semid = semget(key, 1, 0666|IPC_CREAT)) < 0){
        error_op("semget");
    }
    /* 如果有参数则设置初始信号量，修改打印值 */
    if(argc > 1){
        och = 'X';
        if(sem_set(semid, 1) < 0){
            error_op("semctl");
        }
    }
    /* 循环进入临界区10次 */
    for(int i = 0; i < 10; i++)
    {
        /* p */
        if(sem_p(semid, -1) < 0){
            error_op("semop");
        }
        printf("%c", och);
        fflush(stdout); //立即打印 
        sleep(2);
        /* v */
        printf("%c", och);
        fflush(stdout);
        if(sem_p(semid, 1) < 0){
            error_op("semop");
        }
        sleep(1);
    }
    /* 初始进程睡眠10s，删除信号量 */
    if(argc > 1){
        sleep(10);
        if(sem_rm(semid) < 0){
            error_op("semctl");
        }
    }
    exit(EXIT_SUCCESS);
}
