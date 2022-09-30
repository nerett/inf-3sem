#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

/* Дочерний процесс, анализирующий год и возвращающий различные сигналы,
 * в зависимости от его високосности.
 */
int main( int argc, char** argv )
{
	int year = 0;
	if( argc < 2 ) 
	{
		fprintf( stderr, "child: too few arguments\n" );
		return 2;
	}

	year = atoi( argv[1] );

	if( year <= 0 )
    {
		return 2;
    }
    
/*Объяснить как считаем условие*/
	if( ( ( year % 4 == 0 ) && ( year % 100 != 0 ) ) || ( year % 400 == 0 ) )
    {
        //printf( "[CHILD] leap\n" );
		kill( getppid(), SIGUSR1 );
    }
	else
    {
        //printf( "[CHILD] not leap\n" );
		kill( getppid(), SIGUSR2 );
    }
        
	return 0;
}
