#include "http_conn.h"

/*定义HTTP响应的一些状态信息*/
const char* ok_200_title = "OK";
const char* error_400_title = "Pad Request";
const char* error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char* error_403_title = "Forbidden";
const char* error_403_title_form = "Youd do not have permission to get file from this server.\n";
const char* error_404_title = "Not Found";
const char* error_404_form = "The requestted file was not found on this server.\n";
const char* error_500_title = "Internal Error";
const char* error_500_form = "There was an unusual problem serving the requested file.\n";

/*网站根目录*/
const char* doc_root = "/var/www/html";

int setnonblocking( int fd ){
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, new_option );
    return old_option;
}

void addfd( int epollfd, int fd, bool one_shot ){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (one_shot){
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

void removefd( int epollfd, int fd ){
    epoll_ctl( epollfd, EPOLL_CTL_DEL, fd, 0);
    close( fd );
}

void modfd( int epollfd, int fd, int op ){
    
    epoll_event event;
    event.data.fd = fd;
    event.events = op | EPOLLIN | EPOLLIN | EPOLLONESHOT;
    epoll_ctl( epollfd, EPOLL_CTL_MOD, fd, &event );
};

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;



void http_conn::close_conn( bool real_close ){
    if (real_close && m_sockfd != -1){
        removefd( m_epollfd, m_sockfd );
        m_sockfd = -1;
        m_user_count--;//关闭一个连接时，将客户总量减1
    }
}

void http_conn::init( int sockfd, const sockaddr_in& addr ){
    m_sockfd = sockfd;
    m_address = addr;
    /*如下两行是为了避免TIME_WAIT状态，仅用于调试，实际使用时应该去掉*/
    int reuse = 1;
    setsockopt( m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse );
    addfd( m_epollfd, m_sockfd, true );
    m_user_count++;
    init();
}

void http_conn::init(){
    m_check_state = CHECK_STATE::CHECK_STATE_REQUESTLINE;
    m_linger = false;

    m_method = METHOD::GET;
    m_url = 0;
    m_version = 0;
    m_content_length = 0;
    m_host = 0;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;

    memset( m_read_buf, '\0', READ_BUFFER_SIZE );
    memset( m_write_buf, '\0', WRITE_BUFFER_SIZE );
    memset( m_real_file , '\0', FILENAME_LEN);
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

