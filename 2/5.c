/* Программа, иллюстрирующая использование системных вызовов open(), read() и close() для чтения информации из файла */

//#include <io.h>
//#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv, char** envp )
{
    int inputfile = 0, outfile = 0;
    size_t size = 0;
    char string[60];

    /* Попытаемся открыть файл с именем в первом параметре выззова только
    для операций чтения */

    if( ( inputfile = open( "5_test.txt", O_RDONLY ) ) < 0 )
    {
        /* Если файл открыть не удалось, печатаем об этом сообщение и прекращаем работу */
        printf( "Can\'t open file\n" );
        exit( -1 );
    }

    /* Читаем фаил пока не кончится и печатаем */
    size = read( inputfile, string, 60 );
    if( size > 0 )
    {
        printf( "%s\n", string ); /* Печатаем прочитанное*/
    }
    if( close( inputfile ) < 0 )
    {
        printf( "Can\'t close file\n" );
    }
    
    
    if( ( outfile = open( "5_test_out.txt", O_WRONLY ) ) < 0 )
    {
        /* Если файл открыть не удалось, печатаем об этом сообщение и прекращаем работу */
        printf( "Can\'t open file\n" );
        exit( -1 );
    }

    write( outfile, string, 60 );
    
    if( close( outfile ) < 0 )
    {
        printf( "Can\'t close file\n" );
    }
    /*  Записываем файл под новым именем */

    /* Закрываем файл */
    
    /*  Открываем файл в редакторе */

    return 0;
} 
