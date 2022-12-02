#include "ExtraQ.h"


//------------------------------SHARED FUNCTION------------------------------
void send_message( clientid_t snd_mtype, char* message )
{
    mymsgbuf mybuf = { 0 };
    sndbuf.mtype = snd_mtype;
    
    msg_length = strlen( message ) + 1;
    if( msg_length > MAXMSGLENGTH )
    {
        fprintf( stderr, "Message is above maximum length\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
    
    strcpy( sndbuf.mtext, message );
    if( msgsnd( MSQID, ( struct msgbuf* )&mybuf, msg_length, 0 ) < 0 )
    {
        fprintf( stderr, "Can\'t send message to queue\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
}

int receive_message( mymsgbuf* rcv_buf, int modifier )
{    
    int msg_length = 0;
    if( ( msg_length = msgrcv( MSQID, ( struct msgbuf* )&rcvbuf, MAXMSGLENGTH, MYADDR * modifier, 0 ) ) < 0 )
    {
        fprintf( stderr, "Can\'t receive message from queue\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
    
    return msg_length;
}

clientid_t request_addr( char* name )
{
    char* request_string[MAXMSGLENGTH] = "";
    sprintf( request_string, "%d:%s", MYADDR * REQUEST_ID_MOD, name );
    send_message( REQUEST_ID_MTYPE, request_string );
    
    mymsgbuf mybuf = { 0 };
    receive_message( &mybuf, REQUEST_ID_MOD );
    
    clientid_t id = 0;
    sscanf( mybuf.mtext, "%d", &id );
    
    return id;
}

void reg_name( char* myname )
{
    char* request_string[MAXMSGLENGTH] = "";
    sprintf( request_string, "%d:%s", MYADDR * REGISTER_MOD, myname );
    send_message( REGISTER_MTYPE, request_string );
    
    mymsgbuf mybuf = { 0 };
    receive_message( &mybuf, REGISTER_MOD );
    sscanf( mybuf.mtext, "%d", &MYID );
}


//------------------------------THREAD FUNCTION------------------------------
void* listen_func( void* arg )
{
    while( RUN_FLAG )
    {
        mymsgbuf mybuf = { 0 };
        receive_message( &mybuf, MESSAGE_MOD );
        
        printf( "%s\n", mybuf.mtext );
    }
    
    return NULL;
}

void* speak_func( void* arg )
{
    while( RUN_FLAG )
    {
        char rcv_name[MAXMSGLENGTH] = "";
        char message[MAXMSGLENGTH] = "";
        scanf( "%s:%s", rcv_name, message );
        
        rcv_addr = request_addr( rcv_name );
        send_message( rcv_addr, message );
    }
    
    return NULL;
}

void* keepalive_func( void* arg )
{
    while( RUN_FLAG )
    {
        send_message( KEEP_ALIVE_MTYPE, MYADDR );
        sleep( KEEPALIVE_TIMEOUT );
    }
    
    return NULL;
}


//------------------------------MAIN FUNCTION------------------------------
int main()
{
    key_t key = ftok( PATHNAME, 0 );
    
    if( ( MSQID = msgget( key, 0666 | IPC_CREAT ) ) < 0 )
    {
        printf( "Can\'t get msqid\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
    
    printf( "Enter your name: " );
    scanf( "%s", MYNAME );
    
    reg_name( MYNAME );
    
    pthread_t listen_thread = 0, speak_thread = 0, keepalive_thread = 0;;
    if( !pthread_create( &listen_thread, NULL, &listen_func, NULL ) || !pthread_create( &speak_thread, NULL, &speak_func, NULL ) || !pthread_create( &keepalive_thread, NULL, &keepalive_func, NULL )  )
    {
        fprintf( stderr, "Cannot create thread\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
    
    pthread_join( listen_thread, NULL );
    pthread_join( speak_thread, NULL );
    pthread_join( keepalive_thread, NULL );
    
    return 0;
}
