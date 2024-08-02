#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "main.h"
#include "StrLib.h"

extern FL_RTC_InitTypeDef RTC_Data;
extern unsigned int execute_Adjust_Meter_1Second_Count;
extern unsigned char MBUS_UART1_Reboot_In_Main_Flage;
extern unsigned char Set_RX8025T_By_MQTT_CCLK_In_Main_Flage;
extern unsigned char Adjust_Meter_In_Main_Flage;
extern unsigned int execute_Set_RX8025T_By_MQTT_CCLK_1Second_Count;
extern unsigned int execute_MBUS_Reboot_1Min_Count;
// extern unsigned char RTC_Heart_LED_Count;
// extern void Set_Heart_LED_ON();

extern void EC20_ReBoot_In_RTC_By_1Second(char sec);
extern void RTC_Init_By_RX8025T(void);
extern void setRtcDate(strnew SteStrRTCData);
extern void getStrNowDataAndTimeByRTC(char NowStrRTCData[]);
#endif
