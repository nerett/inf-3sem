//#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid, ppid;
    pid = getpid();
    ppid = getppid();
    
    printf( "pid = %d\nppid = %d\n", pid, ppid );
    return 0;
}
