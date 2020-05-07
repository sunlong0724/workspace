#include <iostream>
#include <unistd.h>
#include <stdio.h>


int main()
{
    uid_t uid = getuid();
    uid_t euid = geteuid();
    printf( "userid is %d, effective userid is %d\n", uid, euid );
    std::cout << "Hello world" << std::endl;
    return 0;
}

