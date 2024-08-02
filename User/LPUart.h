#ifndef __LPUarT_H
#define __LPUarT_H
#include <stdint.h>

#define  LPUARTMAX  1024

typedef struct {
    unsigned char TxBuf[LPUARTMAX];
    uint16_t TxLen;
    unsigned char TxOpc;
    unsigned char RxBuf[LPUARTMAX];
    uint16_t RxLen;
    unsigned char RxOpc;
    unsigned char Rxdata;
}LPUARTOpTypeDef;

extern unsigned char CH340_LPUART1_RX_Count_Above30;

extern LPUARTOpTypeDef LPUART0Ddata, LPUART1Ddata;
extern unsigned char Cault_CS(unsigned char * Buffer, unsigned char start_num, unsigned char stop_num);
extern void LPUart0Send(unsigned char * txBuffer, unsigned short int length);
extern void LPUart1Send(unsigned char * txBuffer, unsigned short int length);
extern void Clear_LPUART0_Data(void);
extern void Clear_LPUART1_Data(void);
#endif 

