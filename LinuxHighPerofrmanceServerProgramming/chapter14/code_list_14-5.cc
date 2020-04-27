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

#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>

#define handle_error_en(en, msg) \
    do{errno = en; printf(msg); exit( EXIT_FAILURE );}while(0)

static void *sig_thread(void* argv){
    sigset_t *set = (sigset_t*) argv;
    int s, sig;
    
    for (;;){
         s = sigwait( set, &sig );
         if (s != 0)
            handle_error_en( s, "sigwait" );

         printf("Signal handling thread got signal %d\n", sig);
    }
}
int main(int argc, char** argv)
{
    pthread_t thread;
    sigset_t set;
    int s;

    sigemptyset( &set );
    sigaddset( &set, SIGQUIT );
    sigaddset( &set, SIGUSR1 );
    s = pthread_sigmask( SIG_BLOCK, &set, nullptr );
    if (s != 0){
        handle_error_en(s, "pthread_sigmask");
    }

    s = pthread_create( &thread, nullptr, &sig_thread, (void*)&set );
    if (s != 0){
        handle_error_en(s, "pthread_create");
    }

    pause();
    return 0;
}

