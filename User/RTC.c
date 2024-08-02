#include "RTC.h"
#include "Define.h"
#include "4G_Module.h"
#include "AT24C0256.h"
#include "ADC.h"
#include "RX8025T.h"
#include "TimerMainReading.h"
#include "NumberBaseLib.h"
#include <time.h>


// 内部 RTC ***************************
FL_RTC_InitTypeDef RTC_Data;

unsigned char MBUS_UART1_Reboot_In_Main_Flage;
unsigned char Set_RX8025T_By_MQTT_CCLK_In_Main_Flage;
unsigned char Adjust_Meter_In_Main_Flage;

unsigned int Test_MBUS_1Second_Count;
unsigned int RTC_1Second_Count;
unsigned int Read_Main_Meter_1Min_Count;
unsigned int Read_Copy_Meter_1Min_Count;
unsigned int execute_EC20_Reboot_By_Hard_1Min_Count;
unsigned int execute_MBUS_Reboot_1Min_Count;
// 异步复位控制
unsigned char EC20_ReBoot_Falge;
int EC20_ReBoot_1Second_Delay;
unsigned int EC20_OnLine_Get_CCLK_1Min_Count;

unsigned int execute_Reading_By_Main_1Second_Count;
unsigned int execute_Reading_By_Copy_1Second_Count;
unsigned int execute_Reading_By_Transparent_1Second_Count;
unsigned int execute_Set_RX8025T_By_MQTT_CCLK_1Second_Count;
unsigned int execute_Adjust_Meter_1Second_Count;

void EC20_ReBoot_In_RTC_By_1Second(char sec) {
    EC20_ReBoot_Falge = 1;
    EC20_ReBoot_1Second_Delay = sec;
}

void RTC_ReBoot_Check(void) {
    if (EC20_ReBoot_Falge == 1) {
        EC20_ReBoot_1Second_Delay--;
        if (EC20_ReBoot_1Second_Delay < 0)
            NVIC_SystemReset();
    }
}

void check_Read_Meter_In_Heating(void) {
    unsigned char in_heating = 0;
    if (RTC_Data.month > ((AT24CXX_Manager.heating_start[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_start[6] & 0x0F))) {
        in_heating = 1;
    } else {
        if (RTC_Data.month == ((AT24CXX_Manager.heating_start[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_start[6] & 0x0F))) {
            if (RTC_Data.day >= ((AT24CXX_Manager.heating_start[8] & 0x0F) * 16 + (AT24CXX_Manager.heating_start[9] & 0x0F))) {
                in_heating = 1;
            }
        }
    }

    if (RTC_Data.month < ((AT24CXX_Manager.heating_end[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_end[6] & 0x0F))) {
        in_heating = 1;
    } else {
        if (RTC_Data.month == ((AT24CXX_Manager.heating_end[5] & 0x0F) * 16 + (AT24CXX_Manager.heating_end[6] & 0x0F))) {
            if (RTC_Data.day <= ((AT24CXX_Manager.heating_end[8] & 0x0F) * 16 + (AT24CXX_Manager.heating_end[9] & 0x0F))) {
                in_heating = 1;
            }
        }
    }

    if (in_heating == 1) { // 在供热周期
        Read_Main_Meter_1Min_Count++;
        if (Read_Main_Meter_1Min_Count >= AT24CXX_Manager.main_interval) { // 定时抄表时间到
            Read_Main_Meter_1Min_Count = 0;
            read_Main_Meter_In_Timer_In_Main_Flage = 1;
        }

        Read_Copy_Meter_1Min_Count++;
        if (Read_Copy_Meter_1Min_Count >= AT24CXX_Manager.copy_interval) { // 定时抄表时间到
            Read_Copy_Meter_1Min_Count = 0;
            read_Copy_Meter_In_Timer_In_Main_Flage = 1;
        }
    } else { // 不在供热周期
        Read_Main_Meter_1Min_Count++;
        if (Read_Main_Meter_1Min_Count >= AT24CXX_Manager.not_intimer_interval) { // 定时抄表时间到
            Read_Main_Meter_1Min_Count = 0;
            read_Main_Meter_In_Timer_In_Main_Flage = 1;
        }

        Read_Copy_Meter_1Min_Count++;
        if (Read_Copy_Meter_1Min_Count >= AT24CXX_Manager.not_intimer_interval) { // 定时抄表时间到
            Read_Copy_Meter_1Min_Count = 0;
            read_Copy_Meter_In_Timer_In_Main_Flage = 1;
        }
    }
}

void RTC_IRQHandler(void) {
    FL_RTC_ClearFlag_Second(RTC);
    FL_IWDT_ReloadCounter(IWDT);    // 喂狗
    FL_RTC_GetTime(RTC, &RTC_Data); // 获取时间
    // 异步重启
    RTC_ReBoot_Check();
    // 每天一次对时
    //	if ( RTC_Data.second == 0 && RTC_Data.minute == 0 && RTC_Data.hour == 0 )
    Test_MBUS_1Second_Count++;
    if (Test_MBUS_1Second_Count >= 2) {
        Test_MBUS_1Second_Count = 0;
        Current_MBUS_MV_Value = Test_Current_Voltge_mV();
        Current_MBUS_MA_Value = Test_Current_MBUS_mA();
    }

    if (MQTT_Important_Even_1Second_Count > 0)
        MQTT_Important_Even_1Second_Count++;
    if (MQTT_Important_Even_1Second_Count > 30) {
        MQTT_Important_Even_1Second_Count = 0;
    }

    if (execute_Set_RX8025T_By_MQTT_CCLK_1Second_Count > 0)
        execute_Set_RX8025T_By_MQTT_CCLK_1Second_Count++;
    if (execute_Set_RX8025T_By_MQTT_CCLK_1Second_Count >= 10) {
        execute_Set_RX8025T_By_MQTT_CCLK_1Second_Count = 0;
        Set_RX8025T_By_MQTT_CCLK_In_Main_Flage = 1;
    }

    if (execute_Adjust_Meter_1Second_Count > 0)
        execute_Adjust_Meter_1Second_Count++;
    if (execute_Adjust_Meter_1Second_Count >= 60) {
        execute_Adjust_Meter_1Second_Count = 0;
        Adjust_Meter_In_Main_Flage = 1;
    }

    if (UP_Mode_EC20_ON == 1) {
        if (MQTT_4G_Connect_Flage == 1) {
            if ((MQTT_Important_Even_1Second_Count != 0) || (read_Main_Meter_In_Timer_In_Main_Flage != 0) || (read_Copy_Meter_In_Timer_In_Main_Flage != 0)) {
                if ((RTC_1Second_Count % 2) == 0)
                    Heart_LED_H;
                else
                    Heart_LED_L;
            } else
                Heart_LED_H;
        } else
            Heart_LED_L;
    }

    if (UP_Mode_NET_ON == 1) {
        if ((RTC_1Second_Count % 2) == 0) {
            if (Now_NetDevParameter.NowNetOnlineFlag == true)
                Heart_LED_H;
            else
                Heart_LED_L;
        }
    }
    // 根据模式点亮对应的指示灯    
    (UP_Mode_EC20_ON == 1 ? EC20_LED_H : EC20_LED_L);
    (UP_Mode_NET_ON == 1 ? NET_LED_H : NET_LED_L);
    ((DOWN_Mode_RS4851_ON & DOWN_Mode_RS4852_ON) == 1 ? RS485_LED_H : RS485_LED_L);

    if (DOWN_Mode_MBUS_ON == 1) {
        if (Current_MBUS_MV_Value < 16000)
            ERR_LED_H;
        else
            ERR_LED_L;
    }

    RTC_1Second_Count++;
    if (RTC_1Second_Count >= 60) {
        RTC_1Second_Count = 0;

        if (UP_Mode_EC20_ON == 1) {
            if (MQTT_4G_Connect_Flage == 1)
                execute_EC20_Reboot_By_Hard_1Min_Count = 0; // 两天内无连接硬件重启模块
            else
                execute_EC20_Reboot_By_Hard_1Min_Count++;
            if (execute_EC20_Reboot_By_Hard_1Min_Count > 2880) {
                NVIC_SystemReset();
            }
            if (MQTT_4G_Connect_Flage == 1) {
                EC20_OnLine_Get_CCLK_1Min_Count++;
                if (EC20_OnLine_Get_CCLK_1Min_Count == 2) {
                    Set_RX8025T_By_MQTT_CCLK_In_Main_Flage = 1;
                }
                if (EC20_OnLine_Get_CCLK_1Min_Count > 30) {
                    EC20_OnLine_Get_CCLK_1Min_Count = 5;
                }
            }
        }

        if (DOWN_Mode_MBUS_ON == 1) {
            execute_MBUS_Reboot_1Min_Count++;
            if (execute_MBUS_Reboot_1Min_Count > 2160) {
                MBUS_UART1_Reboot_In_Main_Flage = 1;
            }
        }

        check_Read_Meter_In_Heating();

        if (read_Main_Meter_In_Timer_In_Main_Flage != 0) {
            execute_Reading_By_Main_1Second_Count++;
            if (execute_Reading_By_Main_1Second_Count > 1800) {
                execute_Reading_By_Main_1Second_Count = 0;
                read_Main_Meter_In_Timer_In_Main_Flage = 0;
            }
        } else
            execute_Reading_By_Main_1Second_Count = 0;

        if (read_Copy_Meter_In_Timer_In_Main_Flage != 0) {
            execute_Reading_By_Copy_1Second_Count++;
            if (execute_Reading_By_Copy_1Second_Count > 3600) {
                execute_Reading_By_Copy_1Second_Count = 0;
                read_Copy_Meter_In_Timer_In_Main_Flage = 0;
            }
        } else
            execute_Reading_By_Copy_1Second_Count = 0;

        if (imm_Read.immediately_Exec_Flage != 0) {
            execute_Reading_By_Transparent_1Second_Count++;
            if (execute_Reading_By_Transparent_1Second_Count > 600) {
                execute_Reading_By_Transparent_1Second_Count = 0;
                imm_Read.immediately_Exec_Flage = 0;
            }
        } else
            execute_Reading_By_Transparent_1Second_Count = 0;
    }
}

void RTC_Init_By_RX8025T(void) {
    FL_RTC_InitTypeDef rtc;
    if (HasRX8025T != 0) {
        RX8025T_Init();
        FL_DelayMs(100);
    } else {
        RX8025T_Close();
    }
    FL_RTC_ConfigTime(RTC, &rtc);
    FL_RTC_GetTime(RTC, &RTC_Data); // 获取时间
}
// 计算周
int getDayOfWeek(uint32_t iYear, uint32_t iMonth, uint32_t iDay) {
    int iWeek = 0;
    unsigned int y = 0, c = 0, m = 0, d = 0;

    iYear = anyBaseToAnyBase(iYear, 10, 16);
    iMonth = anyBaseToAnyBase(iMonth, 10, 16);
    iDay = anyBaseToAnyBase(iDay, 10, 16);

    if (iMonth == 1 || iMonth == 2) {
        c = (iYear - 1) / 100;
        y = (iYear - 1) % 100;
        m = iMonth + 12;
        d = iDay;
    } else {
        c = iYear / 100;
        y = iYear % 100;
        m = iMonth;
        d = iDay;
    }

    iWeek = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + d - 1; //蔡勒公式
    iWeek = iWeek >= 0 ? (iWeek % 7) : (iWeek % 7 + 7); //iWeek为负时取模
    if (iWeek == 0) //星期日不作为一周的第一天
    {
        iWeek = 7;
    }

    return iWeek;
}

// 设置时间
void setRtcDate(strnew SteStrRTCData) {
    FL_RTC_InitTypeDef SetRTC;
    char * NowAddr = SteStrRTCData.Name._char + 1; // 初始化 NowAddr "2024-07-05 15:44:03"
    int NowAddrLen = 0;
    char * TempCh = NULL;

    // 获取字符串 年 和对应长度
    TempCh = strchr(NowAddr, '-');
    (*TempCh) = '\0';
    NowAddrLen = TempCh - NowAddr;
    swapStr(NowAddr, NowAddrLen);
    SetRTC.year = doneAsciiStrToAnyBaseNumberData(NowAddr, NowAddrLen, 10);  // 字符串 转 任意进制数
    NowAddr = TempCh + 1;

    // 获取字符串 月 和对应长度
    TempCh = strchr(NowAddr, '-');
    (*TempCh) = '\0';
    NowAddrLen = TempCh - NowAddr;
    swapStr(NowAddr, NowAddrLen);
    SetRTC.month = doneAsciiStrToAnyBaseNumberData(NowAddr, NowAddrLen, 10);  // 字符串 转 任意进制数
    NowAddr = TempCh + 1;

    // 获取字符串 日 和对应长度
    TempCh = strchr(NowAddr, ' ');
    (*TempCh) = '\0';
    NowAddrLen = TempCh - NowAddr;
    swapStr(NowAddr, NowAddrLen);
    SetRTC.day = doneAsciiStrToAnyBaseNumberData(NowAddr, NowAddrLen, 10);  // 字符串 转 任意进制数
    NowAddr = TempCh + 1;

    // 获取字符串 时 和对应长度
    TempCh = strchr(NowAddr, ':');
    (*TempCh) = '\0';
    NowAddrLen = TempCh - NowAddr;
    swapStr(NowAddr, NowAddrLen);
    SetRTC.hour = doneAsciiStrToAnyBaseNumberData(NowAddr, NowAddrLen, 10);  // 字符串 转 任意进制数
    NowAddr = TempCh + 1;

    // 获取字符串 分 和对应长度
    TempCh = strchr(NowAddr, ':');
    (*TempCh) = '\0';
    NowAddrLen = TempCh - NowAddr;
    swapStr(NowAddr, NowAddrLen);
    SetRTC.minute = doneAsciiStrToAnyBaseNumberData(NowAddr, NowAddrLen, 10);  // 字符串 转 任意进制数
    NowAddr = TempCh + 1;

    // 获取字符串 秒 和对应长度
    TempCh = strchr(NowAddr, '\"');
    (*TempCh) = '\0';
    NowAddrLen = TempCh - NowAddr;
    swapStr(NowAddr, NowAddrLen);
    SetRTC.second = doneAsciiStrToAnyBaseNumberData(NowAddr, NowAddrLen, 10);  // 字符串 转 任意进制数

    // 计算周
    SetRTC.week = getDayOfWeek(SetRTC.year, SetRTC.month, SetRTC.day);

    FL_RTC_ConfigTime(RTC, &SetRTC);   // 设置时间结构体
    FL_RTC_GetTime(RTC, &RTC_Data); // 获取时间
    unsigned char EEprom_System_Time[7] = {0};
    EEprom_System_Time[0] = RTC_Data.year;
    EEprom_System_Time[1] = RTC_Data.month;
    EEprom_System_Time[2] = RTC_Data.day;
    EEprom_System_Time[3] = RTC_Data.week;;
    EEprom_System_Time[4] = RTC_Data.hour;
    EEprom_System_Time[5] = RTC_Data.minute;
    EEprom_System_Time[6] = RTC_Data.second;
    for (int i = 0;i < 7;i++) {
        EEprom_Byte1Write(EEPROM_SYSTIME_ADDRESS + i, EEprom_System_Time[i]);
    }
    FL_RTC_ConfigTime(RTC, &RTC_Data);
}
void getStrNowDataAndTimeByRTC(char NowStrRTCData[]) {
    FL_RTC_GetTime(RTC, &RTC_Data); // 获取时间
    // 年月日时分秒
    sprintf(NowStrRTCData, "20%02x-%02x-%02x %02x:%02x:%02x", RTC_Data.year, RTC_Data.month, RTC_Data.day, RTC_Data.hour, RTC_Data.minute, RTC_Data.second);
}
