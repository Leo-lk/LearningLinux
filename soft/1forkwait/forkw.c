#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
    pid_t child;
    int status;
    if((child = fork()) == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(child == 0){
        puts("in child");
        char * argv[] = {"ls -l", NULL};
        if(execve("/bin/ls", argv, NULL) == -1){
            perror("execve");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else{
        waitpid(child,&status, 0);
        puts("in parent");
        printf("pid = %d\n",getpid());
        printf("ppid = %d\n",getppid());
        exit(EXIT_SUCCESS);
    }
}

