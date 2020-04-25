/*************************************************************************
*File Name: code_list_5-12.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月23日 星期四 18时40分03秒
 ************************************************************************/
#include "net.h"

int main(int argc, char** argv)
{
    assert( argc == 2 );
    char *host = argv[1];

    struct hostent* hostinfo = gethostbyname(host);
    assert (hostinfo);

    struct servent* servinfo = getservbyname("daytime","tcp");
    assert(servinfo);
    fprintf(stdout,"daytime port is %d\n", ntohs(servinfo->s_port));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = servinfo->s_port;

    address.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;

    int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
    int ret = connect(sockfd, (struct sockaddr*)&address, sizeof address);
    if (ret < 0){
        fprintf(stdout," ret:%d, errno :%d \n",ret, errno);
    }
    else{
        char buf[MAX_BUFF_SIZE];
        memset(buf, 0x00, sizeof buf);
        int recv_len =recv(sockfd, buf, sizeof buf, 0);
        if (recv_len > 0){
            fprintf(stdout,"%s\n", buf);
        }
    }
    close(sockfd);

    return 0;
}

