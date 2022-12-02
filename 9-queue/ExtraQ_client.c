#include "ExtraQ.h"


//------------------------------CLIENT FUNCTION------------------------------
clientid_t request_addr( char* name )
{
    char request_string[MAXMSGLENGTH] = "";
    sprintf( request_string, "%d:%s", MYADDR * REQUEST_ID_MOD, name );
    send_message( REQUEST_ID_MTYPE, request_string );
    
    struct mymsgbuf mybuf = { 0 };
    receive_message( &mybuf, REQUEST_ID_MOD );
    
    clientid_t id = 0;
    sscanf( mybuf.mtext, "%d", &id );
    
    return id;
}

void reg_name( char* myname )
{
    char request_string[MAXMSGLENGTH] = "";
    sprintf( request_string, "%d:%s", MYADDR * REGISTER_MOD, myname );
    send_message( REGISTER_MTYPE, request_string );
    
    struct mymsgbuf mybuf = { 0 };
    receive_message( &mybuf, REGISTER_MOD );
    sscanf( mybuf.mtext, "%d", &MYADDR );
}


//------------------------------THREAD FUNCTION------------------------------
void* listen_func( void* arg )
{
    while( RUN_FLAG )
    {
        struct mymsgbuf mybuf = { 0 };
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
        
        clientid_t rcv_addr = request_addr( rcv_name );
        send_message( rcv_addr, message );
    }
    
    return NULL;
}

void* keepalive_func( void* arg )
{
    char request_string[MAXMSGLENGTH] = "";
    sprintf( request_string, "%d", MYADDR );
    
    while( RUN_FLAG )
    {
        send_message( KEEP_ALIVE_MTYPE, request_string );
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
