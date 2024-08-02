
#ifndef TIMEMAINREADING_H
#define TIMEMAINREADING_H

#include <stdint.h>
#include "TimerCopyReading.h"
#include "ImmediatelyReading.h" 

typedef struct TIMER_READING_S {
    unsigned char readMeter_CurrentSteep;		// 当前定时抄表流程步骤
    unsigned char readMeter_250mS_Delay;		// 等待延时计数  250ms计数
    unsigned char readMeter_No;					// 当前定时抄表的仪表个数
    unsigned char readMeter_Total_Count;		// 当前存储的总仪表个数
    unsigned char readMeter_Current_MeterID[7]; // 当前定时抄表的仪表号码
    unsigned char readMeter_Status_Flage;		// 当前定时抄表的状态 0:发送抄表命令 1：等待接收抄表数据
    unsigned char readMeter_ReReading_Count;	// 当前仪表重复抄表次数，3次不成功  失败
    unsigned char readMeter_OK_Flage;			// 抄表成功标志
    unsigned char readMeter_Up_Len;				// 上传数据长度
    unsigned char readMeter_Next_Flage;			// 读仪表号码标志位
    unsigned char readMeter_RxBuf[250];			// 读仪表数据
} TIMER_READING_T;

typedef struct IMMEDIATELY_READING_S {
    unsigned char immediately_Exec_Flage;
    unsigned char immediately_Current_Step;
    unsigned char immediately_250mS_Delay;
    unsigned char immediately_Ports;
    unsigned char immediately_TXBuffer[250];
    unsigned char immediately_TX_Len;
    unsigned char immediately_Reading_Current_Count; // 当前指令，重复读取次数 `
    unsigned char immediately_ReReading_Count;		 // 当前指令，重复读取次数 `
    unsigned char immediately_RXBuffer[250];
    unsigned char immediately_Up_Len;
    unsigned char immediately_OK_Flage;
} IMMEDIATELY_READING_T;

extern IMMEDIATELY_READING_T imm_Read;
extern TIMER_READING_T copyTimerRead;
extern TIMER_READING_T mainTimerRead;


extern unsigned char read_Main_Meter_In_Timer_In_Main_Flage;
extern void execute_Main_Reading_In_Timer(void);


#endif
