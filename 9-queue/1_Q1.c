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

    /* Create or attach message queue  */
    key_t key = ftok( PATHNAME, 0 );
    int msqid = 0;
    if( ( msqid = msgget( key, 0666 | IPC_CREAT ) ) < 0 )
    {
        printf( "Can\'t get msqid\n" );
        exit( -1 );
    }

    /* Send information */
    int len = 0;
    for( int i = 1; i <= 5; i++ )
    {
        mybuf.mtype = 1;
        strcpy( mybuf.mtext, "This is text message" );
        len = strlen( mybuf.mtext ) + 1; //+ sizeof( long ); /* sizeof long? */

        if( msgsnd( msqid, ( struct msgbuf* )&mybuf, len, 0 ) < 0 )
        {
            printf( "Can\'t send message to queue\n" );
            msgctl( msqid, IPC_RMID, ( struct msqid_ds* )NULL );
            exit( -1 );
        }
    }

    /* Send the last message */
    mybuf.mtype = LAST_MESSAGE;
    len = 0;

    if( msgsnd( msqid, ( struct msgbuf* )&mybuf, len, 0 ) < 0 )
    {
        printf( "Can\'t send message to queue\n" );
        msgctl( msqid, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
    
    printf( "Waiting...\n" );
    sleep( 10 );
    msgctl( msqid, IPC_RMID, ( struct msqid_ds* )NULL );

    return 0;
}
