#include "UpData.h"
#include <stdbool.h>
#include "StrLib.h"
#include "NumberBaseLib.h"

typedef struct UpData_Param_S {
    unsigned int sign;		// 标志位
    unsigned int pageNum;	// 大小
    unsigned char sum[128]; // 校验
} UpData_Param_T;

UpData_Param_T updata_param;

static unsigned char UpData_Line[UpData_Line_MAX];
static unsigned int UpData_Page32[PAGE_SIZE >> 2]; // 整页存储
static unsigned char * updata_page = (unsigned char *)UpData_Page32;
static unsigned short int UpData_Page_Len;
static unsigned int UpData_Page_Num;
static unsigned char UpData_Buff[PAGE_SIZE]; // 临时存储
static unsigned short int UpData_Bufff_Len;
static unsigned int UpData_AddrBase = 0xaa;		 // 基地址
static unsigned short int UpData_AddrNow = 0xaa; // 当前地址

static int Falsh_Check_Page(unsigned int addr, unsigned int * buf) {
    for (int i = 0; i < PAGE_SIZE >> 2; i++) {
        if (buf[i] != *(unsigned int *)(addr + (i << 2))) {
            return -1;
        }
    }
    return 0;
}

static int Falsh_Write_Page(unsigned int addr, unsigned int * buf) {
    if (FL_FLASH_PageErase(FLASH, addr) == FL_FAIL)
        return -1;
    if (FL_FLASH_Program_Page(FLASH, addr >> PAGE_OFFSET, buf) == FL_FAIL)
        return -1;
    // 校验
    if (Falsh_Check_Page(addr, buf) < 0)
        return -1;
    return 0;
}

// 八位和校验
static unsigned char UpData_CheckSum(unsigned char * buf, int len) {
    unsigned char sum = 0;
    for (int i = 0; i < len; i++)
        sum += buf[i];
    sum = 0x100 - sum;
    return sum;
}

// 写标志扇区
static unsigned int updata_page_buf[PAGE_SIZE >> 2];
static void updata_write_sign(void) {
    memset(updata_page_buf, 0xff, PAGE_SIZE);
    memcpy(updata_page_buf, &updata_param, sizeof(UpData_Param_T));
    Falsh_Write_Page(UPDATA_PAGE_SIGN, updata_page_buf);
}

// 清除标志扇区
static void updata_clean_sign(void) {
    memset(&updata_param, 0, sizeof(updata_param));
    updata_write_sign();
    UpData_Page_Num = UPDATA_PAGE_BEGIN;
    UpData_Page_Len = 0;
    UpData_Bufff_Len = 0;
    UpData_AddrBase = UPDATA_MCU_BASE;
    UpData_AddrNow = UPDATA_MCU_OFFSET;
}

int UpData_Receive_Hex(unsigned short int num_row, unsigned char * buf, unsigned short int len) {
    char * p_begin;
    char * p_end;
    char * p = (char *)buf;
    buf[len] = 0;
    int lineLen = 0;
    unsigned short int addr = NULL;
    unsigned char isFinished = 0;
    unsigned short int line_num = 0;
    while (line_num < num_row) {
        // 开头
        p = strchr(p, ':');
        if (p == NULL) {
            printf("updata line1 err\r\n");
            return -1;
        }
        p_begin = p + 1;

        // 结尾
        p = strchr(p, '\r');
        if (p == NULL) {
            printf("updata line2 err\r\n");
            return -1;
        }
        p_end = p;

        // 长度合理
        lineLen = p_end - p_begin;
        if ((lineLen > (UpData_Line_MAX * 2)) || ((lineLen & 0x01) != 0)) {
            printf("updata len err\r\n");
            return -1;
        }

        if ((lineLen = ASCIIToHEX2((char *)p_begin, lineLen, (char *)UpData_Line, ARR_SIZE(UpData_Line))) < 0) {
            printf("updata data err\r\n");
            return -1;
        }

        // 长度
        if (lineLen - 5 != UpData_Line[0]) {
            printf("updata Len err\r\n");
            return -1;
        }

        // 校验
        if (UpData_CheckSum(UpData_Line, lineLen - 1) != UpData_Line[lineLen - 1]) {
            printf("updata checksum err\r\n");
            return -1;
        }

        // 一行没问题
        switch (UpData_Line[3]) {
            case 0: // 数据
                addr = (((unsigned short int)UpData_Line[1]) << 8) | UpData_Line[2];
                if (addr > UpData_AddrNow) {
                    printf("updata addr err\r\n");
                    return -2;
                } else if (addr < UpData_AddrNow){
                    break;
                } else {
                    memcpy(UpData_Buff + UpData_Bufff_Len, UpData_Line + 4, UpData_Line[0]);
                    UpData_Bufff_Len += UpData_Line[0];
                    UpData_AddrNow += UpData_Line[0];
                }
                break;
            case 1: // 结束
                isFinished = 1;
                break;
            case 4: // 地址
                addr = (((unsigned short int)UpData_Line[4]) << 8) | UpData_Line[5];
                if (addr == UPDATA_MCU_BASE)
                    updata_clean_sign();
                else {
                    UpData_AddrBase++;
                    if (UpData_AddrNow != 0 || addr != UpData_AddrBase) {
                        printf("updata baseaddr err\r\n");
                        return -1;
                    }
                }
                break;
            default:
                break;
        }
        if (isFinished == 1)
            break;
        else
            line_num++;
    }

    // 处理本次数据
    int remain = PAGE_SIZE - UpData_Page_Len;
    if (remain >= UpData_Bufff_Len) {
        memcpy(updata_page + UpData_Page_Len, UpData_Buff, UpData_Bufff_Len);
        UpData_Page_Len += UpData_Bufff_Len;
        UpData_Bufff_Len = 0;
    } else {
        memcpy(updata_page + UpData_Page_Len, UpData_Buff, remain);
        UpData_Page_Len += remain;
        UpData_Bufff_Len -= remain;
        memcpy(UpData_Buff, UpData_Buff + remain, UpData_Bufff_Len);
    }

    if (UpData_Page_Len >= PAGE_SIZE || isFinished == 1) {
        int page = (UpData_Page_Num - UPDATA_PAGE_BEGIN) >> PAGE_OFFSET;
        // 补齐
        for (int i = UpData_Page_Len; i < PAGE_SIZE; i++)
            updata_page[i] = 0xff;
        // 校验
        updata_param.sum[page] = 0;
        for (int i = 0; i < PAGE_SIZE; i++)
            updata_param.sum[page] += updata_page[i];
        // 写入
        Falsh_Write_Page(UpData_Page_Num, (unsigned int *)updata_page);
        UpData_Page_Len = 0;
        UpData_Page_Num += PAGE_SIZE;
        if (isFinished == 1) {
            updata_param.pageNum = page + 1;
            updata_param.sign = UPDATA_SIGN;
            updata_write_sign();
            return 0x55;
        }
    }
    return 0;
}
