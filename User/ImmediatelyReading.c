#include "ImmediatelyReading.h" 
#include "MQTT_JSON.h"
#include "TimerMainReading.h"
#include "main.h" 

unsigned char up_Immediately_History_In_Timer_In_Main_Flage = 0;

void execute_Transparent_Reading_In_Timer(void) {
    switch (imm_Read.immediately_Current_Step) {
        case 0:
            Select_OFF_CH340_To_MBUS;
            Select_OFF_CH340_To_RS4851;
            Select_OFF_CH340_To_RS4852;
            if (DOWN_Mode_MBUS_ON == 1) Select_ON_MBUS_To_UART1;
            if (DOWN_Mode_RS4851_ON == 1) Select_ON_RS4851_To_UART4;
            if (DOWN_Mode_RS4852_ON == 1) Select_ON_RS4852_To_LPUART0;
            MBUS_OUT_ON;
            imm_Read.immediately_250mS_Delay = 2; //等待0.5秒
            imm_Read.immediately_Current_Step++;
            break;
        case 1:
            Clear_RXCommand_By_UART(imm_Read.immediately_Ports);
            imm_Read.immediately_OK_Flage = 0;
            Send_Command_InImmediately_By_UART(imm_Read.immediately_Ports, imm_Read.immediately_TXBuffer, imm_Read.immediately_TX_Len);
            imm_Read.immediately_Reading_Current_Count++;
            imm_Read.immediately_250mS_Delay = 12; //等待4秒
            imm_Read.immediately_Current_Step++;
            break;
        case 2:
            if (imm_Read.immediately_250mS_Delay == 0) {  //时间溢出
                if (imm_Read.immediately_OK_Flage != 0) {
                    if ((MQTT_4G_Connect_Flage == 1) || (Now_NetDevParameter.NowNetOnlineFlag == true)) {//存储抄表数据				 
                        JSON_Send_Immediately_Main_Copy_Meter_Data(40, imm_Read.immediately_Ports, imm_Read.immediately_RXBuffer, imm_Read.immediately_Up_Len);
                    }

                    imm_Read.immediately_Current_Step = 0;
                    imm_Read.immediately_OK_Flage = 0;
                    imm_Read.immediately_Exec_Flage = 0;
                } else {
                    if (imm_Read.immediately_Reading_Current_Count <= imm_Read.immediately_ReReading_Count) {
                        imm_Read.immediately_Current_Step = 1;
                    } else {
                        imm_Read.immediately_Current_Step = 0;
                        imm_Read.immediately_OK_Flage = 0;
                        imm_Read.immediately_Exec_Flage = 0;
                    }
                }
            }
            break;
    }
}
