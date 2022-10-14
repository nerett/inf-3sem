#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <time.h>
#include <math.h>
#include <assert.h>
/* Модифицировать программу, 
 * чтобы замерить среднее время завершения нити после сигнала на завершение.
 */

const int N_EXPERIMENTS = 100000; //увеличение этого параметра до какого-то момента уменьшает среднее время завершения потока (вероятно, это связано с временем, необходимым для повышения частоты ЦП); дополнительная нагрузка (например, запущенный бенчмарк) ещё уменьшает это время (вплоть до 8 мс и, возможно, ниже), хотя общее время теста увеличивается; протестировано в системе с governor = schedutil (как ondemand, но также использует в том числе данные CFS для управления частотой)

void* any_func( void* arg )
{
	while( 1 ) 
	{
		fprintf( stderr, "." );
		sleep( 10 ); //не влияет на результат
	}

	return NULL;
}

int main( void )
{
    long long time_sum = 0;
    
    void* result = NULL;
    clock_t start_time = 0, time_interval = 0;
    pthread_t thread = 0;
    double current_avg = 0, sqrdiff_sum = 0;
    
    printf( "Calculating...\n" );
    for( int experiment = 1; experiment <= N_EXPERIMENTS; experiment++ )
    {
        thread = 0;
        result = NULL;
        
        if( pthread_create( &thread, NULL, &any_func, NULL ) != 0 ) 
        {
            fprintf( stderr, "Error\n" );
            return 1;
        }
        
        start_time = clock();
        
        pthread_cancel( thread );
        if( !pthread_equal( pthread_self(), thread ) )
        {
            pthread_join( thread, &result );
        }
        
        time_interval = clock() - start_time;
        time_sum += time_interval; //вычисляет именно ПРОЦЕССОРНОЕ время (sleep его не тратит, т.е. на эксперимент не влияет), чего в данной задаче достаточно
        current_avg = time_sum / experiment;
        sqrdiff_sum += ( current_avg - time_interval ) * ( current_avg - time_interval );
        
        #ifndef NDEBUG
            fprintf( stderr, "time_interval = %d, time_sum = %d, current_avg = %lf, sqrdiff_sum = %lf\n", time_interval, time_sum, current_avg, sqrdiff_sum );
        
            if( result == PTHREAD_CANCELED )
            {
                fprintf( stderr, "Canceled\n" );
            }
        #endif
    }

    double sigma = sqrt( sqrdiff_sum / N_EXPERIMENTS );
    printf( "AVERAGE of %d experiments = (%lf +- %lf) ms\n", N_EXPERIMENTS, current_avg, sigma );

	return 0;
}
