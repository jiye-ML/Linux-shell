#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int getFileNum(char* root)
{
    DIR* dir = NULL;
    dir = opendir(root);
    if(dir == NULL)
    {
        perror("opendir");
        exit(1);
    }

    int total = 0;
    char path[1024] = {0};
    struct dirent* ptr = NULL;
    while((ptr = readdir(dir)) != NULL)
    {
       if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
       {
           continue;
       }
       if(ptr->d_type == DT_DIR)
       {
           sprintf(path, "%s/%s", root, ptr->d_name);
           total += getFileNum(path);
       }
       if(ptr->d_type == DT_REG)
       {
           total++;
       }
    }
    closedir(dir);
    return total;
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("a.out dir");
        exit(1);
    }

    int total = getFileNum(argv[1]);
    printf("%s has file number: %d\n", argv[1], total);

    return 0;
}

