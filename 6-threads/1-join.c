#include <stdio.h>
#include <pthread.h>
//#include <stdlib.h>
#include <unistd.h>

//-lpthread или -pthread  для успешной компиляции добавтить одну из этих опций
//Добавить вызов any_func в основную нить выполнения, чтобы распечатать 2009

void* any_func( void* arg )
{
    int a = *( int* ) arg; //Записываем в a значение по указателю *arg, интерпретируя его как указатель на int
    a++;
    return ( void* ) a; //возвращаем int, интерпретируя его как указатель
}

int main( void )
{
    pthread_t thread;
    int parg = 2007, pdata = 0;

    fprintf( stderr, "parg1 = %d\n", parg );
    if( pthread_create( &thread, NULL, &any_func, &parg ) != 0 ) //пытаемся создать поток
    {
        fprintf( stderr, "Error\n" );
        return 1;
    }
    
    sleep( 1 );
    fprintf( stderr, "parg2 = %d\n", parg );

    pthread_join( thread, ( void* ) &pdata ); //ожидаем завершения потока и вычитываем возвращаемое значение
    
    fprintf( stderr, "parg3 = %d\n", parg ); //значение parg зануляется после вызова join
    fprintf( stderr, "%d\n", pdata );
    
    
    parg = pdata;
    pdata = ( long )any_func( &parg );
    printf( "RESULT = %d\n", pdata );
    
    return 0;
}
