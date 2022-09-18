/* Проверить совместную работу с 4.
   Написать комментарии, В ТОМ ЧИСЛЕ К ПАРАМЕТРАМ!*/
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>

#define SHMEM_SIZE	4096
#define SH_MESSAGE	"Poglad Kota!\n"

int main (void)
{
    /* IPC дескриптор для области разделяемой памяти */
	int shm_id;
    /* Указатель на разделяемую память */
	char* shm_buf;
	int shm_size;
	struct shmid_ds ds;
	
    /* Пытаемся найти разделяемую память по сгенерированному ключу */
	shm_id = shmget( IPC_PRIVATE, SHMEM_SIZE, IPC_CREAT | IPC_EXCL | 0600 );
	if( shm_id == -1 ) 
	{
		fprintf( stderr, "shmget() error\n" );
		return 1;
	}

	/* Пытаемся отобразить разделяемую память в адресное пространство текущего процесса */
	shm_buf = ( char* )shmat( shm_id, NULL, 0 );
	if( shm_buf == ( char* )( -1 ) )
	{
		fprintf( stderr, "shmat() error\n" );
		return 1;
	}

	/* Проверяем доступный нам объём разделяемой памяти */
	shmctl( shm_id, IPC_STAT, &ds );
	
	shm_size = ds.shm_segsz;
	if( shm_size < strlen ( SH_MESSAGE ) ) 
	{
		fprintf ( stderr, "error: segsize=%d\n", shm_size );
		return 1;
	}
	
	/* Записываем сообщение в разделяемую память */
	strcpy( shm_buf, SH_MESSAGE );

	printf( "ID: %d\n", shm_id );
	printf( "Press <Enter> to exit..." );	
	fgetc( stdin );

    /* Отсоединяем разделяемую память и завершаем работу */ 
	shmdt( shm_buf );
    /* Удаляем разделяемую память из системы */
	shmctl( shm_id, IPC_RMID, NULL );

	return 0;
}
