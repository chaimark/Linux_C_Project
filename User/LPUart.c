#include "LPUart.h"
#include "TimerCopyReading.h"
#include "main.h"
#include "CH340_LPUart1.h"

LPUARTOpTypeDef LPUART0Ddata, LPUART1Ddata;
unsigned char CH340_LPUART1_RX_Count_Above30;
// 校验计算
unsigned char Cault_CS(unsigned char * Buffer, unsigned char start_num, unsigned char stop_num) {
    unsigned char temp, cs;
    cs = 0;
    for (temp = start_num; temp < stop_num; temp++) {
        cs = cs + *(Buffer + temp);
    }
    return cs;
}

static void LPUart_Send(LPUART_Type * LPUARTx, unsigned char * buf, unsigned short int len) {
    for (int i = 0; i < len; i++) {
        while (!FL_LPUART_IsActiveFlag_TXBuffEmpty(LPUARTx))
            ;
        FL_LPUART_WriteTXBuff(LPUARTx, buf[i]);
    }
    while (!FL_LPUART_IsActiveFlag_TXShiftBuffEmpty(LPUARTx))
        ;
}

void LPUart0Send(unsigned char * txBuffer, unsigned short int length) {
    LPUart_Send(LPUART0, txBuffer, length);
}

void LPUart1Send(unsigned char * txBuffer, unsigned short int length) {
    LPUart_Send(LPUART1, txBuffer, length);
}

void Clear_LPUART0_Data(void) {
    memset(&LPUART0Ddata.RxBuf, 0x00, LPUARTMAX);
    LPUART0Ddata.RxLen = 0;
}

void SAVE_LPUART0_Data(void) {
    LPUART0Ddata.RxBuf[LPUART0Ddata.RxLen++] = LPUART0Ddata.Rxdata;
}

// CH340
void Clear_LPUART1_Data(void) {
    memset(&LPUART1Ddata.RxBuf, 0x00, LPUARTMAX);
    LPUART1Ddata.RxLen = 0;
}

void SAVE_LPUART1_Data(void) {
    LPUART1Ddata.RxBuf[LPUART1Ddata.RxLen++] = LPUART1Ddata.Rxdata;
}

void adjust_LPUART0_Head(void) {
    if (LPUART0Ddata.Rxdata == 0x68) {
        LPUART0Ddata.RxBuf[0] = LPUART0Ddata.Rxdata;
        LPUART0Ddata.RxLen = 1;
    } else {
        if ((LPUART0Ddata.RxBuf[0] == 0x68) && ((LPUART0Ddata.Rxdata == 0x20) || (LPUART0Ddata.Rxdata == 0x47) || (LPUART0Ddata.Rxdata == 0x53) || (LPUART0Ddata.Rxdata == 0x55) || (LPUART0Ddata.Rxdata == 0x56) || (LPUART0Ddata.Rxdata == 0x57))) {
            LPUART0Ddata.RxBuf[1] = LPUART0Ddata.Rxdata;
            LPUART0Ddata.RxLen = 2;
        } else {
            LPUART0Ddata.RxLen = 0;
        }
    }
}

void LPUART0_ReadData_Deal(void) {
    if (Cault_CS(LPUART0Ddata.RxBuf, 0, (LPUART0Ddata.RxLen - 2)) == LPUART0Ddata.RxBuf[LPUART0Ddata.RxLen - 2]) {
        if (read_Main_Meter_In_Timer_In_Main_Flage != 0) {
            mainTimerRead.readMeter_Up_Len = LPUART0Ddata.RxLen;
            memcpy(mainTimerRead.readMeter_RxBuf, LPUART0Ddata.RxBuf, LPUART0Ddata.RxLen);
            mainTimerRead.readMeter_OK_Flage = 1;
        }
        if (read_Copy_Meter_In_Timer_In_Main_Flage != 0) {
            copyTimerRead.readMeter_Up_Len = LPUART0Ddata.RxLen;
            memcpy(copyTimerRead.readMeter_RxBuf, LPUART0Ddata.RxBuf, LPUART0Ddata.RxLen);
            copyTimerRead.readMeter_OK_Flage = 1;
        }
        if (imm_Read.immediately_Exec_Flage != 0) {
            imm_Read.immediately_Up_Len = LPUART0Ddata.RxLen;
            memcpy(imm_Read.immediately_RXBuffer, LPUART0Ddata.RxBuf, LPUART0Ddata.RxLen);
            imm_Read.immediately_OK_Flage = 1;
        }
    }
}

// RS4852
void LPUART0_IRQHandler(void) {
    if (FL_LPUART_IsActiveFlag_RXBuffFull(LPUART0)) {
        LPUART0Ddata.Rxdata = FL_LPUART_ReadRXBuff(LPUART0);
        if ((LPUART0Ddata.Rxdata == 0x68) || (LPUART0Ddata.RxBuf[0] == 0x68)) {
            if (LPUART0Ddata.RxLen == 0)
                adjust_LPUART0_Head();
            else {
                if (LPUART0Ddata.RxLen < 12)
                    SAVE_LPUART0_Data();
                else {
                    if (LPUART0Ddata.RxLen >= 70) { // 超过最大70
                        Clear_LPUART0_Data();
                        return;
                    }
                    if (LPUART0Ddata.RxLen <= LPUART0Ddata.RxBuf[10] + 11)
                        SAVE_LPUART0_Data();
                    else {
                        if (LPUART0Ddata.Rxdata == 0x16) {
                            SAVE_LPUART0_Data();
                            LPUART0_ReadData_Deal();
                        } else {
                            SAVE_LPUART0_Data();
                            if (LPUART0Ddata.RxLen > 70) {
                                Clear_LPUART0_Data();
                            }
                        }
                    }
                }
            }
        }
    }
}

void adjust_LPUART1_Head(void) {
    if (LPUART1Ddata.Rxdata == 0x68) {
        LPUART1Ddata.RxBuf[0] = LPUART1Ddata.Rxdata;
        LPUART1Ddata.RxLen = 1;
    } else {
        if ((LPUART1Ddata.RxBuf[0] == 0x68) && (LPUART1Ddata.Rxdata == 0x48)) {
            LPUART1Ddata.RxBuf[1] = LPUART1Ddata.Rxdata;
            LPUART1Ddata.RxLen = 2;
        } else {
            LPUART1Ddata.RxLen = 0;
        }
    }
}

void LPUART1_ReadData_Deal(void) {
    if (Cault_CS(LPUART1Ddata.RxBuf, 0, LPUART1Ddata.RxLen - 2) == LPUART1Ddata.RxBuf[LPUART1Ddata.RxLen - 2]) {
        Modfiy_Parameter_By_CH340();
    }
}

void LPUART1_IRQHandler(void) {
    if (FL_LPUART_IsActiveFlag_RXBuffFull(LPUART1)) {
        LPUART1Ddata.Rxdata = FL_LPUART_ReadRXBuff(LPUART1);
        if ((LPUART1Ddata.Rxdata == 0x68) || (LPUART1Ddata.RxBuf[0] == 0x68)) {
            if (LPUART1Ddata.RxLen == 0)
                adjust_LPUART1_Head();
            else {
                if (LPUART1Ddata.RxLen < 5)
                    SAVE_LPUART1_Data();
                else {
                    if (LPUART1Ddata.RxLen >= 50) { // 超过最大 50
                        Clear_LPUART1_Data();
                        return;
                    }
                    if (LPUART1Ddata.RxLen <= LPUART1Ddata.RxBuf[3] + 2)
                        SAVE_LPUART1_Data();
                    else {
                        if (LPUART1Ddata.Rxdata == 0x16) {
                            SAVE_LPUART1_Data();
                            LPUART1_ReadData_Deal();
                            Clear_LPUART1_Data();
                        } else {
                            SAVE_LPUART1_Data();
                            if (LPUART1Ddata.RxLen > 50) {
                                Clear_LPUART1_Data();
                            }
                        }
                    }
                }
            }
        }
    }
}
