#ifndef __AT24C0256_H
#define __AT24C0256_H

#include <stdint.h>
#include <stdbool.h>

typedef struct AT24CXX_MANAGER_S {
    // 32字节
    char sign;		   // 初始化标志
    char gw_id[6];	   // 设备地址
    char username[10]; // 登录
    char password[15];

    // 32*6 = 192 字节
    char heating_start[15];
    char heating_end[15];
    char NET_Local_IMEI[15];
    char NET_Local_IP[30];
    char NET_Local_MASK[30];
    char NET_Local_GATEWAY[30];
    char NET_4G_Remote_Url[42];
    int NET_4G_Remote_Port;

    short int main_interval;
    short int copy_interval;
    short int copy_statistics;
    unsigned int not_intimer_interval;
    unsigned char Save_Working_Mode;

    char MQTTCheckENableFlag; // MQTT 使能标志
    char TCPCheckENableFlag;  // TCP 查询使能标志
} AT24CXX_MANAGER_T;
extern AT24CXX_MANAGER_T AT24CXX_Manager;

extern unsigned char EEprom_MQTT_Parameter_Init(void);
extern unsigned char EEprom_Byte1Read(unsigned int addr);
extern unsigned int EEprom_Byte2Read(unsigned int addr);
extern unsigned long int EEprom_Byte4Read(unsigned long int addr);
extern unsigned char EEprom_Byte1Write(unsigned int addr, unsigned char data);
extern void EEprom_Byte2Write(unsigned int addr, unsigned int data);
extern void EEprom_Byte4Write(unsigned long int addr, unsigned long int data);
extern unsigned char EEprom_JSON_Write(unsigned char * data, unsigned char len);

extern void Read_MBUS_Buad_Local_From_AT256_To_ARM(void);
extern void Read_RS4851_Buad_Local_From_AT256_To_ARM(void);
extern void Read_RS4852_Buad_Local_From_AT256_To_ARM(void);
#endif
