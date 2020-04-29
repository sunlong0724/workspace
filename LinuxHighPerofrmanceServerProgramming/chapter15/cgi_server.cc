/*************************************************************************
*File Name: cgi_server.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月28日 星期二 20时18分33秒
 ************************************************************************/
#include "processpoll.h"
class cgi_conn{
public:
    cgi_conn(){}
    ~cgi_conn(){}

    void init(int epoll_fd, int sockfd, const sockaddr_in& client_addr){
        m_epollfd = epoll_fd;
        m_sockfd = sockfd;
        m_address = client_addr;
        memset( m_buf, '\0', BUFFER_SIZE );
        m_read_idx = 0;
    }

    void process(){
        int idx = 0;
        int ret = -1;
        /*循环读取和分析客户端数据*/
        while(true){
            idx = m_read_idx;
            ret = recv(m_sockfd, m_buf + idx, BUFFER_SIZE-1-idx, 0);
            /*如果读操作发生错误，则关闭客户连接。但如果是暂时无数据可读，则退出循环*/
            if (ret < 0){
                if (errno != EAGAIN){
                    removefd(m_epollfd, m_sockfd);
                }
                break;
            }else if( ret == 0 ){
                removefd( m_epollfd, m_sockfd );
                break;
            }else{
                m_read_idx += ret;
                printf("user content is %s\n", m_buf);
                /*如果遇到字符"\r\n",则开始处理客户请求*/
                for (; idx < m_read_idx; ++idx){
                    if ( idx >=1 && m_buf[idx-1] == '\r' && m_buf[idx] == '\n' ){
                        break;
                    }
                }
                /*如果没有遇到字符"\r\n",则需要读取更多客户数据*/
                if (idx == m_read_idx){
                    continue;
                }
                m_buf[ idx-1 ]= '\0';
                
                char* file_name = m_buf;
                /*判断客户要运行的cgi程序是否存在*/
                if (access ( file_name, F_OK ) == -1){
                    removefd( m_epollfd, m_sockfd);
                    break;
                }
                /*创建子进程来执行CGI程序      * */
                ret = fork();
                if (ret == -1){
                    removefd(m_epollfd, m_sockfd);
                    break;
                }else if (ret > 0){
                    /*父进程只需关闭连接*/
                    removefd( m_epollfd, m_sockfd );
                    break;
                }else{
                    /*子进程将标准输出定向到m_sockfd, 并执行CGI*/
                    close( STDOUT_FILENO );
                    dup( m_sockfd );
                    execl( m_buf, m_buf, 0 );
                    exit( 0 );
                }
            }
        }
    }

private:
    /*读缓冲区的大小*/
    static const int    BUFFER_SIZE = 1024;
    static int          m_epollfd;
    int                 m_sockfd;
    sockaddr_in         m_address;
    char                m_buf[ BUFFER_SIZE ];
    int                 m_read_idx;
};

int cgi_conn::m_epollfd = -1;

int main(int argc, char** argv)
{
    if (argc <= 2){
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }

    const char* ip = argv[1];
    const short port = atoi( argv[2] );
    int listenfd = socket ( AF_INET, SOCK_STREAM, 0 );
    assert (listenfd >= 0);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof address);
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = port;

    ret = bind( listenfd, (struct sockaddr*)&address, sizeof address );
    assert (ret != -1);
    
    ret = listen( listenfd, 5 );
    assert (ret != -1);

    processpool< cgi_conn> *pool = processpool< cgi_conn>::create( listenfd ,1);
    if (pool) {
        pool->run();
        delete pool;
    }
    close( listenfd );

    return 0;
}























































































































