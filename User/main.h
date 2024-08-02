#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "mf_config.h"
#include "ADC.h"
#include "BSTim.h"
#include "Define.h"
#include "LPUart.h"
#include "UpData.h"
#include "4G_Module.h"
#include "NetProt_Module.h"
#include "RTC.h"
#include "Define.h"
#include "fm33_assert.h"
#include "RTC.h"
#include "BSTim.h"
#include "MQTT_JSON.h"
#include "Uart.h"
#include "AT24C0256.h"
#include "TimerMainReading.h"
#include "ImmediatelyReading.h"
#include "RX8025T.h"

extern void MF_EC20_To_UART0_Init(void);
extern void MF_EC20_To_UART0_Interrupt_Init(void);
extern void MF_NET_To_UART0_Interrupt_Init(void);
extern void MF_MBUS_To_UART1_Init(void);
extern void MF_MBUS_To_UART1_Interrupt_Init(void);
extern void MF_RS4851_To_UART4_Init(void);
extern void MF_RS4851_To_UART4_Interrupt_Init(void);
extern void MF_WIFI_To_UART5_Init(void);
extern void MF_WIFI_To_UART5_Interrupt_Init(void);
extern void MF_RS4852_To_LPUART0_Init(void);
extern void MF_RS4852_To_LPUART0_Interrupt_Init(void);
extern void MF_CH340_To_LPUART1_Init(void);
extern void MF_CH340_To_LPUART1_Interrupt_Init(void);
extern void MF_NVIC_UART0_Init(void);
extern void MF_NVIC_UART1_Init(void);
extern void MF_NVIC_UART4_Init(void);
extern void MF_NVIC_UART5_Init(void);
extern void MF_NVIC_LPUART0_Init(void);
extern void MF_NVIC_LPUART1_Init(void);

#endif 
