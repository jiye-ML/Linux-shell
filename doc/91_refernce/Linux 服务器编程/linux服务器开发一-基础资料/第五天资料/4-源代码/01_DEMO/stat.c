#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("a.out filename\n");
        exit(1);
    }

    struct stat st;
    int ret = lstat(argv[1], &st);

    if(ret == -1)
    {
        perror("stat");
        exit(1);
    }
    printf("file size = %d\n", (int)st.st_size);

    return 0;
}
