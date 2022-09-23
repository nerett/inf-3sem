/* добавить вторую функцию, сравнивающую количество букв,
   добавить вызов различных функций в зависимости от аргумента программы
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void check( char* a, char* b, int ( *cmp )( const char* , const char* ) );
int nsymbolcmp( const char* str1, const char* str2 );

int main( int argc, char** argv )
{
    char s1[80], s2[80];
    /* указатель на функцию */
    int ( *p )( const char*, const char* ) = strcmp;
    //int  * p()  - без скобок будет просто функция возвращающая целое

    if( argc > 1 && !strcmp( argv[1], "length" ) )
    {
        p = nsymbolcmp;
    }
    /* присваивает адрес функции strcmp указателю p */

    printf( "Введите две строки.\n" );
    gets( s1 );
    gets( s2 );

    check( s1, s2, p ); /* Передает адрес функции strcmp
                            посредством указателя p */

    return 0;
}

void check( char *a, char *b, int ( *cmp )( const char*, const char* ) )
{
    printf( "Проверка на совпадение.\n" );
    if( !( *cmp )( a, b ) )
    {
        printf( "Равны\n" );
    }
    else
    {
        printf( "Не равны\n" );
    }
}

int nsymbolcmp( const char* str1, const char* str2 )
{
    return strlen( str1 ) - strlen( str2 );
}

