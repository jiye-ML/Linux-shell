#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char* argv[])
{
    int mode = strtol(argv[2], NULL, 8);
    int ret = chmod(argv[1], mode);
    if(ret == -1)
    {
        perror("chmod");
        exit(1);
    }

    ret = chown(argv[1], 116, 125);
    if(ret == -1)
    {
        perror("chown");
        exit(1);
    }
    return 0;
}
