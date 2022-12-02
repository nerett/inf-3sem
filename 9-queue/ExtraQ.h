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

typedef int clientid_t;

const char PATHNAME[] = "arbitrator_sync.txt";

const id_t REQUEST_ID_MTYPE = 0;
const id_t REGISTER_MTYPE = 1;
const id_t KEEP_ALIVE_MTYPE = 2;

const int MESSAGE_MOD = 1;
const int REQUEST_ID_MOD = 10;
const int REGISTER_MOD = 100;

const int KEEPALIVE_TIMEOUT = 5;

clientid_t MYADDR = 9999;
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
    struct mymsgbuf mybuf = { 0 };
    mybuf.mtype = snd_mtype;
    
    int msg_length = strlen( message ) + 1;
    if( msg_length > MAXMSGLENGTH )
    {
        fprintf( stderr, "Message is above maximum length\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
    
    strcpy( mybuf.mtext, message );
    if( msgsnd( MSQID, ( struct msgbuf* )&mybuf, msg_length, 0 ) < 0 )
    {
        fprintf( stderr, "Can\'t send message to queue\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
}

int receive_message( struct mymsgbuf* rcv_buf, int modifier )
{    
    int msg_length = 0;
    if( ( msg_length = msgrcv( MSQID, ( struct msgbuf* )&rcv_buf, MAXMSGLENGTH, MYADDR * modifier, 0 ) ) < 0 )
    {
        fprintf( stderr, "Can\'t receive message from queue\n" );
        msgctl( MSQID, IPC_RMID, ( struct msqid_ds* )NULL );
        exit( -1 );
    }
    
    return msg_length;
}