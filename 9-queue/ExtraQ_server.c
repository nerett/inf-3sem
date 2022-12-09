#include "ExtraQ.h"


struct client_t
{
    char name[MAXNAMELENGTH];
    clientid_t address;
    
    int active;
};

int NAMETABLE_ACCESSED = 0; //!TODO semaphore

struct client_t clients[MAXCLIENTS] = { 0 };
int n_clients = 0;

//------------------------------SERVER FUNCTION------------------------------
void lock_nametable()
{
    while( NAMETABLE_ACCESSED );
    NAMETABLE_ACCESSED = 1;
}

void unlock_nametable()
{
    NAMETABLE_ACCESSED = 0;
}

//------------------------------THREAD FUNCTION------------------------------
void* nameresolve_func( void* arg )
{
    while( RUN_FLAG )
    {
        struct mymsgbuf mybuf = { 0 };
        receive_message( &mybuf, REQUEST_ID_MOD );
        
        clientid_t sender_addr = 0;
        char requested_name[MAXNAMELENGTH] = "";
        sscanf( mybuf.mtext, "%d:%s", &sender_addr, requested_name );
        
        lock_nametable();
        
        //!TODO hashtable
        clientid_t requested_addr = 0;
        for( int i = ADDRESS_OFFSET; i < MAXCLIENTS; i++ )
        {
            if( !strcmp( requested_name, clients[i].name ) )
            {
                requested_addr = i;
                break;
            }
        }
        
        unlock_nametable();
        
        char request_string[MAXMSGLENGTH] = "";
        sprintf( request_string, "%d", requested_addr );
        send_message( sender_addr, request_string );
    }
    
    return NULL;
}

void* register_func()
{
    while( RUN_FLAG )
    {
        struct mymsgbuf mybuf = { 0 };
        receive_message( &mybuf, REGISTER_MOD );
        
        clientid_t tmp_sender_addr = 0;
        char register_name[MAXNAMELENGTH] = "";
        fprintf( stderr, "[register_func] Received mybuf.mtext = %s\n", mybuf.mtext );
        sscanf( mybuf.mtext, "%d:%s", &tmp_sender_addr, register_name );
        
        clientid_t register_addr = 0;
        int unique = 1;
        
        fprintf( stderr, "[register_func] lock_nametable() attempt\n" );
        lock_nametable();
        fprintf( stderr, "[register_func] lock_nametable() success\n" );
        
        for( int i = ADDRESS_OFFSET; i < MAXCLIENTS; i++ )
        {
            if( !strcmp( clients[i].name, register_name ) )
            {
                unique = 0;
                break;
            }
        }
        
        if( unique )
        {
            for( int i = ADDRESS_OFFSET; i < MAXCLIENTS; i++ )
            {
                if( !strcmp( clients[i].name, "" ) )
                {
                    strncpy( clients[i].name, register_name, MAXNAMELENGTH );
                    register_addr = n_clients + ADDRESS_OFFSET;
                    clients[i].address = register_addr;
                    clients[i].active = INITIAL_ACTIVITY;
                    
                    n_clients++;
                    break;
                }
            }
        }
        
        unlock_nametable();
        
        char request_string[MAXMSGLENGTH] = "";
        sprintf( request_string, "%s:%d", register_name, register_addr ); //!TODO случайное число для увеличения защиты от коллизий
        send_message( tmp_sender_addr, request_string );
    }
}

void* keepalive_func( void* arg )
{
    //!TODO двухступенчатая схема с is_active
    //!TODO добавить очистку сообщений для неактивных пользователей
    
    while( RUN_FLAG )
    {
        lock_nametable();
        
        struct mymsgbuf mybuf = { 0 };
        while( msgrcv( MSQID, ( struct msgbuf* )&mybuf, MAXMSGLENGTH, MYADDR * KEEP_ALIVE_MOD, 0 & IPC_NOWAIT ) >= 0 ) //>=0 >0
        {
            clientid_t sender_addr = 0;
            char sender_name[MAXMSGLENGTH] = "";
            sscanf( mybuf.mtext, "%d:%s", &sender_addr, sender_name ); //!TODO ассоциативный массив
            
            if( ( sender_addr > MAXCLIENTS ) || strcmp( clients[sender_addr].name, sender_name ) ) //! < 0 ?
            {
                fprintf( stderr, "Client address does not match its name\n" );
                continue;
            }
            
            clients[sender_addr].active = INITIAL_ACTIVITY;
        }
        
        for( int i = ADDRESS_OFFSET; i < MAXCLIENTS; i++ ) //active -> inactive & inactive -> remove
        {
            clients[i].active--;
            
            if( clients[i].active < 0 )
            {
                strncpy( clients[i].name, NULL_STRING, MAXNAMELENGTH );
                clients[i].address = 0;
                clients[i].active = 0;
                
                n_clients--;
            }
        }
        
        unlock_nametable();
        
        sleep( KEEPALIVE_TIMEOUT );
    }
    
    return NULL;
}

//------------------------------MAIN FUNCTION------------------------------
int main()
{
    printf( "[ARBITRATOR] Starting...\n" );
    
    MYADDR = 1;
    NAMETABLE_ACCESSED = 0;
    pthread_t nameresolve_thread[N_NAME_RESOLVE_TH] = { 0 }, keepalive_thread = 0, register_thread = 0;
    
    printf( "[ARBITRATOR] Creating additional threads...\n" );
    size_t errors = pthread_create( &keepalive_thread, NULL, &keepalive_func, NULL );
    errors += pthread_create( &register_thread, NULL, &register_func, NULL );
    for( int i = 0; i < N_NAME_RESOLVE_TH; i++ )
    {
        errors += pthread_create( &nameresolve_thread[i], NULL, &nameresolve_func, NULL );
    }

    if( errors )
    {
        fprintf( stderr, "Cannot create 1 or more threads\n" );
        delete_message_queue();
        exit( -1 );
    }
    printf( "[ARBITRATOR] Threads created successfully.\n" );
    
    pthread_join( keepalive_thread, NULL );
    pthread_join( register_thread, NULL );
    for( int i = 0; i < N_NAME_RESOLVE_TH; i++ )
    {
        pthread_join( nameresolve_thread[i], NULL );
    }
    
    printf( "[ARBITRATOR] Exiting...\n" );
    return 0;
}
