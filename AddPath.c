#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void add_to_path(const char * folder_path) {
    // 检查文件夹路径是否为空
    if (folder_path == NULL || folder_path[0] == '\0') {
        printf("文件夹路径不能为空。\n");
        return;
    }

    // 获取当前用户的家目录
    char * home_dir = getenv("HOME");
    if (home_dir == NULL) {
        printf("无法获取家目录。\n");
        return;
    }

    // 构建bashrc文件路径
    char bashrc_path[512];
    snprintf(bashrc_path, sizeof(bashrc_path), "%s/.bashrc", home_dir);

    // 打开bashrc文件
    FILE * bashrc_file = fopen(bashrc_path, "a");
    if (bashrc_file == NULL) {
        printf("无法打开bashrc文件。\n");
        return;
    }

    // 将路径添加到bashrc文件中
    fprintf(bashrc_file, "export PATH=$PATH:%s\n", folder_path);

    // 关闭bashrc文件
    fclose(bashrc_file);

    // 更新当前进程的PATH环境变量
    setenv("PATH", folder_path, 1);

    printf("文件夹路径 %s 已成功添加到PATH中\n并更新了bashrc文件。\n", folder_path);
    char cmd[512] = {0};
    snprintf(cmd, sizeof(cmd), "source %s/.bashrc", home_dir);
    system(cmd);
}

int main(int argc, char * argv[]) {
    if (argc > 2) {
        printf("使用方法: %s <文件夹路径>\n", argv[0]);
        return 1;
    }

    const char * folder_path = argv[1];
    add_to_path(folder_path);
    return 0;
}
