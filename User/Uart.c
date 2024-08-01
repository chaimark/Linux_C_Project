#include "main.h"
#include <stdbool.h>
#include "Uart.h"
#include "Define.h"
#include "AT24C0256.h"
#include "ImmediatelyReading.h"
#include "TimerMainReading.h"
#include "TimerCopyReading.h"
#include "RTC.h"
#include "LPUart.h"
#include "WIFI_Interaction.h"

// 实现 printf
#pragma import(__use_no_semihosting)
struct __FILE {
    int handle;
};
FILE __stdout;

void _sys_exit(int x) {
    x = x;
}

int fputc(int ch, FILE * f) {
    CONSOLE_UART->TXBUF = ch;
    while (!(CONSOLE_UART->ISR & 0x01));
    return (ch);
}


UARTOpStruct UART0Ddata, UART1Ddata, UART4Ddata, UART5Ddata;

uint8_t DOWN_Mode_RS485_ON = 1;
int RTC_EC20_RST_Count = 0;
int RTC_NET_RST_Count = 0;

unsigned char timer_Send_Buff[50];

unsigned char CH340_UART4_RX_Count_Above30;//

// 接收
static void uart_receive_u8(UARTOpStruct * uart, unsigned char tmp) {
    if (uart->RxLen < UARTMAX)
        uart->RxBuf[uart->RxLen++] = tmp;
    else
        uart->RxLen = 0;
}
// 发送
static void Uart_Send(UART_Type * UARTx, unsigned char * buf, unsigned short int len) {
    for (int i = 0; i < len; i++) {
        while (!FL_UART_IsActiveFlag_TXBuffEmpty(UARTx));
        FL_UART_WriteTXBuff(UARTx, buf[i]);
    }
    while (!FL_UART_IsActiveFlag_TXShiftBuffEmpty(UARTx));
}

// 0发送 // 4G NET
void Uart0Send(unsigned char * txBuffer, unsigned short int length) {
    Uart_Send(UART0, txBuffer, length);
}
// 1发送 // MBUS
void Uart1Send(unsigned char * txBuffer, unsigned short int length) {
    Uart_Send(UART1, txBuffer, length);
}
// 4发送 // RS485_1
void Uart4Send(unsigned char * txBuffer, unsigned short int length) {
    Uart_Send(UART4, txBuffer, length);
}
// 5发送 // WIFI
void Uart5Send(unsigned char * txBuffer, unsigned short int length) {
    Uart_Send(UART5, txBuffer, length);
}

// 清理0 // 4G NET
void Clear_UART0_Data(void) {
    memset(&UART0Ddata.RxBuf, 0x00, UARTMAX);
}
// 清理1 // MBUS
void Clear_UART1_Data(void) {
    memset(&UART1Ddata.RxBuf, 0x00, UARTMAX);
    UART1Ddata.RxLen = 0;
}

// 清理4 // RS485_1
void Clear_UART4_Data(void) {
    memset(&UART4Ddata.RxBuf, 0x00, UARTMAX);
    UART4Ddata.RxLen = 0;
}
// 清理5 // WIFI
void Clear_UART5_Data(void) {
    memset(&UART5Ddata.RxBuf, 0x00, UARTMAX);
    UART5Ddata.RxLen = 0;
}

// 保存1
void SAVE_UART1_Data(void) {
    UART1Ddata.RxBuf[UART1Ddata.RxLen++] = UART1Ddata.Rxdata;
}
// 保存4
void SAVE_UART4_Data(void) {
    UART4Ddata.RxBuf[UART4Ddata.RxLen++] = UART4Ddata.Rxdata;
}

void Clear_RXCommand_By_UART(unsigned char port) {
    Clear_UART1_Data();
    Clear_UART4_Data();
    Clear_LPUART0_Data();
}

void adjust_UART1_Head(void) {
    if (UART1Ddata.Rxdata == 0x68) {
        UART1Ddata.RxBuf[0] = UART1Ddata.Rxdata;
        UART1Ddata.RxLen = 1;
    } else {
        if ((UART1Ddata.RxBuf[0] == 0x68) && ((UART1Ddata.Rxdata == 0x20) || (UART1Ddata.Rxdata == 0x47) || (UART1Ddata.Rxdata == 0x53) || (UART1Ddata.Rxdata == 0x55) || (UART1Ddata.Rxdata == 0x56) || (UART1Ddata.Rxdata == 0x57))) {
            UART1Ddata.RxBuf[1] = UART1Ddata.Rxdata;
            UART1Ddata.RxLen = 2;
        } else {
            UART1Ddata.RxLen = 0;
        }
    }
}

void UART1_ReadData_Deal(void) {
    if (Cault_CS(UART1Ddata.RxBuf, 0, (UART1Ddata.RxLen - 2)) == UART1Ddata.RxBuf[UART1Ddata.RxLen - 2]) {
        if (read_Main_Meter_In_Timer_In_Main_Flage != 0) {
            mainTimerRead.readMeter_Up_Len = UART1Ddata.RxLen;
            memcpy(mainTimerRead.readMeter_RxBuf, UART1Ddata.RxBuf, UART1Ddata.RxLen);
            mainTimerRead.readMeter_OK_Flage = 1;
        }
        if (read_Copy_Meter_In_Timer_In_Main_Flage != 0) {
            copyTimerRead.readMeter_Up_Len = UART1Ddata.RxLen;
            memcpy(copyTimerRead.readMeter_RxBuf, UART1Ddata.RxBuf, UART1Ddata.RxLen);
            copyTimerRead.readMeter_OK_Flage = 1;
        }
        if (imm_Read.immediately_Exec_Flage != 0) {
            imm_Read.immediately_Up_Len = UART1Ddata.RxLen;
            memcpy(imm_Read.immediately_RXBuffer, UART1Ddata.RxBuf, UART1Ddata.RxLen);
            imm_Read.immediately_OK_Flage = 1;
        }
    }
}
// 发送抄表命令
void Send_Command_InMain_By_UART(unsigned char port) {
    unsigned char i;
    unsigned int pro;
    pro = ((mainTimerRead.readMeter_Current_MeterID[4] / 0x10) * 10 + (mainTimerRead.readMeter_Current_MeterID[4] % 0x10)) * 100 + (mainTimerRead.readMeter_Current_MeterID[5] / 0x10) * 10 + (mainTimerRead.readMeter_Current_MeterID[5] % 0x10);
    if ((pro == 5011) || (pro == 5012)) {
        timer_Send_Buff[0] = 0x68;
        timer_Send_Buff[1] = 0x0B;
        timer_Send_Buff[2] = 0x0B;
        timer_Send_Buff[3] = 0x68;
        timer_Send_Buff[4] = 0x73;
        timer_Send_Buff[5] = 0xFD;
        timer_Send_Buff[6] = 0x52;
        timer_Send_Buff[7] = mainTimerRead.readMeter_Current_MeterID[3];
        timer_Send_Buff[8] = mainTimerRead.readMeter_Current_MeterID[2];
        timer_Send_Buff[9] = mainTimerRead.readMeter_Current_MeterID[1];
        timer_Send_Buff[10] = mainTimerRead.readMeter_Current_MeterID[0];
        timer_Send_Buff[11] = 0xFF;
        timer_Send_Buff[12] = 0xFF;
        timer_Send_Buff[13] = 0xFF;
        timer_Send_Buff[14] = 0xFF;
        timer_Send_Buff[18] = Cault_CS(timer_Send_Buff, 4, 18);
        timer_Send_Buff[19] = 0x16;
    } else {
        timer_Send_Buff[0] = 0xFE;
        timer_Send_Buff[1] = 0xFE;
        timer_Send_Buff[2] = 0xFE;
        timer_Send_Buff[3] = 0xFE;
        timer_Send_Buff[4] = 0x68;
        if (pro == 1001) {
            timer_Send_Buff[5] = 0x20;
        } else if (pro == 1011) {
            timer_Send_Buff[5] = 0x20;
        } else if (pro == 2006) {
            timer_Send_Buff[5] = 0x47;
        } else if (pro == 2007) {
            timer_Send_Buff[5] = 0x47;
        } else if (pro == 2008) {
            timer_Send_Buff[5] = 0x53;
        } else if (pro == 2034) {
            timer_Send_Buff[5] = 0x55;
        } else if (pro == 2041) {
            timer_Send_Buff[5] = 0x56;
        } else if (pro == 2051) {
            timer_Send_Buff[5] = 0x57;
        } else {
            timer_Send_Buff[5] = 0x20;
        }
        if (pro == 1011) {
            for (i = 0; i < 4; i++)
                timer_Send_Buff[6 + i] = mainTimerRead.readMeter_Current_MeterID[4 - i];
        } else {
            for (i = 0; i < 4; i++)
                timer_Send_Buff[6 + i] = mainTimerRead.readMeter_Current_MeterID[i];
        }
        if (pro == 1011) {
            timer_Send_Buff[10] = 0x66;
            timer_Send_Buff[11] = 0x66;
            timer_Send_Buff[12] = 0x66;
        } else {
            timer_Send_Buff[10] = 0x00;
            timer_Send_Buff[11] = 0x11;
            timer_Send_Buff[12] = 0x11;
        }
        timer_Send_Buff[13] = 0x01;
        timer_Send_Buff[14] = 0x03;
        timer_Send_Buff[15] = 0x90;
        timer_Send_Buff[16] = 0x1F;
        timer_Send_Buff[17] = 0x00;
        timer_Send_Buff[18] = Cault_CS(timer_Send_Buff, 4, 18);
        timer_Send_Buff[19] = 0x16;
    }

    mainTimerRead.readMeter_Up_Len = 0;
    if (port == 0x00) {
        Clear_UART1_Data();
        Uart1Send(timer_Send_Buff, 20);
    } else if (port == 0x01) {
        Clear_UART4_Data();
        Uart4Send(timer_Send_Buff, 20);
    } else if (port == 0x02) {
        Clear_LPUART0_Data();
        LPUart0Send(timer_Send_Buff, 20);
    } else {
        Clear_UART1_Data();
        Uart1Send(timer_Send_Buff, 20);
    }
}
// 发送抄表命令
void Send_Command_InCopy_By_UART(unsigned char port) {
    unsigned char i;
    unsigned int pro;
    pro = ((copyTimerRead.readMeter_Current_MeterID[4] / 0x10) * 10 + (copyTimerRead.readMeter_Current_MeterID[4] % 0x10)) * 100 + (copyTimerRead.readMeter_Current_MeterID[5] / 0x10) * 10 + (copyTimerRead.readMeter_Current_MeterID[5] % 0x10);
    if ((pro == 5011) || (pro == 5012)) {
        timer_Send_Buff[0] = 0x68;
        timer_Send_Buff[1] = 0x0B;
        timer_Send_Buff[2] = 0x0B;
        timer_Send_Buff[3] = 0x68;
        timer_Send_Buff[4] = 0x73;
        timer_Send_Buff[5] = 0xFD;
        timer_Send_Buff[6] = 0x52;
        timer_Send_Buff[7] = copyTimerRead.readMeter_Current_MeterID[3];
        timer_Send_Buff[8] = copyTimerRead.readMeter_Current_MeterID[2];
        timer_Send_Buff[9] = copyTimerRead.readMeter_Current_MeterID[1];
        timer_Send_Buff[10] = copyTimerRead.readMeter_Current_MeterID[0];
        timer_Send_Buff[11] = 0xFF;
        timer_Send_Buff[12] = 0xFF;
        timer_Send_Buff[13] = 0xFF;
        timer_Send_Buff[14] = 0xFF;
        timer_Send_Buff[18] = Cault_CS(timer_Send_Buff, 4, 18);
        timer_Send_Buff[19] = 0x16;
    } else {
        timer_Send_Buff[0] = 0xFE;
        timer_Send_Buff[1] = 0xFE;
        timer_Send_Buff[2] = 0xFE;
        timer_Send_Buff[3] = 0xFE;
        timer_Send_Buff[4] = 0x68;
        if (pro == 1001) {
            timer_Send_Buff[5] = 0x20;
        } else if (pro == 1011) {
            timer_Send_Buff[5] = 0x20;
        } else if (pro == 2006) {
            timer_Send_Buff[5] = 0x47;
        } else if (pro == 2007) {
            timer_Send_Buff[5] = 0x47;
        } else if (pro == 2008) {
            timer_Send_Buff[5] = 0x53;
        } else if (pro == 2034) {
            timer_Send_Buff[5] = 0x55;
        } else if (pro == 2041) {
            timer_Send_Buff[5] = 0x56;
        } else if (pro == 2051) {
            timer_Send_Buff[5] = 0x57;
        } else {
            timer_Send_Buff[5] = 0x20;
        }
        if (pro == 1011) {
            for (i = 0; i < 4; i++)
                timer_Send_Buff[6 + i] = copyTimerRead.readMeter_Current_MeterID[4 - i];
        } else {
            for (i = 0; i < 4; i++)
                timer_Send_Buff[6 + i] = copyTimerRead.readMeter_Current_MeterID[i];
        }
        if (pro == 1011) {
            timer_Send_Buff[10] = 0x66;
            timer_Send_Buff[11] = 0x66;
            timer_Send_Buff[12] = 0x66;
        } else {
            timer_Send_Buff[10] = 0x00;
            timer_Send_Buff[11] = 0x11;
            timer_Send_Buff[12] = 0x11;
        }
        timer_Send_Buff[13] = 0x01;
        timer_Send_Buff[14] = 0x03;
        timer_Send_Buff[15] = 0x90;
        timer_Send_Buff[16] = 0x1F;
        timer_Send_Buff[17] = 0x00;
        timer_Send_Buff[18] = Cault_CS(timer_Send_Buff, 4, 18);
        timer_Send_Buff[19] = 0x16;
    }

    copyTimerRead.readMeter_Up_Len = 0;
    if (port == 0x00) {
        Clear_UART1_Data();
        Uart1Send(timer_Send_Buff, 20);
    } else if (port == 0x01) {
        Clear_UART4_Data();
        Uart4Send(timer_Send_Buff, 20);
    } else if (port == 0x02) {
        Clear_LPUART0_Data();
        LPUart0Send(timer_Send_Buff, 20);
    } else {
        Clear_UART1_Data();
        Uart1Send(timer_Send_Buff, 20);
    }
}

void Adjust_Meter_By_MBUS_Uart1(void) {
    unsigned char i;
    Select_OFF_CH340_To_MBUS;
    MBUS_OUT_ON;
    for (i = 0; i < 8; i++) {
        FL_RTC_GetTime(RTC, &RTC_Data);
        timer_Send_Buff[0] = 0xFE;
        timer_Send_Buff[1] = 0xFE;
        timer_Send_Buff[2] = 0xFE;
        timer_Send_Buff[3] = 0xFE;
        timer_Send_Buff[4] = 0x68;
        timer_Send_Buff[5] = 0x20;
        timer_Send_Buff[5] = 0xFF;
        timer_Send_Buff[5] = 0xFF;
        timer_Send_Buff[5] = 0xFF;
        timer_Send_Buff[5] = 0xFF;
        timer_Send_Buff[10] = 0x00;
        timer_Send_Buff[11] = 0x11;
        timer_Send_Buff[12] = 0x11;
        timer_Send_Buff[13] = 0x04;
        timer_Send_Buff[14] = 0x0A;
        timer_Send_Buff[15] = 0xA0;
        timer_Send_Buff[16] = 0x15;
        timer_Send_Buff[17] = 0x00;
        timer_Send_Buff[18] = RTC_Data.second;
        timer_Send_Buff[19] = RTC_Data.minute;
        timer_Send_Buff[20] = RTC_Data.hour;
        timer_Send_Buff[21] = RTC_Data.day;
        timer_Send_Buff[22] = RTC_Data.month;
        timer_Send_Buff[23] = RTC_Data.year;
        timer_Send_Buff[24] = 0x20;
        timer_Send_Buff[25] = Cault_CS(timer_Send_Buff, 4, 25);
        timer_Send_Buff[26] = 0x16;
        mainTimerRead.readMeter_Up_Len = 0;
        Clear_UART1_Data();
        Uart1Send(timer_Send_Buff, 26);
        FL_DelayMs(1000);
    }
}

void Send_Command_InImmediately_By_UART(unsigned char port, unsigned char * txBuffer, unsigned short int length) {
    if (length < 30) {
        if (port == 0x00) {
            Clear_UART1_Data();
            Uart1Send(txBuffer, length);
        } else if (port == 0x01) {
            Clear_UART4_Data();
            Uart4Send(txBuffer, length);
        } else if (port == 0x02) {
            Clear_LPUART0_Data();
            LPUart0Send(txBuffer, length);
        } else {
            Clear_UART1_Data();
            Uart1Send(txBuffer, length);
        }
    }
}


void Set_MBUS_Buad_Local_From_Buffer_To_AT256(const uint8_t data0, const uint8_t data1, const uint8_t data2, const uint8_t data3) {
    if ((data0 == 0x00) && (data1 == 0x04) && (data2 == 0xB0)) {
        MBUS_UART1_BaudRate = 1200;
        EEprom_Byte4Write(EEPROM_MBUS_UART1_BAUDRATE_ADDR, MBUS_UART1_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x09) && (data2 == 0x60)) {
        MBUS_UART1_BaudRate = 2400;
        EEprom_Byte4Write(EEPROM_MBUS_UART1_BAUDRATE_ADDR, MBUS_UART1_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x12) && (data2 == 0xC0)) {
        MBUS_UART1_BaudRate = 4800;
        EEprom_Byte4Write(EEPROM_MBUS_UART1_BAUDRATE_ADDR, MBUS_UART1_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x25) && (data2 == 0x80)) {
        MBUS_UART1_BaudRate = 9600;
        EEprom_Byte4Write(EEPROM_MBUS_UART1_BAUDRATE_ADDR, MBUS_UART1_BaudRate);
    } else {
        MBUS_UART1_BaudRate = 2400;
        EEprom_Byte4Write(EEPROM_MBUS_UART1_BAUDRATE_ADDR, MBUS_UART1_BaudRate);
    }

    if (data3 == 0x00) {
        MBUS_UART1_Parity = 0;
        EEprom_Byte1Write(EEPROM_MBUS_UART1_PARITY_ADDR, MBUS_UART1_Parity);
    } else if (data3 == 0x01) {
        MBUS_UART1_Parity = 1;
        EEprom_Byte1Write(EEPROM_MBUS_UART1_PARITY_ADDR, MBUS_UART1_Parity);
    } else if (data3 == 0x02) {
        MBUS_UART1_Parity = 2;
        EEprom_Byte1Write(EEPROM_MBUS_UART1_PARITY_ADDR, MBUS_UART1_Parity);
    } else {
        MBUS_UART1_Parity = 2;
        EEprom_Byte1Write(EEPROM_MBUS_UART1_PARITY_ADDR, MBUS_UART1_Parity);
    }
}

void Set_RS4851_Buad_Local_From_Buffer_To_AT256(const uint8_t data0, const uint8_t data1, const uint8_t data2, const uint8_t data3) {
    if ((data0 == 0x00) && (data1 == 0x04) && (data2 == 0xB0)) {
        RS4851_UART4_BaudRate = 1200;
        EEprom_Byte4Write(EEPROM_RS4851_UART4_BAUDRATE_ADDR, RS4851_UART4_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x09) && (data2 == 0x60)) {
        RS4851_UART4_BaudRate = 2400;
        EEprom_Byte4Write(EEPROM_RS4851_UART4_BAUDRATE_ADDR, RS4851_UART4_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x12) && (data2 == 0xC0)) {
        RS4851_UART4_BaudRate = 4800;
        EEprom_Byte4Write(EEPROM_RS4851_UART4_BAUDRATE_ADDR, RS4851_UART4_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x25) && (data2 == 0x80)) {
        RS4851_UART4_BaudRate = 9600;
        EEprom_Byte4Write(EEPROM_RS4851_UART4_BAUDRATE_ADDR, RS4851_UART4_BaudRate);
    } else {
        RS4851_UART4_BaudRate = 1200;
        EEprom_Byte4Write(EEPROM_RS4851_UART4_BAUDRATE_ADDR, RS4851_UART4_BaudRate);
    }

    if (data3 == 0x00) {
        RS4851_UART4_Parity = 0;
        EEprom_Byte1Write(EEPROM_RS4851_UART4_PARITY_ADDR, RS4851_UART4_Parity);
    } else if (data3 == 0x01) {
        RS4851_UART4_Parity = 1;
        EEprom_Byte1Write(EEPROM_RS4851_UART4_PARITY_ADDR, RS4851_UART4_Parity);
    } else if (data3 == 0x02) {
        RS4851_UART4_Parity = 2;
        EEprom_Byte1Write(EEPROM_RS4851_UART4_PARITY_ADDR, RS4851_UART4_Parity);
    } else {
        RS4851_UART4_Parity = 2;
        EEprom_Byte1Write(EEPROM_RS4851_UART4_PARITY_ADDR, RS4851_UART4_Parity);
    }
}

void Set_RS4852_Buad_Local_From_Buffer_To_AT256(const uint8_t data0, const uint8_t data1, const uint8_t data2, const uint8_t data3) {
    if ((data0 == 0x00) && (data1 == 0x04) && (data2 == 0xB0)) {
        RS4852_LPUART0_BaudRate = 1200;
        EEprom_Byte4Write(EEPROM_RS4852_LPUART0_BAUDRATE_ADDR, RS4852_LPUART0_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x09) && (data2 == 0x60)) {
        RS4852_LPUART0_BaudRate = 2400;
        EEprom_Byte4Write(EEPROM_RS4852_LPUART0_BAUDRATE_ADDR, RS4852_LPUART0_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x12) && (data2 == 0xC0)) {
        RS4852_LPUART0_BaudRate = 4800;
        EEprom_Byte4Write(EEPROM_RS4852_LPUART0_BAUDRATE_ADDR, RS4852_LPUART0_BaudRate);
    } else if ((data0 == 0x00) && (data1 == 0x25) && (data2 == 0x80)) {
        RS4852_LPUART0_BaudRate = 9600;
        EEprom_Byte4Write(EEPROM_RS4852_LPUART0_BAUDRATE_ADDR, RS4852_LPUART0_BaudRate);
    } else {
        RS4852_LPUART0_BaudRate = 1200;
        EEprom_Byte4Write(EEPROM_RS4852_LPUART0_BAUDRATE_ADDR, RS4852_LPUART0_BaudRate);
    }

    if (data3 == 0x00) {
        RS4852_LPUART0_Parity = 0;
        EEprom_Byte1Write(EEPROM_RS4852_LPUART0_PARITY_ADDR, RS4852_LPUART0_Parity);
    } else if (data3 == 0x01) {
        RS4852_LPUART0_Parity = 1;
        EEprom_Byte1Write(EEPROM_RS4852_LPUART0_PARITY_ADDR, RS4852_LPUART0_Parity);
    } else if (data3 == 0x02) {
        RS4852_LPUART0_Parity = 2;
        EEprom_Byte1Write(EEPROM_RS4852_LPUART0_PARITY_ADDR, RS4852_LPUART0_Parity);
    } else {
        RS4852_LPUART0_Parity = 2;
        EEprom_Byte1Write(EEPROM_RS4852_LPUART0_PARITY_ADDR, RS4852_LPUART0_Parity);
    }
}
// 4G NET
void UART0_IRQHandler(void) {
    unsigned char tmp;
    if (FL_UART_IsActiveFlag_RXBuffFull(UART0)) {
        tmp = FL_UART_ReadRXBuff(UART0);
        uart_receive_u8(&UART0Ddata, tmp);
    }
}

// MBUS
void UART1_IRQHandler(void) {
    // 接收中断处理
    if ((FL_ENABLE == FL_UART_IsEnabledIT_RXBuffFull(UART1)) && (FL_SET == FL_UART_IsActiveFlag_RXBuffFull(UART1))) {
        UART1Ddata.Rxdata = FL_UART_ReadRXBuff(UART1);
        execute_MBUS_Reboot_1Min_Count = 0;
        // 中断转发接收到的数据
        if ((UART1Ddata.Rxdata == 0x68) || (UART1Ddata.RxBuf[0] == 0x68)) {
            if (UART1Ddata.RxLen == 0)
                adjust_UART1_Head();
            else {
                if (UART1Ddata.RxLen < 12)
                    SAVE_UART1_Data();
                else {
                    if (UART1Ddata.RxLen >= 80) { // 超过最大70
                        Clear_UART1_Data();
                        return;
                    }
                    if (UART1Ddata.RxLen <= UART1Ddata.RxBuf[10] + 11) {
                        SAVE_UART1_Data();
                    } else {
                        if (UART1Ddata.Rxdata == 0x16) {
                            SAVE_UART1_Data();
                            UART1_ReadData_Deal();
                            Clear_UART1_Data();
                        } else {
                            SAVE_UART1_Data();
                            if (UART1Ddata.RxLen > 80) {
                                UART1Ddata.RxLen = 0;
                                Clear_UART1_Data();
                            }
                        }
                    }
                }
            }
        }
    }
}




// RS485_1
void adjust_UART4_Head(void) {
    if (UART4Ddata.Rxdata == 0x68) {
        UART4Ddata.RxBuf[0] = UART4Ddata.Rxdata;
        UART4Ddata.RxLen = 1;
    } else {
        if ((UART4Ddata.RxBuf[0] == 0x68) && ((UART4Ddata.Rxdata == 0x20) || (UART4Ddata.Rxdata == 0x47) || (UART4Ddata.Rxdata == 0x53) || (UART4Ddata.Rxdata == 0x55) || (UART4Ddata.Rxdata == 0x56) || (UART4Ddata.Rxdata == 0x57))) {
            UART4Ddata.RxBuf[1] = UART4Ddata.Rxdata;
            UART4Ddata.RxLen = 2;
        } else {
            UART4Ddata.RxLen = 0;
        }
    }
}

void UART4_ReadData_Deal(void) {
    if (Cault_CS(UART1Ddata.RxBuf, 0, (UART1Ddata.RxLen - 2)) == UART1Ddata.RxBuf[UART1Ddata.RxLen - 2]) {
        if (read_Main_Meter_In_Timer_In_Main_Flage != 0) {
            mainTimerRead.readMeter_Up_Len = UART1Ddata.RxLen;
            memcpy(mainTimerRead.readMeter_RxBuf, UART1Ddata.RxBuf, UART1Ddata.RxLen);
            mainTimerRead.readMeter_OK_Flage = 1;
        }
        if (read_Copy_Meter_In_Timer_In_Main_Flage != 0) {
            copyTimerRead.readMeter_Up_Len = UART1Ddata.RxLen;
            memcpy(copyTimerRead.readMeter_RxBuf, UART1Ddata.RxBuf, UART1Ddata.RxLen);
            copyTimerRead.readMeter_OK_Flage = 1;
        }
        if (imm_Read.immediately_Exec_Flage != 0) {
            imm_Read.immediately_Up_Len = UART1Ddata.RxLen;
            memcpy(imm_Read.immediately_RXBuffer, UART1Ddata.RxBuf, UART1Ddata.RxLen);
            imm_Read.immediately_OK_Flage = 1;
        }
    }
}

void UART4_IRQHandler(void) {
    if (FL_UART_IsActiveFlag_RXBuffFull(UART4)) {
        UART4Ddata.Rxdata = FL_UART_ReadRXBuff(UART4);
        if ((UART4Ddata.Rxdata == 0x68) || (UART4Ddata.RxBuf[0] == 0x68)) {
            if (UART4Ddata.RxLen == 0) adjust_UART4_Head();
            else {
                if (UART4Ddata.RxLen < 5) SAVE_UART4_Data();
                else {
                    if (UART4Ddata.RxLen >= 30) { // 超过最大30
                        Clear_UART4_Data();
                        return;
                    }
                    if (UART4Ddata.RxLen <= UART4Ddata.RxBuf[3] + 2)SAVE_UART4_Data();
                    else {
                        if (UART4Ddata.Rxdata == 0x16) {
                            SAVE_UART4_Data();
                            UART4_ReadData_Deal();
                            Clear_UART4_Data();
                        } else {
                            SAVE_UART4_Data();
                            if (UART4Ddata.RxLen > 30) {
                                Clear_UART4_Data();
                            }
                        }
                    }
                }
            }
        }
    }
}

// WIFI
void UART5_IRQHandler(void) {
    unsigned char tmp;
    if (FL_UART_IsActiveFlag_RXBuffFull(UART5)) {
        tmp = FL_UART_ReadRXBuff(UART5);
        uart_receive_u8(&UART5Ddata, tmp);
    }
    if (IsAcceptWIFIData((char *)UART5Ddata.RxBuf, (int)UART5Ddata.RxLen)) {
        if (WIFI_TASK()) {
            Clear_UART5_Data();
        }
    }
}
