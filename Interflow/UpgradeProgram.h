#ifndef _UPGRADE_PROGRAM_H
#define _UPGRADE_PROGRAM_H

// #include <string.h>
// #include <stdbool.h>
// #define PAGE_OFFSET 9
// #define PAGE_SIZE 512
// #define UpData_Line_MAX 32
// #define UPDATA_SIGN_DATA 0xaa55aa55

// // #define FM33LC026_FLASH_EN // 选择打开 FM33LC026 的 flash
// #ifdef FM33LC026_FLASH_EN
// /***********FM33LC026****************/
// #define UPGRADE_PAGE_BEGIN_ADDR 0x11800
// #define UPGRADE_PAGE_OVER_ADDR 0x1F000
// #define UPGRADE_PAGE_SIGN_ADDR 0x1F000
// #else
// /***********FM33LC046****************/
// #define UPGRADE_PAGE_BEGIN_ADDR 0x23000
// #define UPGRADE_PAGE_OVER_ADDR 0x3E000
// #define UPGRADE_PAGE_SIGN_ADDR 0x3E000
// #endif

// #define UPDATA_MCU_OFFSET 0x4000

// typedef struct _UpParam_Over {
//     unsigned int sign;               // 标志位
//     unsigned int PageNum;            // Page 数 共 256 页，其中一半用于保存升级代码 最大128页
//     unsigned char PageCheckSum[128]; // 每页校验和  共 128 页
// } UpParam_Over;
// typedef struct _UpParam_Infomation {
//     bool Start_Upgrade_Program_TaskFlag;    // 开始升级程序
//     unsigned char Program_ver[128];         // 当前升级版本
//     uint16_t UpProgram_NowAddr;             // 当前升级需要请求的起始地址 (也是升级包第一行的地址)

//     uint16_t DataPage[PAGE_SIZE];           // 当前升级已缓存的升级数据，满一页写入flash (32行*16=512)
//     uint16_t DataPage_NowLen;               // 当前升级数据已写入缓存的长度
//     UpParam_Over Param_Over_Sign_Space;     // 标志扇区，用于帮助 bootloader 识别是否需要升级程序
// } UpParam_Infomation;
// extern UpParam_Infomation Up_Param_Infomation;  // 程序升级信息

// extern void updata_write_sign(unsigned char Flag);
// extern int UpParamTask(char NodeStr[], unsigned short int len);


#endif
