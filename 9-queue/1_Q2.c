#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

const char PATHNAME[] = "1_Q1.c";

int main( void )
{
    struct mymsgbuf
    {
        long mtype;
        char mtext[81];
    } mybuf = { 0 };
        
    key_t key = ftok( PATHNAME, 0 );
    
    int msqid = 0;
    if( ( msqid = msgget( key, 0666 | IPC_CREAT ) ) < 0 )
    {
        printf( "Can\'t get msqid\n" );
        exit( -1 );
    }
    
    int len = 0, maxlen = 0;
    while( 1 ) 
    {
        maxlen = 81;
        if( ( len = msgrcv( msqid, ( struct msgbuf* )&mybuf, maxlen, 0, 0 ) ) < 0 )
        {
            printf( "Can\'t receive message from queue\n" );
            exit( -1 );
        }

        if( mybuf.mtype == LAST_MESSAGE )
        {
            msgctl( msqid, IPC_RMID, ( struct msqid_ds* )NULL );
            exit( 0 );
        }
        
        printf( "message type = %ld, info = %s\n", mybuf.mtype, mybuf.mtext );
    }    
    return 0;       
}
