#include "../net.h"

int main(int argc, char** argv)
{
    if (argc < 3){
        fprintf(stdout,"%s ip port snd_buff_size\n", basename(argv[0]));
        return 1;
    }

    const char* ip_address = argv[1];
    const short port       = atoi( argv[2] );
    

    sockaddr_in  server_addr;
    bzero(&server_addr, sizeof server_addr);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip_address, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert( sock >= 0 );

    int snd_buf_size       = 0;
    int snd_buf_size_len   = sizeof snd_buf_size;

    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &snd_buf_size, snd_buf_size_len);
    fprintf(stdout,"the tcp send buffer size before setting is %d\n", snd_buf_size);

    snd_buf_size = atoi( argv[3] );
    
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &snd_buf_size, (socklen_t*)&snd_buf_size_len);
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &snd_buf_size, snd_buf_size_len);
    fprintf(stdout,"the tcp send buffer size after setting is %d\n", snd_buf_size);

    //控制close在关闭tcp连接时的行为
    struct linger sl;
    sl.l_onoff = 0;
    sl.l_linger = 0;
    setsockopt(sock, SOL_SOCKET, SO_LINGER, &sl, sizeof sl );

    int f_getfd = fcntl(sock, F_GETFD);
    int ret = fcntl(sock, F_SETFD, (f_getfd | O_NONBLOCK));
    assert (ret == 0);

    ret = connect(sock, (struct sockaddr*)&server_addr, sizeof server_addr);
    if (ret < 0){
        fprintf(stdout,"connect failed, err %d\n", errno);

    }else{

        fprintf(stdout,"connect successfully, err %d\n", errno);
        //close(STDOUT_FILENO);
        int ret = dup(sock);
        printf("%d abcdef", ret);

        char buf[MAX_BUFF_SIZE*2];
        memset(buf, 'a', sizeof buf -1); 
        buf[MAX_BUFF_SIZE-1] = '\0'; 

        ret = send(sock, buf, strlen(buf), 0);

        memset(buf, 0x00, sizeof buf -1); 
        buf[MAX_BUFF_SIZE-1] = '\0'; 
        while(false && recv(sock, buf, sizeof buf, 0) > 0){
            fprintf(stdout,"%s\n", buf);
        }
       
    }
    shutdown( sock, SHUT_RD );

    close(sock);
    
    return 0;
}

