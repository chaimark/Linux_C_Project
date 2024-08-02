#include "TimerCopyReading.h"
#include "MQTT_JSON.h"
#include "AT24C0256.h"
#include "Uart.h"
#include "RTC.h"
#include "LPUart.h"
#include "Define.h"
#include "4G_Module.h"
#include "main.h" 


unsigned char read_Copy_Meter_In_Timer_In_Main_Flage = 0;


void execute_Copy_Reading_In_Timer(void) { //定时抄表
    unsigned int i;
    switch (copyTimerRead.readMeter_CurrentSteep) {
        case 0:   //开启MBUS
            Select_OFF_CH340_To_MBUS;
            Select_OFF_CH340_To_RS4851;
            Select_OFF_CH340_To_RS4852;
            if (DOWN_Mode_MBUS_ON == 1) Select_ON_MBUS_To_UART1;
            if (DOWN_Mode_RS4851_ON == 1) Select_ON_RS4851_To_UART4;
            if (DOWN_Mode_RS4852_ON == 1) Select_ON_RS4852_To_LPUART0;
            MBUS_OUT_ON;

            copyTimerRead.readMeter_250mS_Delay = 12;  //等待2秒后开始抄表
            copyTimerRead.readMeter_Status_Flage = 0;     //改为发送抄表命令状态
            copyTimerRead.readMeter_No = 0;   //当前已抄仪表数为0;		
            copyTimerRead.readMeter_Total_Count = EEprom_Byte1Read(EEPROM_COPY_METER_TOTAL_ADDR); //读出存储的总仪表个数	//抄辅表
            if (copyTimerRead.readMeter_Total_Count > 240)copyTimerRead.readMeter_Total_Count = 240;
            copyTimerRead.readMeter_Next_Flage = 1;  //读仪表号码标志
            copyTimerRead.readMeter_CurrentSteep++;  //到下一步
            break;
        case 1:
            if (copyTimerRead.readMeter_Next_Flage != 0) {
                if (copyTimerRead.readMeter_No < copyTimerRead.readMeter_Total_Count) {  //当前已抄仪表个数小于总仪表数
                    for (i = 0;i < 7;i++) {
                        copyTimerRead.readMeter_Current_MeterID[i] = EEprom_Byte1Read(EEPROM_COPY_METER_START_ADDR + 60 * copyTimerRead.readMeter_No + i); //读出仪表号码	 //辅表
                    }
                    copyTimerRead.readMeter_OK_Flage = 0;  //清除抄表成功标志
                    copyTimerRead.readMeter_ReReading_Count = 0; //清除重复抄表次数
                    copyTimerRead.readMeter_Next_Flage = 0;  //清除读仪表号码标志
                    copyTimerRead.readMeter_Status_Flage = 0;       //发送抄表命令标志
                } else {     //抄表完成
                    copyTimerRead.readMeter_CurrentSteep++;  //到下一步  结束
                }
            }
            if (copyTimerRead.readMeter_Status_Flage == 0) {   //发送抄表命令
                if (copyTimerRead.readMeter_250mS_Delay == 0) {  //等待时间到			 
                    Clear_RXCommand_By_UART(copyTimerRead.readMeter_Current_MeterID[6]);
                    copyTimerRead.readMeter_OK_Flage = 0;  //清除抄表成功标志
                    Send_Command_InCopy_By_UART(copyTimerRead.readMeter_Current_MeterID[6]); //发送抄表命令
                    copyTimerRead.readMeter_250mS_Delay = 24;       //溢出时间4秒
                    copyTimerRead.readMeter_Status_Flage = 1;      //进入等待接收抄表数据模式
                }
            } else {      //等待接收数据
                if (copyTimerRead.readMeter_OK_Flage != 0) {  //抄表成功	
                    if ((MQTT_4G_Connect_Flage == 1) || (Now_NetDevParameter.NowNetOnlineFlag == true)) {//存储抄表数据				 
                        JSON_Send_Main_Copy_Meter_Data(20, copyTimerRead.readMeter_Current_MeterID, copyTimerRead.readMeter_RxBuf, copyTimerRead.readMeter_Up_Len);
                    }
                    copyTimerRead.readMeter_No++;  //下一只表
                    copyTimerRead.readMeter_Next_Flage = 1;//读下一只表标志
                    copyTimerRead.readMeter_250mS_Delay = 0; //清除等待延时
                    copyTimerRead.readMeter_Status_Flage = 0;
                } else {
                    if (copyTimerRead.readMeter_250mS_Delay == 0) {  //溢出时间到
                        copyTimerRead.readMeter_ReReading_Count++;  //重复抄表次数加1
                        if (copyTimerRead.readMeter_ReReading_Count >= 3) { //连续3次超时    抄表失败						 
                            copyTimerRead.readMeter_No++;  //下一只表
                            copyTimerRead.readMeter_Next_Flage = 1;//读下一只表标志
                            copyTimerRead.readMeter_250mS_Delay = 0; //清除等待延时
                            copyTimerRead.readMeter_Status_Flage = 0;
                        } else {
                            copyTimerRead.readMeter_Status_Flage = 0;  //继续发送抄表命令
                        }
                    }
                }
            }
            break;
        case 2:     //抄表完成
            copyTimerRead.readMeter_CurrentSteep = 0;
            read_Copy_Meter_In_Timer_In_Main_Flage = 0; //辅表
            if (AT24CXX_Manager.copy_statistics == 1) {
                if (RTC_Data.month > ((AT24CXX_Manager.heating_start[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_start[6] & 0x0F))) {
                    JSON_Send_Copy_Statistics();
                } else {
                    if (RTC_Data.month == ((AT24CXX_Manager.heating_start[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_start[6] & 0x0F))) {
                        if (RTC_Data.day >= ((AT24CXX_Manager.heating_start[8] & 0x0F) * 16 + (AT24CXX_Manager.heating_start[9] & 0x0F))) {
                            JSON_Send_Copy_Statistics();
                        }
                    }
                }

                if (RTC_Data.month < ((AT24CXX_Manager.heating_end[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_end[6] & 0x0F))) {
                    JSON_Send_Copy_Statistics();
                } else {
                    if (RTC_Data.month == ((AT24CXX_Manager.heating_end[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_end[6] & 0x0F))) {
                        if (RTC_Data.day <= ((AT24CXX_Manager.heating_end[8] & 0x0F) * 16 + (AT24CXX_Manager.heating_end[9] & 0x0F))) {
                            JSON_Send_Copy_Statistics();
                        }
                    }
                }
            }
            break;
        default:break;
    }
}
