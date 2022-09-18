#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv, char** envp )
{
    printf( "argv:\n" );
    for( int i = 0; i < argc; i++ )
    {
        printf( "№%d: %s\n", i, argv[i] );
    }
    
    printf( "envp:\n" );
    int j = 0;
    while( envp[j] )
    {
        printf( "№%d: %s\n", j, envp[j] );
        j++;
    }
    return 0;
}
