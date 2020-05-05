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


#define BUFFER_SIZE  4096 
/*主状态机的两种可能状态，分别表示：当前正在分析请求行，当前正在分析头部字段*/
enum class CHECK_STATE{
    REQUESTLINE = 0,
    HEADER,
};
/*从状态机的三种可能状态，即行的读取状态，分别标识为：读取到一个完整的行、
 * 行出错和行数据尚且不完善。*/
enum class LINE_STATUS{
    OK = 0,
    BAD,
    OPEN
};

/*服务器处理http请求的结果,
 * NO_REQUEST表示请求不完整，需要继续读取客户端数据；
 * GET_REQUEST表示获得了一个完整的客户请求；
 * BAD_REQUEST表示客户端；请求有语法错误；
 * FORBIDDEN_REQUEST表示客户对资源没有足够的访问权限；
 * INTERNAL_ERROR表示服务其内部错误；
 * CLOSED_CONNECTION表示客户端已经关闭连接了*/
enum class HTTP_CODE{
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSE_CONNECTION
};

/*为了简化问题， 我们没有给客户端发送一个完整的http应答报文，而只是根据服务器的处理
 * 结果发送如下从成功或失败信息*/
static const char* szret[] = {"I get a correct result\n","Something wrong\n"};

/*从状态机，用于解析出一行内容*/
LINE_STATUS parse_line( char* buffer, int& checked_index, int& read_index ){
    char temp;
    /*checked_index 指向buffer（应用程序的读缓冲区）中当前正在分析的字节，
     * read_index 指向buffer中客户数据的尾部的下一字节。buffer中第0～checked_index
     * 字节都已分析完毕，第checked_index - ( read_index-1 )字节由下面的循环
     * 挨个分析*/
    for (; checked_index < read_index; ++checked_index){
        /*获取当前要分析的字节*/
        temp = buffer [ checked_index ];
        /*如果当前的字节是'\r', 即回车符号，则说明可能读取到一个完整的行*/
        if ( temp == '\r' ){
            /*如果’\r'字符碰巧是目前buffer中的最后一个已经被读入的客户数据，
             * 那么这次分析没有读取到一个完整的行，返回LINE_OPEN以表示还需要读取客户数据才能进一步分析*/
            if ( checked_index + 1 == read_index ){
                return LINE_STATUS::OPEN;
            } 
            /*如果下一个字符是'\n', 则说明我们成功读取到一个完整的行*/
            else if ( buffer [ checked_index +1] == '\n' ){
                buffer[ checked_index ++ ] = '\0';
                buffer[ checked_index++ ] = '\0';
                return LINE_STATUS::OK;
            }
            /*否则的话，说明客户发送的HTTP请求存在语法问题*/
            return LINE_STATUS::BAD;
        }
        /*如果当前的字节是'\n',即换行符，则可能读取到一个完整的行*/
        else if ( temp == '\n' ){
            if (checked_index > 1 && buffer[ checked_index - 1 ] == '\r'){
                buffer [ checked_index -1 ] == '\0';
                buffer [ checked_index++ ] = '\0';
                return LINE_STATUS::OK;
            }
            return LINE_STATUS::BAD;
        }
    }
    /*如果所有内容都分析完毕也没有遇到’\r'字符，则返回OPEN,表示还需要继续读取客户数据才能进一步分析*/
    return LINE_STATUS::OPEN;
}

/*分析请求行*/
HTTP_CODE parse_requestline( char* temp, CHECK_STATE& checkstate ){
    char* url = strpbrk( temp, "\r" );
    /*如果请求行中没有空白字符或'\t'字符，则http请求必有问题*/
    if ( !url ){
        return HTTP_CODE::BAD_REQUEST;
    }

    *url++ = '\0';

    char* method = temp;
    if (strcasecmp( method, "GET" ) == 0){
        printf("The request method is GET\n");
    }else{
        return HTTP_CODE::BAD_REQUEST;
    }

    url += strspn( url, " \t" );
    char* version = strpbrk( url, " \t" );
    if (!version){
        return HTTP_CODE::BAD_REQUEST;
    }
    *version++ = '\0';
    version += strspn( version, " \t" );

    /*仅支持HTTP/1.1*/
    if (strcasecmp ( version, "HTTP/1.1" ) != 0){
        return HTTP_CODE::BAD_REQUEST;
    }
    /*检查url是否合法*/
    if (strncasecmp( url, "http://", 7 ) == 0){
        url += 7;
        url = strchr( url, '/' );
    }
    if (!url || url[ 0 ] != '/'){
        return HTTP_CODE::BAD_REQUEST;
    }
    printf( "The request URL is: %s\n", url );
    /*HTTP请求行处理完毕，状态转移到头部字段的分析*/
    checkstate = CHECK_STATE::HEADER;
    return HTTP_CODE::NO_REQUEST;
}

/*分析头部字段*/
HTTP_CODE parse_headers( char* temp ){
    /*如果遇到了一个空行，说明我们得到了一个正确的HTTP请求*/
    if (temp[ 0 ] = '\0'){
        return HTTP_CODE::GET_REQUEST;
    }
    else if ( strncasecmp( temp, "Host:", 5 ) == 0 ){
        temp += 5;
        temp += strspn( temp, " \t" );
        printf("the request host is: %s\n", temp);
    }else{
        printf( "I can not handle this header\n" );
    }
    return HTTP_CODE::NO_REQUEST;
}

/*分析HTTP请求的入口函数*/
HTTP_CODE parse_content( char* buffer, int& checked_index, CHECK_STATE& 
                        checkstate, int& read_index, int& start_line){
    LINE_STATUS linestatus = LINE_STATUS::OK;
    HTTP_CODE retcode = HTTP_CODE::NO_REQUEST;
    /*主状态机，用于从buffer中取出所有完整的行*/
    while( (linestatus = parse_line( buffer, checked_index, read_index )) == LINE_STATUS::OK){

        char* temp = buffer + start_line;
        start_line = checked_index;
        /*checkstate记录主机状态机当前的状态*/
        switch ( checkstate ){
        case CHECK_STATE::REQUESTLINE:
            {
                retcode = parse_requestline( temp, checkstate );
                if (retcode == HTTP_CODE::BAD_REQUEST){
                    return HTTP_CODE::BAD_REQUEST;
                }
                break;
            }

        case CHECK_STATE::HEADER:
            {
                retcode = parse_headers( temp );
                if (retcode == HTTP_CODE::BAD_REQUEST){
                    return HTTP_CODE::BAD_REQUEST;
                }else if (retcode == HTTP_CODE::GET_REQUEST){
                    return HTTP_CODE::GET_REQUEST;
                }
                break;
            }
        default:
            {
                return HTTP_CODE::INTERNAL_ERROR;
            }
        }
    }
    if ( linestatus == LINE_STATUS::OPEN ){
        return HTTP_CODE::NO_REQUEST;
    }else{
        return HTTP_CODE::BAD_REQUEST;
    }
}


int main(int argc, char** argv)
{
    if (argc <=2){
        printf("uage: %s ip_address port_number\n", basename( argv[0] ));
        return 1;
    }

    const char* ip = argv[1];
    const short port = atoi( argv[2] );

    struct sockaddr_in address;
    bzero(&address, sizeof address);
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    int listenfd = socket( AF_INET, SOCK_STREAM, 0 );
    assert ( listenfd >= 0 );
    int ret = bind( listenfd, (struct sockaddr*)&address, sizeof address);
    assert (ret != -1);
    ret = listen( listenfd, 5 );
    assert (ret != -1);

    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof client_address;
    int fd = accept( listenfd, (struct sockaddr* )&client_address, &client_addrlength);
    if ( fd < 0 ){
        printf( "errno is: %d\n", errno );
    }else{
        char buffer [ BUFFER_SIZE ];
        memset( buffer, '\0', BUFFER_SIZE );
        int data_read = 0;
        int read_index = 0;
        int checked_index = 0;
        int start_line = 0;
        /*设置主状态机的初始状态*/
        CHECK_STATE checkstate = CHECK_STATE::REQUESTLINE;
        while( 1 ){/*循环读取客户数据并分析之*/
            data_read = recv( fd, buffer + read_index, BUFFER_SIZE - read_index, 0 );
            if (data_read == -1){
                printf( "reading failed\n" );
                break;
            }
            else if (data_read == 0){
                printf( "remote client has closed the connection\n" );
                break;
            }

            read_index += data_read;
            
            HTTP_CODE result = parse_content( buffer, checked_index, checkstate,
                                              read_index, start_line);
            if ( result == HTTP_CODE::NO_REQUEST ){
                continue;
            }else if ( result == HTTP_CODE::GET_REQUEST ){
                send( fd, szret[0], strlen( szret[0] ), 0 );
                break;
            }else {
                send( fd, szret[1], strlen( szret[1] ), 0 );
                break;
            }
        }
        close( fd );
    }
    close( listenfd );
    printf("main done!\n");
    return 0;
}

