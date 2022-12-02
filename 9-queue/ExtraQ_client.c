//Задание похоже на 2, но использует отдельный процесс - 'арбитр очереди'

// Арбитр обслуживает очередь, часть сообщений он пропускает через себя и раздаёт потребителям.
// Арбитр очищает очередь после завершения работы одного из абонентов,
// регистрирует абонентов и даёт им идентификаторы.

// Каждый клиент при запуске представляется простым именем, и получает свой идентификатор.
// Клиент должен раз в 10 секунд дёргать арбитра(послать служебное сообщение),
// если он этого не сделает, то арбитр удалит его из списка доступных клиентов.

// При необходимости передать сообщение адресату,
// клиент сначала получает от арбитра номер принимающего абонента,
// а потом отправляет сообщение уже на этот номер

//  Работа в клиенте начинается с регистрации текущего пользователя
//  #<Имя>
//  Передача сообщения: <Имя>#<Сообщение>
//  Выход из имени - ## 

//  Сдесь изложены лишь наброски алгоритма, начинать необходимо с прорабтоки
//  алгоритмов работы клиента и арбитра.
//  Клиента удобнее реализовывать на потоках.

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXMSGLENGTH 128

typedef int clientid_t;

const char[] PATHNAME = "arbitrator_sync.txt";

const id_t REQUEST_ID_MTYPE = 0;
const id_t REGISTER_MTYPE = 1;
const id_t KEEP_ALIVE_MTYPE = 2;

const int MESSAGE_MOD = 1;
const int REQUEST_ID_MOD = 10;
const int REGISTER_MOD = 100;

const int KEEPALIVE_TIMEOUT = 5;

clientid_t MYADDR 9999;
int MSQID = 0;
char MYNAME[MAXMSGLENGTH] = "";

int RUN_FLAG = 1;


struct mymsgbuf
{
    long mtype;
    char mtext[MAXMSGLENGTH];
};


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
