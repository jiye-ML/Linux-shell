#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("a.out filename\n");
        exit(1);
    }
    int ret = unlink(argv[1]);
    if(ret == -1)
    {
        perror("unlink");
        exit(1);
    }

    return 0;
}
