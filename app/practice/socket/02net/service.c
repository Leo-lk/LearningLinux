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
	socklen_t server_len, client_len;
    struct sockaddr_in service_addr;
    struct sockaddr_in client_addr;
    int client_sockfd;
	char ch;

    /* 新建一个网络socket */
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_id == -1){
        perror("socket create");
        exit(EXIT_FAILURE);
    }
    /* 设置套接字域domain */
    service_addr.sin_family = AF_INET;
    /* 设置套接字地址名 */
    service_addr.sin_addr.s_addr = inet_addr(SOCKET_PATH);  /* 转换地址 */
    service_addr.sin_port = SOCKET_PORT;                    /* 设置端口 */
    server_len = sizeof(service_addr);
    /* 绑定socket */
    if(bind(socket_id, (__CONST_SOCKADDR_ARG)&service_addr, server_len) == -1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    /* 监听请求，并将请求加入队列，队列长度为5 */
    if(listen(socket_id, 5) == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        client_len = sizeof(client_addr);
        /* 接受请求，client_len输入client_addr数据结构长度，输出请求的client_addr长度 */
        client_sockfd = accept(socket_id, (__SOCKADDR_ARG)&client_addr, &client_len);
        if (client_sockfd == -1){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "accept a request\n");

        read(client_sockfd, &ch, 1);
        fprintf(stderr, "read:%c\n", ch);
        ch++;
        write(client_sockfd, &ch, 1);

        /* 关闭连接 */
        close(client_sockfd);
    }
}


