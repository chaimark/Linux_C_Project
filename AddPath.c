#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _CmdData {
    const char * folderPath;
    int folderPathAddi;
    char bashrcPath[512];
}CmdData;

void add_to_path(CmdData UserCmdData) {

    // 检查文件夹路径是否为空
    if (UserCmdData.folderPath == NULL || UserCmdData.folderPath[0] == '\0') {
        printf("文件夹路径不能为空。\n");
        return;
    }

    // 打开bashrc文件
    FILE * bashrc_file = fopen(UserCmdData.bashrcPath, "a");
    if (bashrc_file == NULL) {
        printf("无法打开bashrc文件。\n");
        return;
    }
    if (UserCmdData.folderPathAddi == 1) {    // del now path
        fprintf(bashrc_file, "export PATH=$PATH:%s\n", UserCmdData.folderPath); // 将路径添加到bashrc文件中
        fclose(bashrc_file);    // 关闭bashrc文件
    } else {

    }

    setenv("PATH", UserCmdData.folderPath, 1);  // 更新当前进程的PATH环境变量
    printf("文件夹路径 %s 已成功添加到PATH中\n并更新了bashrc文件。\n", UserCmdData.folderPath);
    char cmd[512] = {0};
    strncpy(cmd, "source ", sizeof(cmd) - 1);
    strncat(cmd, UserCmdData.bashrcPath, sizeof(cmd) - strlen(cmd) - 1);
    system(cmd);
}

int main(int argc, char * argv[]){
    if (argc < 2) {
        return 1;
    }
    if (argc > 3) {
        printf("使用方法: %s <文件夹路径>\n", argv[0]);
        return 1;
    }
    CmdData UserCmdData;
    // 获取当前用户的家目录
    char * home_dir = getenv("HOME");
    if (home_dir == NULL) {
        printf("无法获取家目录。\n");
        return 1;
    }
    snprintf(UserCmdData.bashrcPath, sizeof(UserCmdData.bashrcPath), "%s/.bashrc", home_dir);
    if ((strstr(argv[1], "-show") != NULL)) {
        char cmd[512] = {0};
        strncpy(cmd, "source ", sizeof(cmd) - 1);
        strncat(cmd, UserCmdData.bashrcPath, sizeof(cmd) - strlen(cmd) - 1);
        system(cmd);
        return 1;
    }
    UserCmdData.folderPathAddi = ((strstr(argv[1], "-d") != NULL) ? 2 : 1);
    UserCmdData.folderPath = argv[UserCmdData.folderPathAddi];
    add_to_path(UserCmdData);
    return 0;
}
