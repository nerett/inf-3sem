#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
// Написать комментарии, отладить работу
int main( int argc, char *argv[], char *envp[] )
{
    char pathname[] = "1_sem.c";
    struct sembuf mybuf = {0};

    key_t key = ftok( pathname, 0 ); //Генерируем IPC ключ из имени файла и номера экземпляра области разделяемой памяти 0

    int semid = 0;
    if( ( semid = semget( key, 1, 0666 | IPC_CREAT ) ) < 0 ) //Пытаемся создать семафор для сгенерированного ключа
    {
        printf( "Can\'t create semaphore set\n" );
        exit( -1 );
    }

    fprintf( stderr, "1\n" );
    //задаём параметры семафора
    mybuf.sem_num = 0; //номер
    mybuf.sem_op  = -1; //операция
    mybuf.sem_flg = 0; //флаг
    
    fprintf( stderr, "2\n" );

    if( semop( semid, &mybuf, 1 ) < 0 ) //выполнить операции, задаваемые структурой
    {
        printf( "Can\'t wait for condition\n" );
        exit( -1 );
    }

    printf( "The condition is present\n" );
    return 0;
}
