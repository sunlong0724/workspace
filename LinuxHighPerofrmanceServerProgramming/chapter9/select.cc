/*************************************************************************
*File Name: select.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月24日 星期五 16时17分18秒
 ************************************************************************/
#include "../net.h"

#include <map>
#include <unordered_set>

int main(int argc, char** argv)
{
    if (argc < 3){
        fprintf(stdout,"%s ip port\n", basename( argv[0] ));
        return 1;
    }

    const char* ip = argv[1];
    const short port = atoi( argv[2] );
    struct sockaddr_in listen_address;
    bzero(&listen_address, sizeof listen_address);
    listen_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &listen_address.sin_addr);
    listen_address.sin_port = htons( port );

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert (sock >= 0);

    int ret = bind( sock, (struct sockaddr*)&listen_address, sizeof listen_address );
    assert (ret != -1);

    ret = listen(sock, 5);

    fd_set rdset;
    struct timeval tv = {1, 0};

    std::map<int, struct sockaddr_in> clients_r;
    std::unordered_set<int>  closed_fd;
    clients_r.insert({sock, {}});


    while(true){
        FD_ZERO(&rdset);

        for (auto& kv : clients_r){
            FD_SET(kv.first, &rdset);
        }

        int max_fd = clients_r.rbegin()->first;

        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(max_fd+1, &rdset, nullptr, nullptr, &tv);
        if (ret < 0){
            if (ret == -1){

            }
            fprintf(stdout,"errno %d\n", errno);

        }else if (ret == 0){
            fprintf(stdout,"timeout 1 s\n" );
        }else{
            for (auto& kv : clients_r){
                int client_fd = kv.first;
                if (FD_ISSET(client_fd, &rdset)){
                    if (sock == client_fd){
                        struct sockaddr_in client_addr;
                        socklen_t client_addr_len = sizeof client_addr;

                        int client_fd = accept(sock, (struct sockaddr*)&client_addr, &client_addr_len);
                        assert (client_fd >= 0);
                        int f_getfd = fcntl(client_fd, F_GETFD);
                        ret = fcntl(client_fd, F_SETFD, (f_getfd | O_NONBLOCK));
                        assert (ret == 0);
                        clients_r.insert({client_fd, client_addr});

                    }else{
                        char buf[ MAX_BUFF_SIZE ];
                        memset(buf, 0x00, sizeof buf);
                        int ret = 0;
                        while( (ret = recv(client_fd, buf, (sizeof buf )-1, 0)) > 0){
                            fprintf(stdout,"%s\n", buf);
                            memset(buf, 0x00, sizeof buf);
                        }
                        if (ret == 0){
                            closed_fd.insert(client_fd);
                            char ip[20] = {'\0'};
                            socklen_t  cliet_addr_len = 20;
                            inet_ntop(AF_INET, &clients_r[client_fd].sin_addr, ip, 20);
                            fprintf(stdout,"client fd %s closed!\n", ip);
                        }
                        fprintf(stdout,"recv done %d\n", ret);
                    }
                }
            }
        }
        for(auto& k : closed_fd) clients_r.erase(k);
        closed_fd.clear();
    }

    for (auto& kv : clients_r){
        close(kv.first);
    }
    
    return 0;
}

