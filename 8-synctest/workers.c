#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

const key_t SEM_KEY = 1021;
const key_t SHM_KEY = 2021;

const int GAEK_PER_DETAIL = 2;
const int VINT_PER_DETAIL = 3;
const int N_DETAILS = 5;

void take_component();
void take_instrument();
void return_component();
void return_instrument();
void work_with_detail();
void detail_ready();

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
    
    pid_t parent = 0;
        
    for( int i = 0; i < workers; i++ )
    {
        parent = fork();
        if( !parent )
        {
            break;
        }
    }
    
    struct sembuf sb[1] = {0};
    sb[0].sem_num = 0;
    //sb[0].sem_flg = SEM_UNDO; //Позволить 3-му участнику разблокировать семафор
    sb[0].sem_op = 1;
    semop( sem_id, sb, 1 ); //инициализация
    
    
    
    if( parent )
    {
        printf( "[%d] wathdog process\n", getpid );
        
        //fprintf( stderr, "[%d] lock attempt %d\n", getpid, __LINE__ );
        sb[0].sem_op = -1;
        semop( sem_id, sb, 1 ); //Блокировка семафора
        //fprintf( stderr, "[%d] locked\n", getpid );
        
        int* wd_shm_buf = ( int* )shmat( shm_id, NULL, 0 ); // 0 - число гаек, 1 - число винтов, 2 - число собранных деталей
        wd_shm_buf[0] = 0;
        wd_shm_buf[1] = 0;
        wd_shm_buf[2] = 0;
        
        sb[0].sem_op = 1;
        semop( sem_id, sb, 1 ); //Разблокировка семафора
        
        printf( "[%d] Initialized memory, started watching\n", getpid );
        
        while( 1 )
        {
            //fprintf( stderr, "[%d] lock attempt %d\n", getpid, __LINE__ );
            sb[0].sem_op = -1;
            semop( sem_id, sb, 1 ); //Блокировка семафора
            //fprintf( stderr, "[%d] locked\n", getpid );
            
            if( wd_shm_buf[2] >= N_DETAILS )
            {
                break;
            }
            
            sb[0].sem_op = 1;
            semop( sem_id, sb, 1 ); //Разблокировка семафора
            
            sleep( 0.1 );
        }
        
        printf( "Finishing...\n" );

        sb[0].sem_op = 1;
        semop( sem_id, sb, 1 ); //Разблокировка семафора
            
        semctl( sem_id, 1, IPC_RMID, sem_arg ); //Удаляем семафоры
        shmdt( wd_shm_buf ); //Отсоединяем разделяемую память
        shmctl( shm_id, IPC_RMID, NULL ); //Освобождаем разделяемую память
        
        return 0;
    }
    

    sleep( 1 );   
    printf( "[%d] started worker process\n", getpid() );
    int* shm_buf = NULL; // 0 - число гаек, 1 - число винтов, 2 - число собранных деталей
    
    while( 1 )
    {
        take_component();
        take_instrument();
        
        //fprintf( stderr, "[%d] lock attempt %d\n", getpid, __LINE__ );
        sb[0].sem_op = -1;
        semop( sem_id, sb, 1 ); //Блокировка семафора
        //fprintf( stderr, "[%d] locked\n", getpid );
        
        shm_buf = ( int* )shmat( shm_id, NULL, 0 ); //Присоединение памяти
        
        printf( "[%d] shm_buf[2] = %d\n", getpid(), shm_buf[2] );
        if( shm_buf[2] >= N_DETAILS )
        {
            sb[0].sem_op = 1;
            semop( sem_id, sb, 1 ); //Разблокировка семафора
            
            shmdt( shm_buf ); //Отсоединение памяти
            
            return 0;
        }
        
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
            
            sb[0].sem_op = 1;
            semop( sem_id, sb, 1 ); //Разблокировка семафора
            
            return_component();
            return_instrument();
            
            continue;
        }
        
        shmdt( shm_buf ); //Отсоединение памяти
        
        sb[0].sem_op = 1;
        semop( sem_id, sb, 1 ); //Разблокировка семафора
        
        work_with_detail();
        return_instrument();
        
        //fprintf( stderr, "[%d] lock attempt %d\n", getpid, __LINE__ );
        sb[0].sem_op = -1;
        semop( sem_id, sb, 1 ); //Блокировка семафора
        //fprintf( stderr, "[%d] locked\n", getpid );
        
        shm_buf = ( int* )shmat( shm_id, NULL, 0 ); //Присоединение памяти
        
        if( shm_buf[0] == GAEK_PER_DETAIL && shm_buf[1] == VINT_PER_DETAIL )
        {
            detail_ready();
            
            shm_buf[0] = 0;
            shm_buf[1] = 0;
            
            shm_buf[2]++;
        }
        
        shmdt( shm_buf ); //Отсоединение памяти
        
        sb[0].sem_op = 1;
        semop( sem_id, sb, 1 ); //Разблокировка семафора
        
    }
    
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
    fprintf( stderr, "[%d] return_intrument\n", getpid() );
}

void work_with_detail()
{
    fprintf( stderr, "[%d] work_with_detail started\n", getpid() );
    sleep( 2 );
    fprintf( stderr, "[%d] work_with_detail ended\n", getpid() );
}

void detail_ready()
{
    fprintf( stderr, "[%d] detail_ready, moving new one\n", getpid() );
    sleep( 0.1 );
}


