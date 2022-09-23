#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int print_result = 0;

void sig_print_result( int snum )
{
	print_result = 1;
}

int main()
{
    printf( "started with pid = %d\n", getpid() );

    struct sigaction action;
	sigemptyset( &action.sa_mask );
	action.sa_handler = &sig_print_result;
	action.sa_flags = 0;

	sigaction( SIGUSR1, &action, NULL );
    
    unsigned long long result = 1;
    while( 1 )
    {
        result++;
        if( print_result )
        {
            printf( "result = %llu\n", result );
            print_result = 0;
        }
    }
    
    return 0;
}
