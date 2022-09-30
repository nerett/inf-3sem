#include "stdio.h"
#include "stdlib.h"
#include "string.h"

const int LINESIZE = 100;
const int NWORDS = 50;

int main()
{
    /*
    int n = 0;
    scanf( "%d", &n );
    char** wordsequence = ( char** )calloc( n, sizeof( char* ) );
    //char* buffer NULL;
    
    for( int i = 0; i < n; i++ )
    {
        scanf( "%ms", &wordsequence[i] );
    }

    printf( "Output:\n" );
    for( int i = n - 1; i >= 0; i-- )
    {
        if( i == ( n - 3 ) )
        {
            int length = strlen( wordsequence[i] );
            for( int letter_n = length; letter_n >=0; letter_n-- )
            {
                printf( "%c", wordsequence[i][letter_n] );
            }
            printf( " " );
            continue;
        }
        printf( "%s ", wordsequence[i] );
    }
    printf( "\n" );
    */
    
    
    int i = 0;
    int size = LINESIZE;
    char buffer = 'q';
    char* line = ( char* )calloc( size, sizeof( char ) );

    while( ( buffer != '\0' ) && ( buffer != '\n' ) && ( buffer != EOF ) )
    {
        if( i == size )
        {
            fprintf( stderr, "realloc\n" );
            size += 100;
            line = ( char* )realloc( line, size );
        }
        
        //scanf( "%c", &buffer ); //костыль
        buffer = getchar();
        //fprintf( stderr, "buffer: %c\n", buffer );
        //fprintf( stderr, "%p", line[i] );
        line[i] = buffer;
        i++;
    }
    line[--i] = '\0';
    
    
    int n_words = NWORDS;
    char** words = ( char** )calloc( n_words, sizeof( char** ) );
    words[0] = line;
    int k = 1;
    int j = 0;
    for( j = 0; j < i; j++ )
    {
        if( line[j] == ' ' )
        {
            line[j] = '\0';
            
            
            if( k == n_words )
            {
                n_words += 50;
                words = ( char** )realloc( words, n_words );
            }
            
            
            words[k] = &line[j+1];
            k++;
        }
    }
    
    
    printf( "output 1:\n" );
    for( int x = 0; x < k; x++ )
    {
        printf( "%s ", words[x] );
    }
    
    for( int i = 0; i < j; i++ )
    {
        printf( "%c", line[i] );
    }
    printf( "\n" );
    
    
    printf( "output 2:\n" );
    
    for( int i = k - 1; i >= 0; i-- )
    {
        if( i == ( k - 3 ) )
        {
            int length = strlen( words[i] );
            for( int letter_n = length; letter_n >=0; letter_n-- )
            {
                printf( "%c", words[i][letter_n] );
            }
            printf( " " );
            continue;
        }
        
        printf( "%s ", words[i] );
    }
    printf( "\n" );
    
    
    free( line );
    free( words );
    
    
    /*
    int n_words = NWORDS;
    char** words = ( char** )calloc( n_words, sizeof( char* ) );
    int k = 0;
    int n_read = 0;
    
    while( true )
    {
        
        if( k == n_words )
        {
            n_words += 10;
            words = ( char** )realloc( words, n_words );
        }
        
        
        n_read = scanf( "%ms", &words[k] );
        k++;
        
        if( n_read == 0 )
        {
            break;
        }
    }
    
    printf( "output:\n" );
    //printf( "%s", words[1] );
    
    
    for( int i = 0; i < k; k++ )
    {
        printf( "%s ", words[i] );
        free( words[i] );
    }
    free( words );
    */
    

    return 0;
}
