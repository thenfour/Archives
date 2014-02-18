

#include <windows.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
    int i = 0;
    printf("Command Line: %s\n\n", GetCommandLine());

    for(i=0;i<argc;i++)
    {
        printf("arg %04d: [%s]\n", i, argv[i]);
    }

    return 0;
}
