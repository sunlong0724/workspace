#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>


#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

extern int addfd( int epollfd, int fd, bool one_shot );
extern int removefd( int epollfd, int fd );


void addsig( int sig, void( handler )(int), bool restart = true ){
    struct sigaction sa;
    memset( &sa, '\0', sizeof sa );
    sa.sa_handler = handler;
    if (restart) sa.sa_flags |= SA_RESTART;
    sigfillset(& sa.sa_mask);
    assert ( sigaction( sig, &sa, nullptr ) != -1 );
}

void show_error( int connfd, const char* info ){
    printf("%s", info);
    send( connfd, info, strlen( info ), 0 );
    close( connfd );
}



int main(int argc, char** argv)
{
    if (argc <=2){
        printf("uage: %s ip_address port_number\n", basename( argv[0] ));
        return 1;
    }

    const char *ip = argv[1];
    const short port = atoi( argv[2] );

    /*忽略SIGPIPE信号*/
    addsig( SIGPIPE, SIG_IGN );

    /*创建线程池*/
    threadpool< http_conn > *pool = nullptr;
    try {
        pool = new threadpool< http_conn >;
    }catch( ... ){
        return 1;
    }

    /*预先为每个可能的客户连接分配一个http_conn对象*/
    http_conn* users = new http_conn [ MAX_FD ];
    assert ( users );
    int user_count = 0;

    int listenfd = socket( AF_INET, SOCK_STREAM, 0 );
    assert ( listenfd >= 0 );
    struct linger tmp = { 1, 0 };
    setsockopt( listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof ( tmp ) );

    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof address );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    ret = bind( listenfd, (struct sockaddr*)&address, sizeof address );
    assert (ret >= 0);

    ret = listen(listenfd, 5);
    assert (ret >= 0);

    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create(23);
    assert ( epollfd != -1 );
    addfd( epollfd, listenfd, false );
    http_conn::m_epollfd = epollfd;

    while( 1 ){
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        if (number < 0 && errno != EINTR){
            printf( "epoll failure\n" );
            break;
        }

        for (int i = 0; i < number; ++i){
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd){
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof client_address;
                int connfd = accept( listenfd, (struct sockaddr*)&client_address, &client_addrlength);
                
                if (connfd < 0){
                    printf(" errno is:%d\n", errno);
                    continue;
                }
                if (http_conn::m_user_count >= MAX_FD){
                    show_error( connfd, "Internal server busy" );
                    continue;
                }
                /*初始化客户连接*/
                users[connfd].init( connfd, client_address );
            }
            else if ( events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR) ){
                /*如果有异常，直接关闭客户连接*/
                users[ sockfd ].close_conn();
            }
            else if ( events[i].events & EPOLLIN ){
                /*根据读的结果， 决定是否将任务添加到线程池，还是关闭连接*/
                if (users[ sockfd ].read()){
                    pool->append( users + sockfd );
                }else {
                    users[ sockfd ].close_conn();
                }
            }else if (events[i].events & EPOLLOUT){
                /*根据写的结果，决定是否关闭连接*/
                if (!users[ sockfd ].write()){
                    users[ sockfd ].close_conn();
                }
            }else {

            }
        }
    }

    close( epollfd );
    close( listenfd );
    delete []  users;
    delete pool;

    printf("main done!\n");
    return 0;
}


























































































































































































































































































































































