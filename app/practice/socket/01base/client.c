#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
/* 引用sockaddr_un结构体 */
#include <sys/un.h>

#define SOCKET_PATH         "skt_file"

int main(int argc, char **argv)
{
    socklen_t server_len;
    struct sockaddr_un service_addr;
	char ch = 'A';

    /* 新建一个socket */
    int socket_id = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_id == -1){
        perror("socket create");
        exit(EXIT_FAILURE);
    }
    /* 填入服务器的地址等信息 */
    service_addr.sun_family = AF_UNIX;
    strcpy(service_addr.sun_path, SOCKET_PATH);
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


