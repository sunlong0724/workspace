/*************************************************************************
*File Name: code_list_6-2.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月24日 星期五 10时05分03秒
 ************************************************************************/
#include "../net.h"

#define BUFFER_SIZE 1024
static const char* status_line[2] = {"200 OK", "500 Internal server error"};

int main(int argc, char** argv)
{
    if (argc <= 3){
        fprintf(stdout,"usage: %s ip_address port_number filename\n", basename( argv[0] ));
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi( argv[2] );

    const char* file_name = argv[3];
    struct sockaddr_in address;
    bzero(&address, sizeof address);
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons(port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert (sock >= 0);

    int ret = bind( sock, (struct sockaddr*)&address, sizeof address );
    assert ( ret != -1 );

    ret = listen(sock, 5);
    assert( ret != -1 );

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof (client);
    int connfd = accept(sock, (struct sockaddr*)&address, &client_addrlength);

    if (connfd < 0){
        fprintf(stdout,"errno is %d\n", errno);
    }else{
        char header_buf[ BUFFER_SIZE ];
        memset(header_buf, '\0', BUFFER_SIZE);
        char* file_buf=nullptr;
        struct stat file_stat;
        bool valid = true;
        int len = 0;
        if (stat(file_name, &file_stat) < 0){ //目标文件不存在
            valid = false;    
        }else{
            if ( S_ISDIR( file_stat.st_mode ) ){
                valid = false;
            }else if (file_stat.st_mode & S_IROTH){
                int fd = open( file_name, O_RDONLY );
                file_buf = new char [ file_stat.st_size + 1 ];
                memset(file_buf, '\0', file_stat.st_size + 1);
                if (read ( fd, file_buf, file_stat.st_size  ) < 0){
                    valid = false;
                }
            }else{
                valid = false;
            }
        }

        if (valid){
            ret = snprintf(header_buf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", status_line[0]);
            len += ret;
            ret = snprintf(header_buf+len, BUFFER_SIZE -1 -len, "%s", "\r\n");

            struct iovec iv[2];
            iv[ 0 ].iov_base = header_buf;
            iv[ 0 ].iov_len = strlen( header_buf );
            iv[ 1 ].iov_base = file_buf;
            iv[ 1 ].iov_len = file_stat.st_size;
            ret = writev( connfd, iv, 2 ); 
            
        }else{
            ret = snprintf(header_buf, BUFFER_SIZE-1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
            len += ret;
            ret = snprintf(header_buf+len, BUFFER_SIZE -1-len, "%s", "\r\n");
            send (connfd, header_buf, strlen(header_buf), 0);
        }
        close( connfd );
        delete[] file_buf;
    }

    close( connfd );

    return 0;
}

