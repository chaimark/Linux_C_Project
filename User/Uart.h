#ifndef __UarT_H
#define __UarT_H

#define CONSOLE_UART UART5

#define UARTMAX   600
#define UART0_MAX   1024
#include <stdint.h>

typedef struct {
    unsigned char TxBuf[UARTMAX];
    uint16_t TxLen;
    unsigned char TxOpc;
    unsigned char RxBuf[UARTMAX];
    uint16_t RxLen;
    unsigned char RxOpc;
    unsigned char Rxdata;
}UARTOpStruct;

extern UARTOpStruct UART0Ddata, UART1Ddata, UART4Ddata, UART5Ddata;
extern uint8_t DOWN_Mode_RS485_ON;
extern int RTC_EC20_RST_Count;
extern int RTC_NET_RST_Count;
extern unsigned char timer_Send_Buff[50];
extern unsigned char CH340_UART4_RX_Count_Above30;
extern unsigned char UART1_get_Head_Flage;

extern void Send_Command_InImmediately_By_UART(unsigned char port, unsigned char * txBuffer, unsigned short int length);
extern void Uart0Send(unsigned char * txBuffer, unsigned short int length);
extern void Uart4Send(unsigned char * txBuffer, unsigned short int length);
extern void Uart5Send(unsigned char * txBuffer, unsigned short int length);
extern void Set_MBUS_Buad_Local_From_Buffer_To_AT256(const uint8_t data0, const uint8_t data1, const uint8_t data2, const uint8_t data3);
extern void Set_RS4851_Buad_Local_From_Buffer_To_AT256(const uint8_t data0, const uint8_t data1, const uint8_t data2, const uint8_t data3);
extern void Set_RS4852_Buad_Local_From_Buffer_To_AT256(const uint8_t data0, const uint8_t data1, const uint8_t data2, const uint8_t data3);
// extern void Set_CH340_Buad_Local_From_Buffer_To_AT256(const uint8_t data0, const uint8_t data1, const uint8_t data2, const uint8_t data3);
extern void Clear_RXCommand_By_UART(unsigned char port);
extern void Send_Command_InCopy_By_UART(unsigned char port);
extern void Clear_UART0_Data(void);
extern void Clear_UART1_Data(void);
extern void Clear_UART4_Data(void);
extern void Clear_UART5_Data(void);
extern void Clear_LPUART0_Data(void);
extern void Send_Command_InMain_By_UART(unsigned char port);
extern void Adjust_Meter_By_MBUS_Uart1(void);

#endif 

