#include "main.h"
#include "WIFI_Interaction.h"
#include "NumberBaseLib.h"

bool IsAcceptWIFIData(char * UartBuff, int BuffLen) {
    // 判断当前串口接收到的最后的数据是否是 GET WG DATA;
    // 判断当前串口接收到的最后的数据是否是 SET END;
    if (UartBuff[BuffLen - 1] != ';') {
        return false;
    }
    if (strcmp(UartBuff + BuffLen - strlen("GET WG DATA;"), "GET WG DATA;") == 0) {
        return true;
    } else if (strcmp(UartBuff + BuffLen - strlen("SET END;"), "SET END;") == 0) {
        return true;
    } else if (strcmp(UartBuff + BuffLen - strlen("REBOOT WG;"), "REBOOT WG;") == 0) {
        EC20_ReBoot_In_RTC_By_1Second(2);
        return false;
    }
    return false;
}

char * getWGDataElement(char * OutStr, char * StartAddr) {
    char * Temp = NULL;
    if ((Temp = strchr(StartAddr, ';')) == NULL) {
        return StartAddr;
    }
    (*Temp) = 0;
    strcpy(OutStr, StartAddr);
    StartAddr = Temp + 1;
    (*Temp) = ';';
    return StartAddr;
}

void setWGDataByCmd(void) {
    char * NowAddr = NULL;
    NowAddr = strstr((char *)UART5Ddata.RxBuf, "SET WG DATA") + strlen("SET WG DATA");
    (*NowAddr) = 0;
    NowAddr++;
    /*模拟定义设备的外部参数, 后期修改 Data_Interaction.h 从外部引入参数*/
    struct _WGData {
        char gateway_id[40];
        char service_type[40];
        char server_address[40];
        char server_port[40];
        char mbus_baud_rate[40];
        char baud_rate_rs485_1[40];
        char baud_rate_rs485_2[40];
        char WorkMode[40];
    } WGData = {0};

    NowAddr = getWGDataElement(WGData.gateway_id, NowAddr);      // 00 32 33 34 35 36 37 38 39 30 33
    NowAddr = getWGDataElement(WGData.service_type, NowAddr);
    NowAddr = getWGDataElement(WGData.server_address, NowAddr);  // "hysmartmeter.com"
    NowAddr = getWGDataElement(WGData.server_port, NowAddr);     // 1883
    NowAddr = getWGDataElement(WGData.mbus_baud_rate, NowAddr);
    NowAddr = getWGDataElement(WGData.baud_rate_rs485_1, NowAddr);
    NowAddr = getWGDataElement(WGData.baud_rate_rs485_2, NowAddr);
    NowAddr = getWGDataElement(WGData.WorkMode, NowAddr);

    if (strcmp(WGData.gateway_id, "") != 0) {
    // 将数据保存到 AT24CXX_Manager.gw_id
        int Readlen = strlen((char *)WGData.gateway_id);
        swapStr((char *)WGData.gateway_id, Readlen);
        if ((Readlen % 2) != 0) {
            WGData.gateway_id[strlen((char *)WGData.gateway_id)] = '0';
            Readlen++;
        }
        swapStr((char *)WGData.gateway_id, Readlen);
        ASCIIToHEX2((char *)WGData.gateway_id, Readlen, (char *)WGData.gateway_id, (Readlen / 2));
        for (int i = 0; ((i < (Readlen / 2)) && (i < 6)); i++) {
            AT24CXX_Manager.gw_id[i] = WGData.gateway_id[i];
        }
        EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.gw_id), 6);
    }

    if (strcmp(WGData.server_address, "") != 0) {
        // 将数据保存到 AT24CXX_Manager.NET_4G_Remote_Url
        strcpy(AT24CXX_Manager.NET_4G_Remote_Url, (char *)WGData.server_address);
        EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.NET_4G_Remote_Url), 30);
    }
    if (strcmp(WGData.server_port, "") != 0) {
        // 将数据保存到 AT24CXX_Manager.NET_4G_Remote_Port
        AT24CXX_Manager.NET_4G_Remote_Port = doneAsciiStrToAnyBaseNumberData(WGData.server_port, strlen(WGData.server_port), 16);
        EEprom_JSON_Write((unsigned char *)(&AT24CXX_Manager.NET_4G_Remote_Port), 2);
    }
    char * StartTemp = NULL;
    char * EndeTemp = NULL;
    if (strcmp(WGData.mbus_baud_rate, "") != 0) {
        // mbus_baud_rate = "9600,E,8,1"
        StartTemp = WGData.mbus_baud_rate;
        EndeTemp = NULL;
        EndeTemp = strchr(StartTemp, ',');
        (*EndeTemp) = 0;
        MBUS_UART1_BaudRate = doneAsciiStrToAnyBaseNumberData(StartTemp, strlen(StartTemp), 16);
        StartTemp = EndeTemp + 1;
        EndeTemp = strchr(StartTemp, ',');
        (*EndeTemp) = 0;
        MBUS_UART1_Parity = ((*StartTemp) == 'N' ? 0 : ((*StartTemp) == '0' ? 1 : ((*StartTemp) == 'E' ? 2 : 0)));
        EEprom_Byte4Write(EEPROM_MBUS_UART1_BAUDRATE_ADDR, MBUS_UART1_BaudRate); // 2400偶校验
        EEprom_Byte1Write(EEPROM_MBUS_UART1_PARITY_ADDR, MBUS_UART1_Parity);
    }


    if (strcmp(WGData.baud_rate_rs485_1, "") != 0) {
        // baud_rate_rs485_1 = "9600,N,8,1"
        StartTemp = WGData.baud_rate_rs485_1;
        EndeTemp = NULL;
        EndeTemp = strchr(StartTemp, ',');
        (*EndeTemp) = 0;
        RS4851_UART4_BaudRate = doneAsciiStrToAnyBaseNumberData(StartTemp, strlen(StartTemp), 16);
        StartTemp = EndeTemp + 1;
        EndeTemp = strchr(StartTemp, ',');
        (*EndeTemp) = 0;
        RS4851_UART4_Parity = ((*StartTemp) == 'N' ? 0 : ((*StartTemp) == '0' ? 1 : ((*StartTemp) == 'E' ? 2 : 0)));
        EEprom_Byte4Write(EEPROM_RS4851_UART4_BAUDRATE_ADDR, RS4851_UART4_BaudRate);
        EEprom_Byte1Write(EEPROM_RS4851_UART4_PARITY_ADDR, RS4851_UART4_Parity);
    }


    if (strcmp(WGData.baud_rate_rs485_2, "") != 0) {
        // baud_rate_rs485_2 = "9600,N,8,1"
        StartTemp = WGData.baud_rate_rs485_2;
        EndeTemp = NULL;
        EndeTemp = strchr(StartTemp, ',');
        (*EndeTemp) = 0;
        RS4852_LPUART0_BaudRate = doneAsciiStrToAnyBaseNumberData(StartTemp, strlen(StartTemp), 16);
        StartTemp = EndeTemp + 1;
        EndeTemp = strchr(StartTemp, ',');
        (*EndeTemp) = 0;
        RS4852_LPUART0_Parity = ((*StartTemp) == 'N' ? 0 : ((*StartTemp) == '0' ? 1 : ((*StartTemp) == 'E' ? 2 : 0)));
        EEprom_Byte4Write(EEPROM_RS4852_LPUART0_BAUDRATE_ADDR, RS4852_LPUART0_BaudRate);
        EEprom_Byte1Write(EEPROM_RS4852_LPUART0_PARITY_ADDR, RS4852_LPUART0_Parity);
    }

    if (strcmp(WGData.WorkMode, "") != 0) {
        AT24CXX_Manager.Save_Working_Mode = doneAsciiStrToAnyBaseNumberData(WGData.WorkMode, strlen(WGData.WorkMode), 10);
        AT24CXX_Manager.Save_Working_Mode = AT24CXX_Manager.Save_Working_Mode | 0x01;   // 固定打开 USB config 位
        EEprom_JSON_Write(&AT24CXX_Manager.Save_Working_Mode, 1);
    }

}
char getBandModeParity(int Parity) {
    switch (Parity) {
        case 0:
            return 'N';
        case 1:
            return '0';
        case 2:
            return 'E';
        default:
            return 'N';
    }
}

bool WIFI_TASK(void) {
    char WG_DATA_STR[100] = {0};
    int Nowlen = 0;
    if (strstr((char *)UART5Ddata.RxBuf, "GET WG DATA;") != NULL) {
        memset(&UART5Ddata.RxBuf, 0x00, UARTMAX);
        UART5Ddata.RxLen = 0;
        sprintf(&WG_DATA_STR[Nowlen], "POST WG DATA;");
        Nowlen = strlen(WG_DATA_STR);
        char IDTemp[13] = {0};
        HEX2ToASCII((char *)AT24CXX_Manager.gw_id, 6, IDTemp, 12);
        sprintf(&WG_DATA_STR[Nowlen], "%s;", IDTemp);
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "%s;", "TCPMQTT");
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "%s;", AT24CXX_Manager.NET_4G_Remote_Url);
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "%d;", AT24CXX_Manager.NET_4G_Remote_Port);
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "%d,%c,8,1;", MBUS_UART1_BaudRate, getBandModeParity(MBUS_UART1_Parity));
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "%d,%c,8,1;", RS4851_UART4_BaudRate, getBandModeParity(RS4851_UART4_Parity));
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "%d,%c,8,1;", RS4852_LPUART0_BaudRate, getBandModeParity(RS4852_LPUART0_Parity));
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "%x;", AT24CXX_Manager.Save_Working_Mode | 0x01);
        Nowlen = strlen(WG_DATA_STR);
        sprintf(&WG_DATA_STR[Nowlen], "POST END;");
        Nowlen = strlen(WG_DATA_STR);
        printf("%s", WG_DATA_STR);
        return true;
    } else if (strstr((char *)UART5Ddata.RxBuf, "SET WG DATA;") != NULL) {
        if (strstr((char *)UART5Ddata.RxBuf, "SET END;") == NULL) {
            return false;
        }
        setWGDataByCmd();
        sprintf(&WG_DATA_STR[Nowlen], "%s;", "OK");
        Nowlen = strlen(WG_DATA_STR);
        return true;
    }
    return false;
}

