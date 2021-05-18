#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int getFileNum(char* root)
{
    // 打开目录
    DIR* dir = opendir(root);
    if(dir == NULL)
    {
        perror("opendir");
        exit(0);
    }

    // 读目录
    int total = 0;
    char path[1024] = {0};
    struct dirent* ptr = NULL;
    while((ptr = readdir(dir)) != NULL)
    {
        // 跳过 . 和 ..
        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }
        // 判断是不是文件
        if(ptr->d_type == DT_REG)
        {
            total ++;
        }
        // 如果是目录
        if(ptr->d_type == DT_DIR)
        {
            // 递归读目录
            sprintf(path, "%s/%s", root, ptr->d_name);
            total += getFileNum(path);
        }
    }
    closedir(dir);
    return total;
}

int main(int argc, char* argv[])
{
    // 读目录， 统计文件个数
    int total = getFileNum(argv[1]);
    // 打印
    printf("%s has file number: %d\n", argv[1], total);
    return 0;
}
