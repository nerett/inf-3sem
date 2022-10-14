#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHMEM_SIZE 4096
#define SH_MESSAGE "Hello World!\n"

#define SEM_KEY 2015
#define SHM_KEY 2015
// Написать комментарии, отладить работу

union semnum //Название объединения
{
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
} sem_arg; //Переменная типа semnum


int main( void )
{
    int shm_id = shmget( SHM_KEY, SHMEM_SIZE, IPC_CREAT | IPC_EXCL | 0600 ); //Получение id разделяемой памяти по ключу
    if( shm_id == -1 )
    {
        fprintf( stderr, "shmget() error\n" );
        return 1;
    }

    int sem_id = semget( SEM_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL ); //Получение id семафора по ключу
    if( sem_id == -1 )
    {
        fprintf( stderr, "semget() error\n" );
        return 1;
    }

    printf( "Semaphore: %d\n", sem_id );
    unsigned short sem_vals[1] = {0};
    sem_vals[0] = 1;
    sem_arg.array = sem_vals;

    fprintf( stderr, "1\n" );
    
    if( semctl( sem_id, 0, SETALL, sem_arg ) == -1 ) //Выставить все семафоры в 1
    {
        fprintf( stderr, "semctl() error\n" );
        return 1;
    }

    fprintf( stderr, "2\n" );
    
    char* shm_buf = ( char* )shmat( shm_id, NULL, 0 ); //Присоедяем разделяемую память
    if( shm_buf == ( char* ) -1 )
    {
        fprintf( stderr, "shmat() error\n" );
        return 1;
    }

    fprintf( stderr, "3\n" );

    struct shmid_ds ds = {0};
    shmctl( shm_id, IPC_STAT, &ds ); //Полуаем информацию об области разделяемой памяти
    int shm_size = ds.shm_segsz;
    if( shm_size < strlen( SH_MESSAGE ) )
    {
        fprintf( stderr, "error: segsize=%d\n", shm_size );
        return 1;
    }

    fprintf( stderr, "4\n" );
    
    struct sembuf sb[1] = {0};
    sb[0].sem_num = 0;
    sb[0].sem_flg = SEM_UNDO; //Позволить 3-му участнику разблокировать семафор
    sb[0].sem_op = -1;
    semop( sem_id, sb, 1 ); //Блокировка семафора
    
    strcpy( shm_buf, SH_MESSAGE ); //Запись сообщения в разделяемую память
    printf( "ID: %d\n", shm_id );
    
    sb[0].sem_op = 1;
    semop( sem_id, sb, 1 ); //Разблокировка семафора

    fprintf( stderr, "5\n" );
    
    sleep( 10 );
    
    semctl( sem_id, 1, IPC_RMID, sem_arg ); //Удаляем семафоры
    shmdt( shm_buf ); //Отсоединяем разделяемую память
    shmctl( shm_id, IPC_RMID, NULL ); //Освобождаем разделяемую память

    return 0;
}
