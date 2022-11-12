#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

const key_t SEM_KEY = 1021;
const key_t SHM_KEY = 2021;

const int GAEK_PER_DETAIL = 2;
const int VINT_PER_DETAIL = 3;
const int N_DETAILS = 5;

const double DETAIL_MOVE_TIME = 0.1;
const double COMPONENT_ATTACH_TIME = 2;

enum semops{ lock = -1, unlock = 1 };
typedef int semops;

void take_component();
void take_instrument();
void return_component();
void return_instrument();
void work_with_detail();
void detail_ready();

void semaphore_operations( semops operation, int sem_id, int line );

#define semaphore( operation ) do{ semaphore_operations( operation, sem_id, __LINE__ ); } while(0)

union semnum //Название объединения
{
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
} sem_arg; //Переменная типа semnum



int main( int argc, char** argv )
{
    int shm_id = shmget( SHM_KEY, 3 * sizeof( int ), IPC_CREAT | IPC_EXCL | 0600 );
    if( shm_id == -1 )
    {
        fprintf( stderr, "shmget() error\n" );
        return 1;
    }
    
    int sem_id = semget( SEM_KEY, 1, 0600 | IPC_CREAT | IPC_EXCL );
    if( sem_id == -1 )
    {
        fprintf( stderr, "semget() error\n" );
        return 1;
    }
    
    
    int workers = 2;
    if( argc > 1 )
    {
        sscanf( argv[1], "%d", &workers );
    }
    
    semaphore( unlock ); //Инициализация
    
    pid_t parent = 0;
    pid_t* child_pid = ( pid_t* )calloc( workers, sizeof( pid_t ) );
    for( int i = 0; i < workers; i++ )
    {
        parent = fork();
        if( !parent )
        {
            break;
        }
        
        child_pid[i] = parent;
    }
    
    int* shm_buf = NULL; // 0 - число гаек, 1 - число винтов, 2 - число собранных деталей
    
    if( parent ) //Логика диспетчера процессов
    {
        printf( "[%d] wathdog process\n", getpid );
        
        semaphore( lock ); //Блокировка семафора
        
        shm_buf = ( int* )shmat( shm_id, NULL, 0 ); // 0 - число гаек, 1 - число винтов, 2 - число собранных деталей
        shm_buf[0] = 0;
        shm_buf[1] = 0;
        shm_buf[2] = 0;
        
        semaphore( unlock ); //Разблокировка семафора
        
        printf( "[%d] Initialized memory, started watching\n", getpid );
        
        while( 1 )
        {
            semaphore( lock ); //Блокировка семафора
            if( shm_buf[2] >= N_DETAILS ) break;
            semaphore( unlock ); //Разблокировка семафора
            
            sleep( 0.1 );
        }
        
        printf( "Finishing...\n" );
        for( int i = 0; i < workers; i++ )
        {
            kill( child_pid[i], SIGHUP );
        }
        //semaphore( unlock ); //Разблокировка семафора
            
        semctl( sem_id, 1, IPC_RMID, sem_arg ); //Удаляем семафоры
        shmdt( shm_buf ); //Отсоединяем разделяемую память
        shmctl( shm_id, IPC_RMID, NULL ); //Освобождаем разделяемую память
        
        free( child_pid );
        return 0;
    }
    
    //Логика основных процессов
    sleep( 1 );   
    printf( "[%d] started worker process\n", getpid() );
    
    while( 1 )
    {
        take_component();
        take_instrument();
        
        semaphore( lock ); //Блокировка семафора
        shm_buf = ( int* )shmat( shm_id, NULL, 0 ); //Присоединение памяти
        
        fprintf( stderr, "[%d] shm_buf[2] = %d\n", getpid(), shm_buf[2] );
        
        if( shm_buf[0] < GAEK_PER_DETAIL )
        {
            shm_buf[0]++;
        }
        else if( shm_buf[1] < VINT_PER_DETAIL )
        {
            shm_buf[1]++;
        }
        else
        {
            shmdt( shm_buf ); //Отсоединение памяти
            semaphore( unlock ); //Разблокировка семафора
            
            return_component();
            return_instrument();
            
            continue;
        }
        
        shmdt( shm_buf ); //Отсоединение памяти
        semaphore( unlock ); //Разблокировка семафора
        
        work_with_detail();
        return_instrument();
        
        semaphore( lock ); //Блокировка семафора
        shm_buf = ( int* )shmat( shm_id, NULL, 0 ); //Присоединение памяти
        
        if( shm_buf[0] == GAEK_PER_DETAIL && shm_buf[1] == VINT_PER_DETAIL )
        {
            detail_ready();
            
            shm_buf[0] = 0;
            shm_buf[1] = 0;
            shm_buf[2]++;
        }
        
        shmdt( shm_buf ); //Отсоединение памяти
        semaphore( unlock ); //Разблокировка семафора
    }
    
    free( child_pid );
    return 0;
}



void take_component()
{
    fprintf( stderr, "[%d] take_component\n", getpid() );
}

void take_instrument()
{
    fprintf( stderr, "[%d] take_instrument\n", getpid() );
}

void return_component()
{
    fprintf( stderr, "[%d] return_component\n", getpid() );
}

void return_instrument()
{
    fprintf( stderr, "[%d] return_instrument\n", getpid() );
}

void work_with_detail()
{
    fprintf( stderr, "[%d] work_with_detail started\n", getpid() );
    sleep( COMPONENT_ATTACH_TIME );
    fprintf( stderr, "[%d] work_with_detail ended\n", getpid() );
}

void detail_ready()
{
    fprintf( stderr, "[%d] detail_ready, moving new one\n", getpid() );
    sleep( DETAIL_MOVE_TIME );
}

void semaphore_operations( semops operation, int sem_id, int line )
{
    struct sembuf sb[1] = {0};
    sb[0].sem_num = 0;
    //sb[0].sem_flg = SEM_UNDO; //Позволить 3-му участнику разблокировать семафор
    sb[0].sem_op = 1;
    
    #ifndef NDEBUG
        if( operation == lock ) fprintf( stderr, "[%d] lock attempt %d\n", getpid, line );
    #endif

    semop( sem_id, sb, operation );
    
    #ifndef NDEBUG
        if( operation == lock ) fprintf( stderr, "[%d] locked\n", getpid );
    #endif
}
