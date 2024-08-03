#ifndef __UPData_H
#define __UPData_H

#include "main.h"

#define PAGE_OFFSET 9
#define PAGE_SIZE 512
#define UpData_Line_MAX 32

// #define FM33LC026_FLASH_EN // 选择打开 FM33LC026 的 flash
#ifdef FM33LC026_FLASH_EN
/***********FM33LC026****************/
#define UPDATA_PAGE_SIGN 0x1F000
#define UPDATA_PAGE_BEGIN 0x11800
#define UPDATA_PAGE_END 0x1F000
#else
/***********FM33LC046****************/
#define UPDATA_PAGE_SIGN 0x3E000
#define UPDATA_PAGE_BEGIN 0x23000
#define UPDATA_PAGE_END 0x3E000
#endif

#define UPDATA_SIGN 0xaa55aa55
// 地址偏移
#define UPDATA_MCU_BASE 0
#define UPDATA_MCU_OFFSET 0x4000

extern int UpData_Receive_Hex(unsigned short int num_row, unsigned char * buf, unsigned short int len);

#endif
