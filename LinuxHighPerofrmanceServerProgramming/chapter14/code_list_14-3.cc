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
#include <wait.h>

pthread_mutex_t mutex;
int pid;

void* another( void* arg ){
    printf("%s current pid %d, %d\n", __FUNCTION__,getpid(),pid );
    printf( " in child thread, lock the mutex\n" );
    pthread_mutex_lock( &mutex );
    sleep( 5 );
    pthread_mutex_unlock( &mutex );
    printf( " leave the  child thread\n" );
}


void prepare(){
    printf("%s current pid %d, %d\n", __FUNCTION__,getpid(),pid );
    pthread_mutex_lock( &mutex );
}

void infork(){
    printf("%s current pid %d, %d\n", __FUNCTION__,getpid(),pid );
    pthread_mutex_unlock( &mutex );
}


int main(int argc, char** argv)
{

    pthread_mutex_init( &mutex, nullptr);
    pthread_t id;

    pthread_create( &id, nullptr, another, nullptr );

    sleep(1);

    int ret  = pthread_atfork( prepare, infork, infork );
    assert  (ret == 0);
    pid = fork(); 
    printf("%s fork done, current pid %d, %d\n", __FUNCTION__,getpid(),pid );
    if ( pid < 0){
        pthread_join( id, nullptr );
        pthread_mutex_destroy( &mutex );
        return 1;
    }else if (pid == 0){
        printf( "I am in the child process, want to get the lock\n" );
        pthread_mutex_lock( &mutex );
        printf( "I can not run to here, oop....\n" );
        pthread_mutex_unlock( &mutex );
        exit( 0 );
    }else{
        wait( nullptr );
    }

    pthread_join( id, nullptr );
    pthread_mutex_destroy( &mutex );

    printf("main done!\n");
    return 0;
}

