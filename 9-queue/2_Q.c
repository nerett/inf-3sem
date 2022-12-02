//написать программу получающую в качестве параметров два номера. Первый номер это
//номер для получения сообщений, второй номер для передачи сообщений. Можно
//запустить несколько экземпляров такой программы и они все будут обмениваться
//сообщениями между собой по номерам.
// Всё это нужно реализовать через одну очередь.
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MESSAGE_LENGTH 255
#define LAST_MESSAGE 255

const char PATHNAME[] = "2_Q.c";
//const int MAX_MESSAGE_LENGTH = 50;

int main( int argc, char** argv )
{
    int snd_addr = 0, rcv_addr = 0;
    if( argc == 3 )
    {
        sscanf( argv[1], "%d", &rcv_addr );
        sscanf( argv[2], "%d", &snd_addr );
    }
    else
    {
        printf( "Inappropriate number of arguments!\n" );
        exit( -1 );
    }
        
    struct mymsgbuf
    {
        long mtype;
        char mtext[MESSAGE_LENGTH];
    } mybuf = { 0 };
    
    int len = 0, maxlen = 0;
    char message[MESSAGE_LENGTH] = { 0 };
    
    key_t key = ftok( PATHNAME, 0 );
    
    // Если очередь уже существует, то ничего не делаем
    int msqid = 0;
    if( ( msqid = msgget( key, 0666 | IPC_CREAT ) ) < 0 )
    {
        printf( "Can\'t get msqid\n" );
        exit( -1 );
    }

    
    if( fork() ) //родительский процесс; отправляет сообщения
    {
        printf( "[%d] Parent process started\n", getpid() );
        mybuf.mtype = snd_addr;

        while( 1 )
        {
            scanf( "%s", message );
            if( !strcmp( message, "quit" ) )
            {
                printf( "[%d] Deleting queue\n", getpid()  );
                msgctl( msqid, IPC_RMID, ( struct msqid_ds* )NULL );
                exit( 0 );
            }
            strcpy( mybuf.mtext, message );
            
            len = strlen( mybuf.mtext ) + 1; //+ sizeof( long ); /* sizeof long? */
            if( msgsnd( msqid, ( struct msgbuf* )&mybuf, len, 0 ) < 0 )
            {
                printf( "Can\'t send message to queue\n" );
                msgctl( msqid, IPC_RMID, ( struct msqid_ds* )NULL );
                exit( -1 );
            }
        }
    }
    else //дочерний процесс
    {
        printf( "[%d] Child process started\n", getpid() );
        mybuf.mtype = rcv_addr;
        
        while( 1 )
        {
            maxlen = MESSAGE_LENGTH;
            
            if( ( len = msgrcv( msqid, ( struct msgbuf* )&mybuf, maxlen, rcv_addr, 0 ) ) < 0 )
            {
                printf( "Can\'t receive message from queue\n" );
                exit( -1 );
            }
            
            printf( "[%d][msg for %ld] %s\n", getpid(), mybuf.mtype, mybuf.mtext );
        }
    }
        
    
  //один из процессов проверяет очередь на предмет наличия в ней сообщений,
  // адресованных данному терминалу и выводит их на экран

  // второй процесс ожидает ввода с клавиатуры и отправляет сообщения,
  // встретив символ перевода строки
}
