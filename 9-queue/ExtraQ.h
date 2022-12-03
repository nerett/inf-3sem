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

//! gcc -pthread ...

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXMSGLENGTH 128

#define N_NAME_RESOLVE_TH 10
#define MAXCLIENTS 100
#define MAXNAMELENGTH 20

typedef int clientid_t;

struct mymsgbuf
{
    long mtype;
    char mtext[MAXMSGLENGTH];
};

/* 
 * Общение с сервером (арбитром очереди) происходит сразу через несколько "каналов" (типов сообщений):
 * X, X0 и X00, где X - адрес клиента (тип сообщений, на которые он реагирует).
 * Адрес клиенту присваивает сервер, адрес сервера считается равным 1 (для сервера X всегда равен 1).
 * Через определённый канал отправляются сообщения строго определённого назначения, что позволяет обрабатывать
 * заведомо разные сообщения разным образом (в т.ч. разными потоками), при этом сохраняется простота и
 * унификация адресации при небольшом количестве общеизвестных параметров.
*/

const char PATHNAME[] = "arbitrator_sync.txt";

const int ADDRESS_OFFSET = 2;
const int INITIAL_ACTIVITY = 2;

const char NULL_STRING[MAXNAMELENGTH] = "";

const id_t REQUEST_ID_MTYPE = 10;
const id_t REGISTER_MTYPE = 100;
const id_t KEEP_ALIVE_MTYPE = 1;

const int MESSAGE_MOD = 1;
const int REQUEST_ID_MOD = 10;
const int REGISTER_MOD = 1;
const int KEEP_ALIVE_MOD = 100;

const int KEEPALIVE_TIMEOUT = 5;
const int RETRY_TIMEOUT = 5;

clientid_t MYADDR = 0;
int MSQID = 0;
char MYNAME[MAXMSGLENGTH] = "";

int RUN_FLAG = 1;


//------------------------------SHARED FUNCTION------------------------------
void delete_message_queue()
{
    msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
}

void configure_message_queue()
{
    key_t key = ftok( PATHNAME, 0 );
    
    if( ( MSQID = msgget( key, 0666 | IPC_CREAT ) ) < 0 )
    {
        printf( "Can\'t get msqid\n" );
        delete_message_queue();
        exit( -1 );
    }
}

void send_message( clientid_t snd_mtype, char* message )
{
    struct mymsgbuf mybuf = { 0 };
    mybuf.mtype = snd_mtype;
    
    int msg_length = strlen( message ) + 1;
    if( msg_length > MAXMSGLENGTH )
    {
        fprintf( stderr, "Message is above maximum length\n" );
        delete_message_queue();
        exit( -1 );
    }
    
    strcpy( mybuf.mtext, message );
    if( msgsnd( MSQID, ( struct msgbuf* )&mybuf, msg_length, 0 ) < 0 )
    {
        fprintf( stderr, "Can\'t send message to queue\n" );
        delete_message_queue();
        exit( -1 );
    }
}

int receive_message( struct mymsgbuf* rcv_buf, int modifier )
{    
    int msg_length = 0;
    if( ( msg_length = msgrcv( MSQID, ( struct msgbuf* )&rcv_buf, MAXMSGLENGTH, MYADDR * modifier, 0 ) ) < 0 )
    {
        fprintf( stderr, "Can\'t receive message from queue\n" );
        delete_message_queue();
        exit( -1 );
    }
    
    return msg_length;
}
