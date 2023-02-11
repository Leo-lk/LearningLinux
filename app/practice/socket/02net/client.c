#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
/* 引用sockaddr_in结构体 */
#include <netinet/in.h>
/* 引用inet_addr函数 */
#include <arpa/inet.h>

#define SOCKET_PATH         "127.0.0.1"
#define SOCKET_PORT         9734

int main(int argc, char **argv)
{
    socklen_t server_len;
    struct sockaddr_in service_addr;
	char ch = 'A';

    /* 新建一个网络 socket */
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_id == -1){
        perror("socket create");
        exit(EXIT_FAILURE);
    }
    /* 填入服务器的地址等信息 */
    service_addr.sin_family = AF_INET;
    service_addr.sin_addr.s_addr = inet_addr(SOCKET_PATH);  /* 转换地址 */
    service_addr.sin_port = SOCKET_PORT;                    /* 设置端口 */
    server_len = sizeof(service_addr);

    /* 发出连接请求 */
    if(connect(socket_id, (__CONST_SOCKADDR_ARG)&service_addr, server_len) == -1){
        perror("socket connect");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "connect to a service\n");

    write(socket_id, &ch, 1);
    read(socket_id, &ch, 1);
    fprintf(stderr, "read:%c\n", ch);

    /* 关闭连接 */
    close(socket_id);
}


