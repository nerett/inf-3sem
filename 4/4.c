#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*Восстановить нормальное поведение программы через 5 принятых сигналов.*/
sig_atomic_t sig_occured = 0;

void sig_handler( int snum )
{
	sig_occured = 1;
}

int main( void )
{
	struct sigaction act;
	sigemptyset( &act.sa_mask );
	act.sa_handler = &sig_handler;
	act.sa_flags = 0;

	if( sigaction( SIGINT, &act, NULL ) == -1 ) 
	{
		fprintf( stderr, "sigaction() error\n" );
		return 1;
	}

	int nsignals = 0;
	while( 1 ) 
	{
		if( sig_occured )
		{
			fprintf( stderr, "signal...\n" );
			sig_occured = 0;
            nsignals++;
		}
		
		if( nsignals >=5 )
        {
            sigemptyset( &act.sa_mask );
            act.sa_handler = NULL;
            act.sa_flags = 0;
            sigaction( SIGINT, &act, NULL );
        }
	}
	
	return 0;
}
