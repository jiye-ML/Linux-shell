#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("a.out softlink\n");
        exit(1);
    }

    char buf[512];
    int ret = readlink(argv[1], buf, sizeof(buf));
    if(ret == -1)
    {
        perror("readlink");
        exit(1);
    }
    buf[ret] = 0;
    printf("buf = %s\n", buf);

    return 0;
}
