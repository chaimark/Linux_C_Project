#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "AddPath.h"

void del_to_path(CmdData UserCmdData) {

    char FindPath[512];
    sprintf(FindPath, "export PATH=$PATH:%s", UserCmdData.folderPath); // 设置要排除的 folderPath 文件中

    system("cp /home/leige/Public/bashrc /tmp/bashrcTemp"); // 复制一份模板
    // bashrcMather 写入 tmp/bashrcTemp
    FILE * bashrcMatherFp = fopen("/tmp/bashrcTemp", "w");
    if (bashrcMatherFp == NULL) {
        printf("无法赋值 bashrcTemp 模板文件。\n");
        return;
    }
    fprintf(bashrcMatherFp, "%s", bashrcMather);
    fclose(bashrcMatherFp);// 关闭 /tmp/bashrcTemp 文件

    FILE * bashrcTemp = fopen("/tmp/bashrcTemp", "a");
    if (bashrcTemp == NULL) {
        printf("无法打开 bashrcTemp 文件。\n");
        return;
    }
    system("cat ~/.bashrc | grep ^export | awk '{print $0}' > /tmp/exportTxt"); // 读取 bashrc 文件的 export PATH 行
    FILE * exportTxt = fopen("/tmp/exportTxt", "r");   // 打开 /tmp/exportTxt 文件
    if (exportTxt == NULL) {
        printf("无法打开 exportTxt 文件。\n");
        return;
    }
    int c;
    char LineData[512] = {0};
    int LineDataLen = 0;
    while ((c = fgetc(exportTxt)) != EOF) {
        if (c == '\n') {
            LineData[LineDataLen++] = c;    // 复制字符到 LineData 数组中
            if (strstr(LineData, FindPath) != NULL) {   // 是需要排除的行，跳过该行
                LineDataLen = 0;
                memset(LineData, 0, sizeof(LineData));
                continue;
            }
            // 将 LineData 写入 /tmp/bashrc 文件
            fprintf(bashrcTemp, "%s", LineData); // 将路径添加到 bashrc 文件中
            // 清空 LineData 数组
            LineDataLen = 0;
            memset(LineData, 0, sizeof(LineData));
        } else {
            LineData[LineDataLen++] = c;    // 复制字符到 LineData 数组中
        }
    }
    fclose(bashrcTemp);// 关闭 /tmp/bashrc 文件
    system("cp /tmp/bashrcTemp ~/.bashrc");
    system("rm -f /tmp/bashrc");// 删除 /tmp/bashrc 文件

    printf("路径 %s 已成功从 bashrc 文件中删除。\n", UserCmdData.folderPath);
    printf("按任意键, 查看 bashrc");
    getchar();
    system("cat ~/.bashrc");
}

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
    fprintf(bashrc_file, "export PATH=$PATH:%s\n", UserCmdData.folderPath); // 将路径添加到bashrc文件中
    fclose(bashrc_file);    // 关闭bashrc文件
    system("source ~/.bashrc");
    printf("文件夹路径 %s 已成功添加到 PATH 中\n 并更新了bashrc文件。\n", UserCmdData.folderPath);
}

int main(int argc, char * argv[]) {
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
        strncpy(cmd, "cat ", sizeof(cmd) - 1);
        strncat(cmd, UserCmdData.bashrcPath, sizeof(cmd) - strlen(cmd) - 1);
        system(cmd);
        return 1;
    }
    UserCmdData.folderPathAddi = ((strstr(argv[1], "-del") != NULL) ? 2 : 1);
    UserCmdData.folderPath = argv[UserCmdData.folderPathAddi];

    if (UserCmdData.folderPathAddi == 1) {
        add_to_path(UserCmdData);   // add now path
    } else {
        del_to_path(UserCmdData);   // del now path
    }

    return 0;
}
