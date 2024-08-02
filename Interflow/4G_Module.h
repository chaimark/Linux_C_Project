#ifndef _4G_Module_H
#define _4G_Module_H

#include <stdint.h>
#include <stdbool.h>
#include "RTC.h"
#include "AT24C0256.h"
#include "MQTT_JSON.h"
#include "Uart.h"
#include "Gpio.h"
#include "main.h"
// 超时处理
#define MQTT_4G_REBOOT_INV_MAX    360000      // 1h   最大重启间隔
#define MQTT_4G_READ_ISSI_INV     360000      // 1h   读取信号强度间隔 
#define MQTT_4G_READ_CCLK_INV     12*360000   // 12h   读取信号强度间隔 
#define MQTT_4G_HEART_INV         360000      // 60s  心跳间隔 1h
// 流程处理
#define MQTT_4G_PROCESS_MAX  sizeof(MQTT_4G_Pro) / sizeof(MQTT_4G_PROCESS_T) // 总条数
#define MQTT_4G_CONNECT_INDEX     17     // 连接成功
#define MQTT_4G_READ_RSSI_INDEX   18     // 读信号强度 索引位置 
#define MQTT_4G_READ_CCLK_INDEX   9      // 读系统时钟 索引位置 
#define MQTT_4G_Send_INDEX        19     // 发送数据等待索引位置
#define OFFSET_MAX_SIZE  500

extern char EC200T_VER[30];
extern char EC20T_CCID[30];
extern char EC20T_IMEI[30];
extern char EC20T_CCLK[22];
extern char EC20T_CSQ[5];
extern int EC20T_RSSI;
extern int EC20T_SINR;
extern unsigned char MQTT_4G_History_Confirm;
extern unsigned char MQTT_4G_Connect_Flage;
extern char MQTT_4G_SendBuf[UART0_MAX];
static char * JSON_TTL_Buff = MQTT_4G_SendBuf;

extern void MQTT_4G_Init(void);
extern void MQTT_4G_Task(void);

extern void MQTT_4G_10mS_Timers_Add(void);
extern void MQTT_4G_Send_Protocol_To_TTL(char * txBuffer, unsigned short int length);
#endif
