
// #include "MQTT_JSON.h"
#include "UpgradeProgram.h"
// #include <stdbool.h>
// #include "StrLib.h"
// #include "NumberBaseLic.h"
// #include "main.h"

// UpParam_Infomation Up_Param_Infomation;

// static int Falsh_Check_Page(unsigned int addr, unsigned int * buf) {
//     for (int i = 0; i < PAGE_SIZE >> 2; i++) {
//         if (buf[i] != *(unsigned int *)(addr + (i << 2))) {
//             return -1;
//         }
//     }
//     return 0;
// }

// static int Falsh_Write_Page(unsigned int addr, unsigned int * buf) {
//     if (FL_FLASH_PageErase(FLASH, addr) == FL_FAIL)
//         return -1;
//     if (FL_FLASH_Program_Page(FLASH, addr >> PAGE_OFFSET, buf) == FL_FAIL)
//         return -1;
//     // 校验
//     if (Falsh_Check_Page(addr, buf) < 0)
//         return -1;
//     return 0;
// }

// // 八位和校验
// static unsigned char UpData_CheckSum(unsigned char * buf, int len) {
//     unsigned char sum = 0;
//     for (int i = 0; i < len; i++)
//         sum += buf[i];
//     sum = 0x100 - sum;
//     return sum;
// }

// // 写标志扇区 Flag == 0 清空标志扇区, Flag == 1 写入标志扇区
// void updata_write_sign(unsigned char Flag) {
//     unsigned char Temp[PAGE_SIZE] = {0}; // 每页 512 字节
//     if (Flag == 0) {
//         memset(&Up_Param_Infomation.Param_Over_Sign_Space, 0, sizeof(UpParam_Over));
//     } else {
//         memcpy(Temp, (int *)0xFF, ARR_SIZE(Temp));
//         Up_Param_Infomation.Param_Over_Sign_Space.sign = UPDATA_SIGN_DATA;
//         Up_Param_Infomation.Param_Over_Sign_Space.PageNum = 0;
//     }
//     memcpy(Temp, &Up_Param_Infomation.Param_Over_Sign_Space, sizeof(UpParam_Over));
//     Falsh_Write_Page(UPGRADE_PAGE_SIGN_ADDR, (unsigned int *)Temp);
// }

// // 程序升级任务
// int UpParamTask(char NodeStr[], unsigned short int len) {
//     char DataLine[UpData_Line_MAX] = {0};
//     unsigned short int DataLinelen = 0;
//     int DataLineCsNum = 0;
//     int RowNum = 0;

//     NodeStr = JSON_Find_Int(NodeStr, &json_matching.row, &RowNum);          // 查找 row，获取行数
//     NodeStr = JSON_Find_Node(NodeStr, &json_matching.fw, &DataLinelen) + 1;     // 查找 fw 然后跳过冒号

//     char * P_end = NULL;
//     for (int i = 0; i < RowNum; i++) {
//         P_end = strchr(NodeStr, ':');
//         (*P_end) = '\0';
//         if ((strlen(NodeStr) / 2) > ARR_SIZE(DataLine)) {
//             return -1;  // 数据长度超过限制
//         }
//         DataLinelen = ASCIIToHEX2(NodeStr, strlen(NodeStr), DataLine, ARR_SIZE(DataLine));// 转为hex
//         DataLineCsNum = UpData_CheckSum((unsigned char *)NodeStr, len);// 检查CS校验
//         if (DataLineCsNum != DataLine[DataLinelen - 1]) {
//             return -1;  // CHECKSUM-8 LRC 校验错误 
//         }

//         // 行结构正确，判断数据类型
//         switch (DataLine[3]) {
//             case 0x00://数据记录 :10 4000 00 38570020B9410000C1410000C3410000 01
//                 if (((((unsigned short int)DataLine[1]) << 8) | DataLine[2]) > Up_Param_Infomation.UpProgram_NowAddr) {
//                     printf("updata addr err\r\n");
//                     return -2;
//                 } else if (((((unsigned short int)DataLine[1]) << 8) | DataLine[2]) < Up_Param_Infomation.UpProgram_NowAddr){
//                     break;
//                 } else {

//                 }
//                 break;
//             case 0x01://文件结束记录 :00 0000 01 FF
//                 break;
//             case 0x02://扩展段地址记录 :02 0000 02 12FF BD
//                 break;
//             case 0x03://开始段地址记录 :02 0000 03 0123 BC
//                 break;
//             case 0x04://扩展线性地址记录 :02 0000 04 0000 FA
//                 break;
//             case 0x05://开始线性地址记录 :04 0000 05 000040C1 F6
//                 break;
//         }

//         // 写入数据缓存
//         memcpy(&Up_Param_Infomation.DataPage[Up_Param_Infomation.DataPage_NowLen], &DataLine[4], DataLinelen - 5);

//         // 满一页写入flash
//         // 保存每页的校验和
//         NodeStr = P_end + 1;    // 准备读取下一行数据
//     }

//     return 0;
// }

