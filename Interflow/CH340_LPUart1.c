/**************************************************************************
 * @file
 * @brief LEUART Demo Application
 * @author Energy Micro AS
 * @version 1.02
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
#include "NetProt_Module.h"
#include "CH340_LPUart1.h"
#include "AT24C0256.h"
#include "StrLib.h"
#include "NumberBaseLib.h"
#include "LPUart.h"
#include "Define.h"
#include "RTC.h"

unsigned char CH340_LPUART1_RXDATA_Count_Above30 = 0; // 在配置的时候不显示
unsigned int LEUART0_Count485;
unsigned char LEUART0_Buffer;
uint32_t LEUSART0_IF_Flage;

/*end LED配置调用****************************************************/

// 状态显示
void Heart_LED_ON() {
    // Set_Heart_LED_ON();
    // RTC_Heart_LED_Count = 3;
}

void Modfiy_Parameter_By_CH340(void) {
    uint16_t Readlen = 0;
    int Len = 0;
    char cs;
    int i;
    unsigned char Temp_Buffer[80] = {0};
    memset((char*)&LPUART1Ddata.TxBuf, 0x00, LPUARTMAX);
    LPUART1Ddata.TxLen = 0;
    // 功能一：读IMEI
    if ((LPUART1Ddata.RxBuf[2] == 0xF1) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        // Read_NET_Local_IMEI_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xF1;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        for (i = 0; i < 10; i++) {
            LPUART1Ddata.TxBuf[Len++] = AT24CXX_Manager.NET_Local_IMEI[i];
        }
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2;
        cs = 0;
        for (i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能二：读LocaIP
    if ((LPUART1Ddata.RxBuf[2] == 0xF2) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        // Read_NET_Local_IP_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xF2;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;

        for (int i = 0; i < 30; i++) {
            if (AT24CXX_Manager.NET_Local_IP[i] == '\0') {
                LPUART1Ddata.TxBuf[Len++] = '\r';
                LPUART1Ddata.TxBuf[Len++] = '\n';
                break;
            }
            LPUART1Ddata.TxBuf[Len++] = AT24CXX_Manager.NET_Local_IP[i];
        }
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2; //
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能二：读LocaMASK
    if ((LPUART1Ddata.RxBuf[2] == 0xF3) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        // Read_NET_Local_MASK_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xF3;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        for (int i = 0; i < 30; i++) {
            if (AT24CXX_Manager.NET_Local_MASK[i] == '\0') {
                LPUART1Ddata.TxBuf[Len++] = '\r';
                LPUART1Ddata.TxBuf[Len++] = '\n';
                break;
            }
            LPUART1Ddata.TxBuf[Len++] = AT24CXX_Manager.NET_Local_MASK[i];
        }
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2; //
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能二：读Loca网关
    if ((LPUART1Ddata.RxBuf[2] == 0xF4) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        // Read_NET_Local_GATEWAY_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xF4;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        for (int i = 0; i < 30; i++) {
            if (AT24CXX_Manager.NET_Local_GATEWAY[i] == '\0') {
                LPUART1Ddata.TxBuf[Len++] = '\r';
                LPUART1Ddata.TxBuf[Len++] = '\n';
                break;
            }
            LPUART1Ddata.TxBuf[Len++] = AT24CXX_Manager.NET_Local_GATEWAY[i];
        }
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2; //
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能二：读RemoteIP
    if ((LPUART1Ddata.RxBuf[2] == 0xF6) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        // Read_NET_Remote_IP_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xF6;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        for (int i = 0; i < 30; i++) {
            if (AT24CXX_Manager.NET_4G_Remote_Url[i] == '\0') {
                LPUART1Ddata.TxBuf[Len++] = '\r';
                LPUART1Ddata.TxBuf[Len++] = '\n';
                ;
                break;
            }
            LPUART1Ddata.TxBuf[Len++] = AT24CXX_Manager.NET_4G_Remote_Url[i];
        }
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2; //
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能二：读Remote端口
    if ((LPUART1Ddata.RxBuf[2] == 0xF7) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        // Read_NET_Remote_Port_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xF7;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;

        char TempProt[30] = {0};

        int ProtLen = doneBaseNumberDataToAsciiStr(TempProt, 30, AT24CXX_Manager.NET_4G_Remote_Port, 16);
        TempProt[(ProtLen < 30) ? ProtLen : (ProtLen--)] = '\0';
        for (int i = 0; i < ProtLen; i++) {
            if (TempProt[i] == '\0') {
                LPUART1Ddata.TxBuf[Len++] = '\r';
                LPUART1Ddata.TxBuf[Len++] = '\n';
                ;
                break;
            }
            LPUART1Ddata.TxBuf[Len++] = TempProt[i];
        }
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2; //
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能二：修改 LocaIP
    if (LPUART1Ddata.RxBuf[2] == 0xD2) {
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // OK

        // 保存
        Readlen = LPUART1Ddata.RxBuf[3] - 2 - 2;
        memset(AT24CXX_Manager.NET_Local_IP, 0, ARR_SIZE(AT24CXX_Manager.NET_Local_IP));
        memcpy(AT24CXX_Manager.NET_Local_IP, &LPUART1Ddata.RxBuf[4], Readlen);
        EEprom_JSON_Write((unsigned char *)AT24CXX_Manager.NET_Local_IP, sizeof(AT24CXX_Manager.NET_Local_IP));

        Temp_Buffer[0] = 'A';
        Temp_Buffer[1] = 'T';
        Temp_Buffer[2] = '+';
        Temp_Buffer[3] = 'I';
        Temp_Buffer[4] = 'P';
        Temp_Buffer[5] = '=';
        for (i = 0; i < 70; i++) {
            if ((LPUART1Ddata.RxBuf[i + 4] == 0x0D) && (LPUART1Ddata.RxBuf[i + 5] == 0x0A)) {
                Temp_Buffer[i + 6] = '\r';
                Temp_Buffer[i + 7] = '\n';
                Temp_Buffer[i + 8] = '\0';
                goto next2;
            } else {
                Temp_Buffer[i + 6] = LPUART1Ddata.RxBuf[i + 4];
            }
        }
    next2:
        RTC_NET_RST_Count = 180; //
    }
    // 功能二：修改 LocaMASK 子网掩码
    if (LPUART1Ddata.RxBuf[2] == 0xD3) {
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // OK

        // 保存
        Readlen = LPUART1Ddata.RxBuf[3] - 2 - 2;
        memset(AT24CXX_Manager.NET_Local_MASK, 0, ARR_SIZE(AT24CXX_Manager.NET_Local_MASK));
        memcpy(AT24CXX_Manager.NET_Local_MASK, &LPUART1Ddata.RxBuf[4], Readlen);
        EEprom_JSON_Write((unsigned char *)AT24CXX_Manager.NET_Local_MASK, sizeof(AT24CXX_Manager.NET_Local_MASK));

        Temp_Buffer[0] = 'A';
        Temp_Buffer[1] = 'T';
        Temp_Buffer[2] = '+';
        Temp_Buffer[3] = 'M';
        Temp_Buffer[4] = 'A';
        Temp_Buffer[5] = 'S';
        Temp_Buffer[6] = 'K';
        Temp_Buffer[7] = '=';
        for (i = 0; i < 70; i++) {
            if ((LPUART1Ddata.RxBuf[i + 4] == 0x0D) && (LPUART1Ddata.RxBuf[i + 5] == 0x0A)) {
                Temp_Buffer[i + 8] = '\r';
                Temp_Buffer[i + 9] = '\n';
                Temp_Buffer[i + 10] = '\0';
                goto next3;
            } else {
                Temp_Buffer[i + 8] = LPUART1Ddata.RxBuf[i + 4];
            }
        }
    next3:
        RTC_NET_RST_Count = 180; //
    }
    // 功能二：修改 Loca网关
    if (LPUART1Ddata.RxBuf[2] == 0xD4) {
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // OK

        // 保存
        Readlen = LPUART1Ddata.RxBuf[3] - 2 - 2;
        memset(AT24CXX_Manager.NET_Local_GATEWAY, 0, ARR_SIZE(AT24CXX_Manager.NET_Local_GATEWAY));
        memcpy(AT24CXX_Manager.NET_Local_GATEWAY, &LPUART1Ddata.RxBuf[4], Readlen);
        EEprom_JSON_Write((unsigned char *)AT24CXX_Manager.NET_Local_GATEWAY, sizeof(AT24CXX_Manager.NET_Local_GATEWAY));

        Temp_Buffer[0] = 'A';
        Temp_Buffer[1] = 'T';
        Temp_Buffer[2] = '+';
        Temp_Buffer[3] = 'G';
        Temp_Buffer[4] = 'A';
        Temp_Buffer[5] = 'T';
        Temp_Buffer[6] = 'E';
        Temp_Buffer[7] = 'W';
        Temp_Buffer[8] = 'A';
        Temp_Buffer[9] = 'Y';
        Temp_Buffer[10] = '=';
        for (i = 0; i < 70; i++) {
            if ((LPUART1Ddata.RxBuf[i + 4] == 0x0D) && (LPUART1Ddata.RxBuf[i + 5] == 0x0A)) {
                Temp_Buffer[i + 11] = '\r';
                Temp_Buffer[i + 12] = '\n';
                Temp_Buffer[i + 13] = '\0';
                goto next4;
            } else {
                Temp_Buffer[i + 11] = LPUART1Ddata.RxBuf[i + 4];
            }
        }
    next4:
        RTC_NET_RST_Count = 180; //
    }
    // 功能二：修改 RemoteIP
    if (LPUART1Ddata.RxBuf[2] == 0xD6) {
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // OK

        // 保存
        Readlen = LPUART1Ddata.RxBuf[3] - 2 - 2;
        memset(AT24CXX_Manager.NET_4G_Remote_Url, 0, ARR_SIZE(AT24CXX_Manager.NET_4G_Remote_Url));
        memcpy(AT24CXX_Manager.NET_4G_Remote_Url, &LPUART1Ddata.RxBuf[4], Readlen);
        EEprom_JSON_Write((unsigned char *)AT24CXX_Manager.NET_4G_Remote_Url, sizeof(AT24CXX_Manager.NET_4G_Remote_Url));

        Temp_Buffer[0] = 'A';
        Temp_Buffer[1] = 'T';
        Temp_Buffer[2] = '+';
        Temp_Buffer[3] = 'D';
        Temp_Buffer[4] = 'I';
        Temp_Buffer[5] = 'P';
        Temp_Buffer[6] = '=';
        for (int i = 0; i < 70; i++) {
            if ((LPUART1Ddata.RxBuf[i + 4] == 0x0D) && (LPUART1Ddata.RxBuf[i + 5] == 0x0A)) {
                Temp_Buffer[i + 7] = '\r';
                Temp_Buffer[i + 8] = '\n';
                Temp_Buffer[i + 9] = '\0';
                goto next6;
            } else {
                Temp_Buffer[i + 7] = LPUART1Ddata.RxBuf[i + 4];
            }
        }
    next6:
        RTC_NET_RST_Count = 180; //
    }
    // 功能二：修改 Remote端口
    if (LPUART1Ddata.RxBuf[2] == 0xD7) {
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // 回复OK

        // 保存
        Readlen = LPUART1Ddata.RxBuf[3] - 2 - 2;
        swapStr((char *)&LPUART1Ddata.RxBuf[4], Readlen);
        AT24CXX_Manager.NET_4G_Remote_Port = doneAsciiStrToAnyBaseNumberData((char *)&LPUART1Ddata.RxBuf[4], Readlen, 16);
        EEprom_JSON_Write((unsigned char *)AT24CXX_Manager.NET_4G_Remote_Port, 1);

        Temp_Buffer[0] = 'A';
        Temp_Buffer[1] = 'T';
        Temp_Buffer[2] = '+';
        Temp_Buffer[3] = 'D';
        Temp_Buffer[4] = 'P';
        Temp_Buffer[5] = 'O';
        Temp_Buffer[6] = 'R';
        Temp_Buffer[7] = 'T';
        Temp_Buffer[8] = '=';
        for (i = 0; i < 70; i++) {
            if ((LPUART1Ddata.RxBuf[i + 4] == 0x0D) && (LPUART1Ddata.RxBuf[i + 5] == 0x0A)) {
                Temp_Buffer[i + 9] = '\r';
                Temp_Buffer[i + 10] = '\n';
                Temp_Buffer[i + 11] = '\0';
                goto next7;
            } else {
                Temp_Buffer[i + 9] = LPUART1Ddata.RxBuf[i + 4];
            }
        }
    next7:
        RTC_NET_RST_Count = 180; //
    }
    // 功能三：读工作模式
    if ((LPUART1Ddata.RxBuf[2] == 0xF9) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xF9;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        LPUART1Ddata.TxBuf[Len++] = AT24CXX_Manager.Save_Working_Mode;
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2; //
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能三：修改工作模式
    if (LPUART1Ddata.RxBuf[2] == 0xD9) {
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // 回复OK

        // 保存
        AT24CXX_Manager.Save_Working_Mode = LPUART1Ddata.RxBuf[4];
        EEprom_JSON_Write(&AT24CXX_Manager.Save_Working_Mode, 1);
        RTC_NET_RST_Count = 180; //
    }


    // 功能一: 读取 RTC 时间 6848FD 05 03901F 6416
    if (LPUART1Ddata.RxBuf[2] == 0xFD) {
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xFF;
        LPUART1Ddata.TxBuf[Len++] = 0xFF;// 长度
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        LPUART1Ddata.TxBuf[Len++] = '\"';
        char NowTime[20] = {0};
        getStrNowDataAndTimeByRTC(NowTime);
        Len = catString((char *)&LPUART1Ddata.TxBuf, NowTime, LPUARTMAX, strlen(NowTime)); // 当前时间
        LPUART1Ddata.TxBuf[Len++] = '\"';
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2;
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能一：读取网关标识 68 48 FE 0503901F 6516
    if (LPUART1Ddata.RxBuf[2] == 0xFE) {
        // Read_EC20_Local_IMEI_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xFE;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;

        char IDTemp[13] = {0};

        HEX2ToASCII((char *)AT24CXX_Manager.gw_id, 6, IDTemp, 12);
        for (int i = 0; i < 11; i++) {
            LPUART1Ddata.TxBuf[Len++] = IDTemp[i + 1];
        }
        LPUART1Ddata.TxBuf[3] = Len - 4 + 2; //
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能一：读取 IP and Prot 68 48 FF 0503901F 6616
    if (LPUART1Ddata.RxBuf[2] == 0xFF) {
        // Read_EC20_Remote_QIOPEN_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xFF;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;

        char urlTemp[30] = {0};
        memcpy(urlTemp, (char *)&AT24CXX_Manager.NET_4G_Remote_Url, sizeof(AT24CXX_Manager.NET_4G_Remote_Url));
        catString(urlTemp, ":", 30, 1);

        char TempProt[30] = {0};

        int ProtLen = doneBaseNumberDataToAsciiStr(TempProt, 30, AT24CXX_Manager.NET_4G_Remote_Port, 16);
        TempProt[(ProtLen < 30) ? ProtLen : (ProtLen--)] = '\0';
        catString(urlTemp, TempProt, 30, ProtLen);

        for (int i = 0; urlTemp[i] != '\0'; i++) {
            LPUART1Ddata.TxBuf[Len++] = urlTemp[i];
        }

        LPUART1Ddata.TxBuf[3] = Len - 4 + 2;
        cs = 0;
        for (int i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能一: 修改 RTC 时间 6848DD 17 22323032342D30372D32342031343A32363A353522 A216
    if (LPUART1Ddata.RxBuf[2] == 0xDD) {
        Readlen = LPUART1Ddata.RxBuf[3] - 2;
        // 回复
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1) {
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
        }

        copyString((char *)Temp_Buffer, (char *)&LPUART1Ddata.RxBuf[4], ARR_SIZE(Temp_Buffer), Readlen);
        char * TempStart = NULL;
        if ((TempStart = strchr((char *)&Temp_Buffer, '\"')) == NULL) {
            return;
        }
        strnew SteStrRTCData;
        SteStrRTCData.Name._char = TempStart;
        SteStrRTCData.MaxLen = ARR_SIZE(Temp_Buffer);
        setRtcDate(SteStrRTCData);
    }
    // 功能一: 修改网关标识
    if (LPUART1Ddata.RxBuf[2] == 0xDE) {// 修改网关标识
        Readlen = LPUART1Ddata.RxBuf[3] - 2;
        // 回复
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1) {
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
        }
#warning "测试"
        // 00 32 33 34 35 36 37 38 39 30 33
        copyString((char *)Temp_Buffer, (char *)&LPUART1Ddata.RxBuf[4], ARR_SIZE(Temp_Buffer), Readlen);
        swapStr((char *)Temp_Buffer, Readlen);
        if ((Readlen % 2) != 0) {
            Temp_Buffer[strlen((char *)Temp_Buffer)] = '0';
            Readlen++;
        }
        swapStr((char *)Temp_Buffer, Readlen);
        ASCIIToHEX2((char *)Temp_Buffer, Readlen, (char *)Temp_Buffer, (Readlen / 2));
        for (int i = 0; ((i < (Readlen / 2)) && (i < 6)); i++) {
            AT24CXX_Manager.gw_id[i] = Temp_Buffer[i];
        }
        EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.gw_id), 6);
        RTC_EC20_RST_Count = 180; //
    }
    // 功能一: 修改 IP and Prot
    if (LPUART1Ddata.RxBuf[2] == 0xDF) {
        // 192.168.4.1 8080 : 6848DF24312C302C22544350222C223139322E3136382E342E31222C383038302C302C320D0AFE16
        // hy1234 8080 : 6848DF1F312C302C22544350222C22687931323334222C383038302C302C320D0A7A16
        Temp_Buffer[0] = 'A'; // AT+QIOPEN
        Temp_Buffer[1] = 'T';
        Temp_Buffer[2] = '+';
        Temp_Buffer[3] = 'Q';
        Temp_Buffer[4] = 'I';
        Temp_Buffer[5] = 'O';
        Temp_Buffer[6] = 'P';
        Temp_Buffer[7] = 'E';
        Temp_Buffer[8] = 'N';
        Temp_Buffer[9] = '=';
        for (i = 0; i < 65; i++) {
            if ((LPUART1Ddata.RxBuf[i + 4] == 0x0D) && (LPUART1Ddata.RxBuf[i + 5] == 0x0A)) {
                Temp_Buffer[i + 10] = '\r';
                Temp_Buffer[i + 11] = '\n';
                Temp_Buffer[i + 12] = '\0';
                goto next8;
            } else {
                Temp_Buffer[i + 10] = LPUART1Ddata.RxBuf[i + 4];
            }
        }
    next8:
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);

        Readlen = LPUART1Ddata.RxBuf[3] - 2;
        char IPTemp[50] = {0};
        copyString((char *)IPTemp, (char *)&LPUART1Ddata.RxBuf[4], ARR_SIZE(IPTemp), Readlen);

        // 拷贝 IP or Url
        char * AddrStart = (myStrstr((char *)IPTemp, "\",\"", (ARR_SIZE(LPUART1Ddata.RxBuf) - 3)) + strlen("\",\""));
        char * AddrEnd = myStrstrCont((char *)IPTemp, "\",", (ARR_SIZE(LPUART1Ddata.RxBuf) - 3), 2);
        int IpOrProtLen = AddrEnd - AddrStart;
        copyString(AT24CXX_Manager.NET_4G_Remote_Url, AddrStart, ARR_SIZE(AT24CXX_Manager.NET_4G_Remote_Url), IpOrProtLen); // 59.110.170.225
        EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.NET_4G_Remote_Url), 30);

        // 拷贝 Prot
        AddrStart = myStrstrCont((char *)IPTemp, ",", (ARR_SIZE(LPUART1Ddata.RxBuf) - 3), 2) + 1;
        AddrEnd = myStrstr((char *)IPTemp, "\r\n", (ARR_SIZE(LPUART1Ddata.RxBuf) - 3));
        IpOrProtLen = AddrEnd - AddrStart;
        swapStr(AddrStart, IpOrProtLen);
        AT24CXX_Manager.NET_4G_Remote_Port = doneAsciiStrToAnyBaseNumberData(AddrStart, IpOrProtLen, 16);
        EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.NET_4G_Remote_Port), 2);
        RTC_EC20_RST_Count = 180; //
    }


    // 功能一：读取波特率 68 48 C1 05 03 90 1F 2816
    if ((LPUART1Ddata.RxBuf[2] == 0xC1) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) {
        Read_MBUS_Buad_Local_From_AT256_To_ARM();
        Read_RS4851_Buad_Local_From_AT256_To_ARM();
        Read_RS4852_Buad_Local_From_AT256_To_ARM();
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xC1;
        Len = 4;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        if (MBUS_UART1_BaudRate == 1200) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x04;
            LPUART1Ddata.TxBuf[Len++] = 0xB0;
        } else if (MBUS_UART1_BaudRate == 2400) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x09;
            LPUART1Ddata.TxBuf[Len++] = 0x60;
        } else if (MBUS_UART1_BaudRate == 4800) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x12;
            LPUART1Ddata.TxBuf[Len++] = 0xC0;
        } else if (MBUS_UART1_BaudRate == 9600) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x25;
            LPUART1Ddata.TxBuf[Len++] = 0x80;
        } else { // 读取2400
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x09;
            LPUART1Ddata.TxBuf[Len++] = 0x60;
        }
        if (MBUS_UART1_Parity == 0)
            LPUART1Ddata.TxBuf[Len++] = 0x00;
        else if (MBUS_UART1_Parity == 1)
            LPUART1Ddata.TxBuf[Len++] = 0x01;
        else if (MBUS_UART1_Parity == 2)
            LPUART1Ddata.TxBuf[Len++] = 0x02;
        LPUART1Ddata.TxBuf[Len++] = 0x08;
        LPUART1Ddata.TxBuf[Len++] = 0x01;

        if (RS4851_UART4_BaudRate == 1200) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x04;
            LPUART1Ddata.TxBuf[Len++] = 0xB0;
        } else if (RS4851_UART4_BaudRate == 2400) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x09;
            LPUART1Ddata.TxBuf[Len++] = 0x60;
        } else if (RS4851_UART4_BaudRate == 4800) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x12;
            LPUART1Ddata.TxBuf[Len++] = 0xC0;
        } else if (RS4851_UART4_BaudRate == 9600) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x25;
            LPUART1Ddata.TxBuf[Len++] = 0x80;
        } else { // 读取1200
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x04;
            LPUART1Ddata.TxBuf[Len++] = 0xB0;
        }
        if (RS4851_UART4_Parity == 0)
            LPUART1Ddata.TxBuf[Len++] = 0x00;
        else if (RS4851_UART4_Parity == 1)
            LPUART1Ddata.TxBuf[Len++] = 0x01;
        else if (RS4851_UART4_Parity == 2)
            LPUART1Ddata.TxBuf[Len++] = 0x02;
        LPUART1Ddata.TxBuf[Len++] = 0x08;
        LPUART1Ddata.TxBuf[Len++] = 0x01;

        if (RS4852_LPUART0_BaudRate == 1200) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x04;
            LPUART1Ddata.TxBuf[Len++] = 0xB0;
        } else if (RS4852_LPUART0_BaudRate == 2400) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x09;
            LPUART1Ddata.TxBuf[Len++] = 0x60;
        } else if (RS4852_LPUART0_BaudRate == 4800) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x12;
            LPUART1Ddata.TxBuf[Len++] = 0xC0;
        } else if (RS4852_LPUART0_BaudRate == 9600) {
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x25;
            LPUART1Ddata.TxBuf[Len++] = 0x80;
        } else { // 读取1200
            LPUART1Ddata.TxBuf[Len++] = 0x00;
            LPUART1Ddata.TxBuf[Len++] = 0x04;
            LPUART1Ddata.TxBuf[Len++] = 0xB0;
        }
        if (RS4852_LPUART0_Parity == 0)
            LPUART1Ddata.TxBuf[Len++] = 0x00;
        else if (RS4852_LPUART0_Parity == 1)
            LPUART1Ddata.TxBuf[Len++] = 0x01;
        else if (RS4852_LPUART0_Parity == 2)
            LPUART1Ddata.TxBuf[Len++] = 0x02;
        LPUART1Ddata.TxBuf[Len++] = 0x08;
        LPUART1Ddata.TxBuf[Len++] = 0x01;

        LPUART1Ddata.TxBuf[3] = Len - 4 + 2;
        cs = 0;
        for (i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }
    // 功能一：读取 MBUS 电流 68 48 C6 05 03 90 1F 2D16
    if ((LPUART1Ddata.RxBuf[2] == 0xC6) && (LPUART1Ddata.RxBuf[4] == 0x03) && (LPUART1Ddata.RxBuf[5] == 0x90) && (LPUART1Ddata.RxBuf[6] == 0x1F)) { // 读取MA
        LPUART1Ddata.TxBuf[Len++] = 0x68;
        LPUART1Ddata.TxBuf[Len++] = 0x48;
        LPUART1Ddata.TxBuf[Len++] = 0xC6;
        LPUART1Ddata.TxBuf[Len++] = 0x0F;
        LPUART1Ddata.TxBuf[Len++] = 0x03;
        LPUART1Ddata.TxBuf[Len++] = 0x90;
        LPUART1Ddata.TxBuf[Len++] = 0x1F;
        LPUART1Ddata.TxBuf[Len++] = 0x00;

        LPUART1Ddata.TxBuf[Len++] = (Current_MBUS_MA_Value % 0xFF0000) / 0xFF00;
        LPUART1Ddata.TxBuf[Len++] = (Current_MBUS_MA_Value % 0xFF00) / 0xFF;
        LPUART1Ddata.TxBuf[Len++] = Current_MBUS_MA_Value % 0xFF;

        LPUART1Ddata.TxBuf[Len++] = (Current_MBUS_MV_Value % 0xFF0000) / 0xFF00;
        LPUART1Ddata.TxBuf[Len++] = (Current_MBUS_MV_Value % 0xFF00) / 0xFF;
        LPUART1Ddata.TxBuf[Len++] = Current_MBUS_MV_Value % 0xFF;

        LPUART1Ddata.TxBuf[Len++] = (SOFT_VERSION % 0xFF0000) / 0xFF00;
        LPUART1Ddata.TxBuf[Len++] = (SOFT_VERSION % 0xFF00) / 0xFF;
        LPUART1Ddata.TxBuf[Len++] = SOFT_VERSION % 0xFF;
        cs = 0;
        for (i = 0; i < Len; i++) {
            cs = cs + LPUART1Ddata.TxBuf[i];
        }
        LPUART1Ddata.TxBuf[Len++] = cs;
        LPUART1Ddata.TxBuf[Len++] = 0x16;
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len);
    }


    // 功能三：MCU 重启
    if (LPUART1Ddata.RxBuf[2] == 0xDC) {
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // 回复OK
        Now_NetDevParameter.NowTCPLinkFlag = false;
        EC20_ReBoot_In_RTC_By_1Second(2);
    }
    // 功能二：USB 透传 MUBS： USB临时串口MBUS
    if (LPUART1Ddata.RxBuf[2] == 0xDB) {
        // current_MBUS_OFF_2S_Count=5;
        VBAT_29302_CTL_OFF;
        V33_PWR_CTL_OFF;
        MBUS_OUT_OFF;
        Select_OFF_EC20_To_UART0;
        Select_OFF_NET_To_UART0;

        Select_OFF_MBUS_To_UART1;
        Select_OFF_RS4851_To_UART4;
        Select_OFF_RS4852_To_LPUART0;
        Select_OFF_WIR_To_UART1;

        Select_OFF_CH340_To_LPUART1;
        Select_OFF_CH340_To_MBUS;
        Select_OFF_CH340_To_RS4851;
        Select_OFF_CH340_To_RS4852;

        Heart_LED_L;
        EC20_LED_L;
        NET_LED_L;
        CH340_To_LED_L;
        WIR_LED_L;
        RS485_LED_L;
        ERR_LED_L;

        Select_ON_CH340_To_MBUS;
        MBUS_OUT_ON;
        UP_Mode_EC20_ON = 0;
        UP_Mode_NET_ON = 0;
        UP_Mode_MBUSTOUSB_ON = 0;
        UP_Mode_WIR_ON = 0;
        DOWN_Mode_MBUS_ON = 1;
        DOWN_Mode_RS485_ON = 0;
        DOWN_Mode_WIR_ON = 0;
        DEFINE_Mode_SETUSB_ON = 0;

        CH340_To_LED_H;
    }
    // 功能一：修改固定波特率
    if (LPUART1Ddata.RxBuf[2] == 0xB1) {
        // 68 48 B1 0E 002580 02 08 01 002580 02 08 01 CF16
        for (i = 0; i < 18; i++) {
            Temp_Buffer[i] = LPUART1Ddata.RxBuf[i + 4];
        }
        LPUART1Ddata.TxBuf[Len++] = 'O';
        LPUART1Ddata.TxBuf[Len++] = 'K';
        if (DEFINE_Mode_SETUSB_ON == 1)
            LPUart1Send(LPUART1Ddata.TxBuf, Len); // 回复OK
        Set_MBUS_Buad_Local_From_Buffer_To_AT256(Temp_Buffer[0], Temp_Buffer[1], Temp_Buffer[2], Temp_Buffer[3]);
        Set_RS4851_Buad_Local_From_Buffer_To_AT256(Temp_Buffer[6], Temp_Buffer[7], Temp_Buffer[8], Temp_Buffer[9]);
        Set_RS4852_Buad_Local_From_Buffer_To_AT256(Temp_Buffer[12], Temp_Buffer[13], Temp_Buffer[14], Temp_Buffer[15]);
        RTC_NET_RST_Count = 180; // 如果没有点击复位  3分钟内复位
    }
    // 功能二：USB 透传 RS4851：USB临时串口RS4851
    if (LPUART1Ddata.RxBuf[2] == 0xB2) {
        // current_MBUS_OFF_2S_Count=5;
        VBAT_29302_CTL_OFF;
        V33_PWR_CTL_OFF;
        MBUS_OUT_OFF;
        Select_OFF_EC20_To_UART0;
        Select_OFF_NET_To_UART0;

        Select_OFF_MBUS_To_UART1;
        Select_OFF_RS4851_To_UART4;
        Select_OFF_RS4852_To_LPUART0;
        Select_OFF_WIR_To_UART1;

        Select_OFF_CH340_To_LPUART1;
        Select_OFF_CH340_To_MBUS;
        Select_OFF_CH340_To_RS4851;
        Select_OFF_CH340_To_RS4852;

        Heart_LED_L;
        EC20_LED_L;
        NET_LED_L;
        CH340_To_LED_L;
        WIR_LED_L;
        RS485_LED_L;
        ERR_LED_L;

        Select_ON_CH340_To_RS4851;
        UP_Mode_EC20_ON = 0;
        UP_Mode_NET_ON = 0;
        UP_Mode_MBUSTOUSB_ON = 0;
        UP_Mode_WIR_ON = 0;
        DOWN_Mode_MBUS_ON = 0;
        DOWN_Mode_RS485_ON = 1;
        DOWN_Mode_WIR_ON = 0;
        DEFINE_Mode_SETUSB_ON = 0;
        CH340_To_LED_H;
        RS485_LED_H;
    }
    // 功能二：USB 透传 RS4852： USB临时串口RS4852
    if (LPUART1Ddata.RxBuf[2] == 0xB3) {
        // current_MBUS_OFF_2S_Count=5;
        VBAT_29302_CTL_OFF;
        V33_PWR_CTL_OFF;
        MBUS_OUT_OFF;
        Select_OFF_EC20_To_UART0;
        Select_OFF_NET_To_UART0;

        Select_OFF_MBUS_To_UART1;
        Select_OFF_RS4851_To_UART4;
        Select_OFF_RS4852_To_LPUART0;
        Select_OFF_WIR_To_UART1;

        Select_OFF_CH340_To_LPUART1;
        Select_OFF_CH340_To_MBUS;
        Select_OFF_CH340_To_RS4851;
        Select_OFF_CH340_To_RS4852;

        Heart_LED_L;
        EC20_LED_L;
        NET_LED_L;
        CH340_To_LED_L;
        WIR_LED_L;
        RS485_LED_L;
        ERR_LED_L;

        Select_ON_CH340_To_RS4852;
        UP_Mode_EC20_ON = 0;
        UP_Mode_NET_ON = 0;
        UP_Mode_MBUSTOUSB_ON = 0;
        UP_Mode_WIR_ON = 0;
        DOWN_Mode_MBUS_ON = 0;
        DOWN_Mode_RS485_ON = 1;
        DOWN_Mode_WIR_ON = 0;
        DEFINE_Mode_SETUSB_ON = 0;
        CH340_To_LED_H;
        RS485_LED_H;
    }
}


