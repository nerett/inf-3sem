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
    srand( time( NULL ) );
    
    while( RUN_FLAG )
    {
        clientid_t tmp_addr = rand() % TMP_ADDRESS_RANGE + MAXCLIENTS;
        MYADDR = tmp_addr;
        fprintf( stderr, "[reg_name] Acquired tmp_address = %d\n", tmp_addr );
        
        char request_string[MAXMSGLENGTH] = "";
        sprintf( request_string, "%d:%s", tmp_addr * REGISTER_MOD, myname ); //MYADDR * REGISTER_MOD
        fprintf( stderr, "[reg_name] Sending request_string = %s\n", request_string );
        send_message( REGISTER_MTYPE, request_string );
        
        char ret_name[MAXMSGLENGTH] = "";
        clientid_t ret_addr = 0;
        
        struct mymsgbuf mybuf = { 0 };
        receive_message( &mybuf, REGISTER_MOD );
        fprintf( stderr, "[reg_name] Received mybuf.mtext = %s\n", mybuf.mtext );
        sscanf( mybuf.mtext, "%d:%s", &ret_addr, ret_name );
        
        if( ret_addr && !strcmp( myname, ret_name ) ) //!TODO случайное число как отдельный параметр для улучшения защиты от коллизий
        {
            MYADDR = ret_addr;
            return;
        }
        
        fprintf( stderr, "[reg_name] Registration failed, retrying in %d seconds...\n", RETRY_TIMEOUT );
        sleep( RETRY_TIMEOUT );
    }
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
        char rcv_name[MAXNAMELENGTH] = "";
        char message[MAXMSGLENGTH] = "";
        scanf( "%s:%s", rcv_name, message );
        
        clientid_t rcv_addr = request_addr( rcv_name );
        if( !rcv_addr )
        {
            printf( "%s is offline.\n", rcv_name );
            continue;
        }
        
        send_message( rcv_addr, message );
    }
    
    return NULL;
}

void* keepalive_func( void* arg )
{
    char request_string[MAXMSGLENGTH] = "";
    sprintf( request_string, "%d:%s", MYADDR, MYNAME );
    
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
    configure_message_queue();
    
    printf( "Enter your name: " );
    scanf( "%s", MYNAME );
    
    printf( "Registering...\n" );
    reg_name( MYNAME );
    printf( "Registered successfully." );
    
    pthread_t listen_thread = 0, speak_thread = 0, keepalive_thread = 0;;
    if( !pthread_create( &listen_thread, NULL, &listen_func, NULL ) || !pthread_create( &speak_thread, NULL, &speak_func, NULL ) || !pthread_create( &keepalive_thread, NULL, &keepalive_func, NULL )  )
    {
        fprintf( stderr, "Cannot create thread\n" );
        delete_message_queue();
        exit( -1 );
    }
    
    pthread_join( listen_thread, NULL );
    pthread_join( speak_thread, NULL );
    pthread_join( keepalive_thread, NULL );
    
    return 0;
}
