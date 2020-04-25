/*************************************************************************
*File Name: code_list_6-3_splice.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月24日 星期五 13时52分14秒
 ************************************************************************/
#include "../net.h"

int main(int argc, char** argv)
{
    if (argc <= 2) {
        fprintf(stdout,"%s ip port\n", basename (argv[ 0 ]));
        return 1;
    }
    const char* ip = argv[ 1 ];
    const short port=atoi( argv[2] );
    
    struct sockaddr_in address;
    bzero(&address, sizeof address);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert (sock >= 0);

    int ret = bind( sock, (struct sockaddr*)&address, sizeof address );
    assert (ret != -1);

    ret = listen(sock, 5);
    assert (ret != -1);

    struct sockaddr_in client;
    socklen_t client_addr_len = sizeof client;
    bzero(&client, client_addr_len);

    int client_fd = accept(sock, (struct sockaddr*)&client, &client_addr_len);
    if (client_fd < 0){
        fprintf(stdout,"errno %d\n", errno);
    }else{
        int r_w_fds[2];
        ret = pipe(r_w_fds);
        assert (ret != -1);


        int r_w_fds_std[2];
        ret = pipe(r_w_fds_std);

        ret = splice(client_fd, nullptr, r_w_fds[1], nullptr, 32768, SPLICE_F_MOVE|SPLICE_F_MORE);

        ret = tee(r_w_fds[0], r_w_fds_std[1], 32768,SPLICE_F_NONBLOCK);
        assert(ret != -1);

        ret = splice(r_w_fds[0], nullptr,client_fd, nullptr, 32768, SPLICE_F_MOVE|SPLICE_F_MORE);
        assert(ret != -1);
        ret = splice(r_w_fds_std[0], nullptr, STDOUT_FILENO, nullptr, 32768, SPLICE_F_MOVE| SPLICE_F_MORE);
        assert (ret != -1);

        close(client_fd);

        close(r_w_fds[0]);
        close(r_w_fds[1]);
        close(r_w_fds_std[0]);
        close(r_w_fds_std[1]);

    }

    close(sock);




    return 0;
}

