#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("a.out filename 111\n");
        exit(1);
    }

    long int len = strtol(argv[2], NULL, 10); 
    int ret = truncate(argv[1], len);
    if(ret == -1)
    {
        perror("truncate");
        exit(1);
    }
    return 0;
}
