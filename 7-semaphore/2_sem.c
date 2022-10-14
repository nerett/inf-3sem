#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
// Написать комментарии, отладить работу
int main( int argc, char *argv[], char *envp[] )
{
    char pathname[] = "1_sem.c"; //взаимодейстиве с 1-й программой по имени файла
    struct sembuf mybuf = {0};
    key_t key = ftok( pathname, 0 );

    int semid = 0;
    if( ( semid = semget( key, 1, 0666 | IPC_CREAT ) ) < 0 )
    {
        printf( "Can\'t create semaphore set\n" );
        exit( -1 );
    }

    mybuf.sem_num = 0;
    mybuf.sem_op  = 1;
    mybuf.sem_flg = 0;

    if( semop( semid, &mybuf, 1 ) < 0 )
    {
        printf( "Can\'t add 1 to semaphore\n" );
        exit( -1 );
    }

    printf( "The condition is present\n" );
    return 0;
}
