#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv, char** envp )
{
    pid_t pid = 0, ppid = 0, chpid = 0;
    int a = 0, result = 0;
    
    chpid = fork();
    printf( ":after fork\n" );
    
    if( chpid < 0 )
    { 
        /* Ошибка */
        printf( "Can\'t fork a child process\n" );
    
    }
    else if( chpid > 0 )
    {
        printf( ":child process\n" );
        /* Родительский процесс */
        a = a + 1;
        
        pid = getpid();
        ppid = getppid();

        printf( "pid = %d, ppid = %d, result = %d\n", (int)pid, (int)ppid, a ); 
        
    }
    else 
    {
        printf( ":parent process\n" );
        /* Порожденный процесс */
        
        /* Мы будем запускать команду cat c аргументом командной строки без изменения параметров среды,
        т.е. фактически выполнять команду "cat имя файла", которая должна выдать содержимое данного файла на экран.
        Для функции execle в качестве имени программы мы указываем ее полное имя с путем от корневой директории.
        Первое слово в командной строке у нас должно совпадать с именем запускаемой программы. Второе слово в командной строке - это имя файла, содержимое которого мы хотим распечатать.
        */

        printf( ":before execle\n" );
        //result = execle( "/bin/cat", "/bin/cat", "4_test.txt", 0, envp );
        result = execle( "/usr/bin/cat", "/usr/bin/cat", "4_test.txt", NULL, envp );
        
        printf( ":after execle\n" );

        if( result < 0 )
        {
            /* Если возникла ошибка, то попадаем сюда*/
            printf( "Error on program start\n" );
            exit( -1 );
        }
    
    }
    return 0;
}
