/*************************************************************************
*File Name: code_list_5-11.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月23日 星期四 16时10分39秒
 ************************************************************************/
#include <stdio.h>
#include "net.h"

int main(int argc, char** argv)
{
    if (argc < 3) {
        fprintf(stdout,"%s ip port recv_buf_size\n", basename(argv[0] ));
        return 1;
    }
    
    const char* ip = argv[1];
    const short port = atoi( argv[2] );

    sockaddr_in listen_addr;
    bzero(&listen_addr, sizeof listen_addr);
    listen_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &listen_addr.sin_addr);
    listen_addr.sin_port = htons( port );

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert ( sock >= 0 );

    int ret = bind(sock, (struct sockaddr*)&listen_addr, sizeof listen_addr);
    assert ( ret != -1 );

    ret = listen( sock, 5 );
    assert( ret != -1 );


    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof client_addr;

    int client_sock = accept(sock,(struct sockaddr*)&client_addr,  &client_addr_len);

    if (client_sock < 0){
        fprintf(stdout,"accept failed, errno:%d\n", errno);
        return 1;
    }else{

        int recv_buf_len = 0;
        socklen_t  recv_buf_len_len = sizeof recv_buf_len;
        getsockopt( client_sock, SOL_SOCKET, SO_RCVBUF, &recv_buf_len, &recv_buf_len_len);
        fprintf(stdout,"the recv buffer len before setting is %d\n", recv_buf_len);

        recv_buf_len = atoi(argv[3]);
        setsockopt(client_sock, SOL_SOCKET, SO_RCVBUF, &recv_buf_len, recv_buf_len_len);
        getsockopt( client_sock, SOL_SOCKET, SO_RCVBUF, &recv_buf_len, &recv_buf_len_len);
        fprintf(stdout,"the recv buffer len after setting is %d\n", recv_buf_len);

        char buf[MAX_BUFF_SIZE];
        memset(buf, '\0', MAX_BUFF_SIZE);
        int recv_len = 0;
        int total_len = 0;
        while( (recv_len = recv(client_sock, buf, sizeof buf, 0)) > 0){
            total_len += recv_len;
            fprintf(stdout,"recv data len  is  %d(%s)\n",  total_len,buf); 
            memset(buf, '\0', MAX_BUFF_SIZE);
        }
    }

    close(sock);
    return 0;
}

