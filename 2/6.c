#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
//#include <string.h>

int main()
{
    pid_t chpid = fork();
    pid_t pid = getpid();
    pid_t id = 0;
    pid_t msg_pid = 0;
    
    if( chpid == 0 )
    {
        //child process
        
        id = getppid();
        printf( "[CHILD] process started, pid = %d, id = %d\n", pid, id );
        
        char readbuf[100] = "\0";
        //int lastpos = 0, lastlastpos = 0; 

        while( true )
        {
            msg_pid = id;
            
            
            FILE* infile = NULL;
            infile = fopen( "icq.txt", "r" );
            assert( infile );
            setvbuf( infile, NULL, _IONBF, 0 );
            
            int error_code = fscanf( infile, "%d:%s\n", &msg_pid, readbuf ); //FIXME
            //lastpos = ftell( infile );
            //printf( "[CHILD] lastpos = %d\n", lastppos );
            
            fclose( infile );
            infile = NULL;
            
            if( msg_pid != id ) // && lastpos != laslastpos
            {
                //fscanf( infile, "%s", readbuf );
                system( "echo > icq.txt" ); //FIXME
                printf( "[NEW MESSAGE][%lld] %s\n", msg_pid, readbuf );
            }
            
            sleep( 2 );
        }
    }
    else
    {
        //parent process
        
        id = getpid();
        printf( "[PARENT] process started, pid = %d, id = %d\n", pid, id );
        
        FILE* outfile = NULL;
        char writebuf[100] = "\0";

        while( true )
        {
            //printf( "[YOUR MESSAGE] " );
            scanf( "%s", writebuf );
            //fgets( writebuf, 50, stdin );
            printf( "[PARENT] writebuf = %s\n", writebuf );
            
            
            outfile = fopen( "icq.txt", "w" );
            assert( outfile );
            setvbuf( outfile, NULL, _IONBF, 0 );
            
            fprintf( outfile, "%d:%s\n", id, writebuf );
            
            fclose( outfile );
            outfile = NULL;
        }
    }
    
    return 0;
}
