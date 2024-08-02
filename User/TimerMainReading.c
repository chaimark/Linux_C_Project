#include "main.h"
#include "MQTT_JSON.h"
#include "RTC.h"
#include "Define.h"
#include "LPUart.h"
#include "AT24C0256.h"
#include "Uart.h"
#include "ImmediatelyReading.h"
#include "TimerMainReading.h"


IMMEDIATELY_READING_T imm_Read;
TIMER_READING_T copyTimerRead;
TIMER_READING_T mainTimerRead;
unsigned char read_Main_Meter_In_Timer_In_Main_Flage;

void execute_Main_Reading_In_Timer(void) // 定时抄表
{
    unsigned int i;
    switch (mainTimerRead.readMeter_CurrentSteep) {
        case 0: // 开启MBUS
            Select_OFF_CH340_To_MBUS;
            Select_OFF_CH340_To_RS4851;
            Select_OFF_CH340_To_RS4852;
            if (DOWN_Mode_MBUS_ON == 1)
                Select_ON_MBUS_To_UART1;
            if (DOWN_Mode_RS4851_ON == 1)
                Select_ON_RS4851_To_UART4;
            if (DOWN_Mode_RS4852_ON == 1)
                Select_ON_RS4852_To_LPUART0;
            MBUS_OUT_ON;

            mainTimerRead.readMeter_250mS_Delay = 12;											  // 等待2秒后开始抄表
            mainTimerRead.readMeter_Status_Flage = 0;											  // 改为发送抄表命令状态
            mainTimerRead.readMeter_No = 0;														  // 当前已抄仪表数为0;
            mainTimerRead.readMeter_Total_Count = EEprom_Byte1Read(EEPROM_MAIN_METER_TOTAL_ADDR); // 读出存储的总仪表个数   //抄主表
            if (mainTimerRead.readMeter_Total_Count > 20)
                mainTimerRead.readMeter_Total_Count = 20;
            mainTimerRead.readMeter_Next_Flage = 1; // 读仪表号码标志
            mainTimerRead.readMeter_CurrentSteep++; // 到下一步
            break;
        case 1:
            if (mainTimerRead.readMeter_Next_Flage != 0) {
                if (mainTimerRead.readMeter_No < mainTimerRead.readMeter_Total_Count) { // 当前已抄仪表个数小于总仪表数
                    for (i = 0; i < 7; i++) {
                        mainTimerRead.readMeter_Current_MeterID[i] = EEprom_Byte1Read(EEPROM_MAIN_METER_START_ADDR + 60 * mainTimerRead.readMeter_No + i); // 读出仪表号码	 //主表
                    }
                    mainTimerRead.readMeter_OK_Flage = 0;		 // 清除抄表成功标志
                    mainTimerRead.readMeter_ReReading_Count = 0; // 清除重复抄表次数
                    mainTimerRead.readMeter_Next_Flage = 0;		 // 清除读仪表号码标志
                    mainTimerRead.readMeter_Status_Flage = 0;	 // 发送抄表命令标志
                } else {											// 抄表完成
                    mainTimerRead.readMeter_CurrentSteep++; // 到下一步  结束
                }
            }
            if (mainTimerRead.readMeter_Status_Flage == 0) { // 发送抄表命令
                if (mainTimerRead.readMeter_250mS_Delay == 0) { // 等待时间到
                    Clear_RXCommand_By_UART(mainTimerRead.readMeter_Current_MeterID[6]);
                    mainTimerRead.readMeter_OK_Flage = 0;									 // 清除抄表成功标志
                    Send_Command_InMain_By_UART(mainTimerRead.readMeter_Current_MeterID[6]); // 发送抄表命令
                    mainTimerRead.readMeter_250mS_Delay = 24;								 // 溢出时间4秒
                    mainTimerRead.readMeter_Status_Flage = 1;								 // 进入等待接收抄表数据模式
                }
            } else { // 等待接收数据
                if (mainTimerRead.readMeter_OK_Flage != 0) { // 抄表成功 
                    if ((MQTT_4G_Connect_Flage == 1) || (Now_NetDevParameter.NowNetOnlineFlag == true)) { // 存储抄表数据
                        JSON_Send_Main_Copy_Meter_Data(20, mainTimerRead.readMeter_Current_MeterID, mainTimerRead.readMeter_RxBuf, mainTimerRead.readMeter_Up_Len);
                    }
                    mainTimerRead.readMeter_No++;			 // 下一只表
                    mainTimerRead.readMeter_Next_Flage = 1;	 // 读下一只表标志
                    mainTimerRead.readMeter_250mS_Delay = 0; // 清除等待延时
                    mainTimerRead.readMeter_Status_Flage = 0;
                } else {
                    if (mainTimerRead.readMeter_250mS_Delay == 0) {											   // 溢出时间到
                        mainTimerRead.readMeter_ReReading_Count++; // 重复抄表次数加1
                        if (mainTimerRead.readMeter_ReReading_Count >= 5) {											 // 连续5次超时    抄表失败
                            mainTimerRead.readMeter_No++;			 // 下一只表
                            mainTimerRead.readMeter_Next_Flage = 1;	 // 读下一只表标志
                            mainTimerRead.readMeter_250mS_Delay = 0; // 清除等待延时
                            mainTimerRead.readMeter_Status_Flage = 0;
                        } else {
                            mainTimerRead.readMeter_Status_Flage = 0; // 继续发送抄表命令
                        }
                    }
                }
            }
            break;
        case 2: // 抄表完成
            mainTimerRead.readMeter_CurrentSteep = 0;
            read_Main_Meter_In_Timer_In_Main_Flage = 0; // 主表
            break;
        default:
            break;
    }
}
