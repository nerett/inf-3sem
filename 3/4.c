/* разобраться как работает, написать комментарии,
   в том числе ко всем параметрам. */
#include <sys/shm.h>
#include <stdio.h>

int main( int argc, char** argv )
{
	int shm_id;
	char* shm_buf;
	
	if( argc < 2 ) 
	{
		fprintf( stderr, "Too few arguments\n" );
		return 1;
	}

	sscanf( argv[1], "%d", &shm_id ); //argv[1];
    printf( "searching for shm_id %d\n", shm_id );
	shm_buf = ( char* )shmat( shm_id, NULL, 0 );
	if( shm_buf == ( char* )( -1 ) ) 
	{
		fprintf( stderr, "shmat() error\n" );
		return 1;
	}

	printf( "Message: %s\n", shm_buf );
	shmdt( shm_buf );

	return 0;
}
