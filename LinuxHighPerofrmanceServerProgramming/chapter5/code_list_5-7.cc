#include "net.h"

int main(int argc, char** argv)
{
    if (argc < 2){
        fprintf(stdout,"%s usgae : ip_address port\n", basename(argv[0]));
        return 1;
    }

    const char* ip = argv[1];
    short       port=atoi(argv[2]);

    struct sockaddr_in server_address;
    bzero(&server_address,sizeof server_address);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_address.sin_addr);
    server_address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0); 
    assert( sock >= 0 );

    int ret = bind(sock,(struct sockaddr*) &server_address,sizeof server_address);
    assert (ret != -1);

    ret = listen(sock, 5);
    assert( ret!= -1 );

    struct sockaddr_in client;
    socklen_t client_addrlength;
    int client_fd = accept(sock, (struct sockaddr*)&client, &client_addrlength);

    if (client_fd <  0){
        fprintf(stdout,"errno : %d", errno);
    }else{
        char buf[1024] = {'\0'};
        int buf_len = sizeof buf - 1;
        memset(buf, '\0', buf_len+1);
        ret = recv(client_fd, buf, buf_len, 0);
        fprintf(stdout,"got %d bytes, %s\n",ret, buf);

        memset(buf, '\0', buf_len+1);
        ret = recv(client_fd, buf, buf_len, MSG_OOB);
        fprintf(stdout,"got %d bytes, %s\n",ret, buf);

        memset(buf, '\0', buf_len+1);
        ret = recv(client_fd, buf, buf_len, 0);
        fprintf(stdout,"got %d bytes, %s\n",ret, buf);

    }


    return 0;
}

