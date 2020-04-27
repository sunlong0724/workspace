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

#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

class process{
public:
    process():m_pid(-1){}
public:
    pid_t m_pid;
    int m_pipefd[2];
};

template< typename T >
class processpool{
private:
    processpool(int listenfd, int process_number = 8);
public:

    static processpool< T >* create(int listenfd, int process_number = 8){
        if (!m_instance)
            m_instance = new processpool< T >(listenfd, process_number);
        return m_instance;
    }
    void run();
private:
    void setup_sig_pipe();
    void run_parent();
    void run_child();
private:
    /* 进程池允许的最大子进程数量 */
    static const int MAX_PROCESS_NUMBER = 16;
    /* 每个子进程最多能处理的客户数量 */
    static const int USER_PER_PROCESS = 65535;
    /* epoll最多能处理的事件数 */
    static const int MAX_EVENT_NUMBER = 10000;
    /* 进程池中的进程数 */
    int m_process_number;
    /* 子进程在池中的序号 */
    int m_idx;
    /* 每个进程都有一个epoll内核事件表,用epollfd标识 */
    int m_epollfd;
    /* 监听soket */
    int m_listenfd;
    /* 子进程通过m_stop来决定是否停止运行 */
    int m_stop;
    /* 保存所有子进程的描述信息 */
    process* m_sub_process;
    /* 进程池静态实例 */
    static processpool< T >* m_instance;
};

template< typename T >
processpool<T>* processpool<T>::m_instance = nullptr;

/* 用于处理信号的管道，以实现统一事件源，后面称之为信号管道 */
static int sig_pipefd[2];

static int setnonblocking(int fd){
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

static void addfd( int epollfd, int fd ){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl( epollfd, EPOLL_CTL_ADD,fd, &event );
    setnonblocking( fd );
}

/* 从epollfd 标识的epoll内核事件表中删除fd上的所有注册事件 */
static void removefd( int epollfd, int fd ){
    epoll_ctl( epollfd, EPOLL_CTL_DEL, fd, 0 );
    close(fd);
}

static void sig_handler( int sig ){
    int save_errno = errno;
    int msg = sig;
    send(sig_pipefd[1], (char*)&msg, 1, 0);
    errno = save_errno;
}

static void addsig( int sig, void( handler )(int), bool restart=true ){
    struct sigaction sa;
    memset(&sa, 0x00, sizeof sa );
    sa.sa_handler = handler;
    if (restart){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset( &sa.sa_mask );
    assert( sigaction(sig, &sa, nullptr) != -1 );
}

/*进程池构造函数。参数listenfd是监听socket，它必须在创建进程池之前被创建，否则子进程无法直接引用它。参数process_number
 指定进程池中子进程的数量 */
template <typename T>
processpool<T>::processpool( int listenfd, int process_number ):
    m_listenfd( listenfd ),m_process_number(process_number),m_idx(-1),m_stop(false){
        assert( (process_number > 0) && ( process_number <= MAX_PROCESS_NUMBER ) );

        m_sub_process= new process[ process_number ];
        assert (m_sub_process);

        /*创建process_number个子进程，并建立他们和父进程之间的管道*/
        for (int i = 0; i < process_number; ++i){
            int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd );
            assert (ret == 0);

            m_sub_process[i].m_pid = fork();
            assert ( m_sub_process[i].m_pid >= 0 );
            if (m_sub_process[i].m_pid > 0){//父进程
                close( m_sub_process[i].m_pipefd[1] );
                continue;
            }else{//子进程
                close( m_sub_process[i].m_pipefd[0] );
                m_idx = i;
                break;
            }
        }
    }

/*统一事件源*/

template<typename T>
void processpool<T>::setup_sig_pipe(){
    /*创建epoll事件监听表和信号集合*/
    m_epollfd = epoll_create(22);
    assert (epollfd != -1);

    int ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert (ret != -1);

    setnonblocking( sig_pipefd[1] );
    addfs( m_epollfd, sig_pipefd[1] );
    
    /*设置信号集函数*/
    addsig( SIGCHLD, sig_handler );
    addsig( SIGTERM, sig_handler );
    addsig( SIGINT,  sig_handler );
    addsig( SIGPIPE, SIG_IGN );

}

/*父进程中m_id值为-1，子进程中m_id值大于等于0,我们据此判断接下来要运行的是父进程代码还是子进程代码*/
template< typename T >
void processpool<T>::run(){
    if (m_idx != -1){
        run_child();
        return;
    }
    run_parent();
}

template <typename T>
void processpool<T>::run_child(){
    setup_sig_pipe();
    
    /*每个子进程都通过其在进程池中的序号值m_idx找到与父进程通信的管道*/
    int pipefd = m_sub_process[ m_idx ].m_pipefd[ 1 ];
    /*子进程需要监听管道文件描述符pipefd,因为父进程将通过它来通知子进程accept新连接*/
    addfd( m_epollfd, pipefd );

    epoll_event events[ MAX_EVENT_NUMBER ];
    T* users = new T [ USER_PER_PROCESS ];
    assert(users != nullptr);
    
    int number = 0;
    int ret = -1;

    while( !m_stop ){
        number = epoll_wait( m_epollfd, events, MAX_EVENT_NUMBER, -1 );
        if ( number < 0 && errno != EINTR ){
            printf("epoll failure\n");
            break;
        }
        for (int i = 0; i < number; ++i){
            int sockfd = events[i].data.fd;
            if ( sockfd == pipefd && events[i].event & EPOLLIN ){
                int client = 0;
                /*从父、子进程之间的管道读取数据，并将结果保存在变量client中，如果读取成功，则表示有客户连接到来*/
                ret = recv( sockfd, (char* )&client, sizeof client, 0);
                if ((ret < 0 && errno != EAGAIN) || ret == 0){
                    continue;
                }else{
                    struct sockaddr_in client_address;
                    socklen_t client_addrlength = sizeof client_address;
                    int connfd = accept(m_listenfd, (struct sockaddr*)&client_address, &client_addrlength);
                    if (connfd < 0){
                        printf("errno is %d\n", errno);
                        continue;
                    }

                    addfd( m_epollfd, connfd);
                    /*模板类T必须实现init方法，以初始化一个客户端连接，我们直接使用connfd来索引逻辑处理对象（T类型的对象）
                    * 以提高程序效率*/
                    users[connfd].init( m_epollfd, connfd, client_address );
                }        
            }else if (sockfd == sig_pipefd[0] && events[i].event & EPOLLIN){
                int sig;
                char signal[1024];
                ret = recv( sig_pipefd[0], signal, sizeof signal, 0);
                if (ret <= 0){
                    continue;
                }else{

                    for (int j = 0; j < ret; ++j){
                        switch( signal[j] ){
                        case SIGCHLD:{
                                         pid_t  pid;
                                         int stat;
                                         while( (pid = waitpid(-1, &stat, WNOHANG)) > 0){
                                             continue;
                                         }
                                         break;
                                     }
                        case SIGINT:
                        case SIGTERM:{
                                         m_stop = true;
                                         break;
                                     }
                        default:{
                                    break;
                                }
                        }
                    }
                }
            }else if (events[i].event & EPOLLIN){
                users[sockfd].process();
            }else{
                continue;
            }
        }
    }
    delete [] users;
    users = nullptr;
    close( pipefd );

    close( m_epollfd );
}
































































































































































int main(int argc, char** argv)
{
    if (argc <=2){
        printf("uage: %s ip_address port_number\n", basename( argv[0] ));
        return 1;
    }
    printf("main done!\n");
    return 0;
}

