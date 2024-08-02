/**
******************************************************************************
* @file           : mf_config.c
* @brief          : MCU FUNCTION CONFIG               
******************************************************************************
* @attention
*
* Copyright (c) [2021] [Fudan Microelectronics]
* THIS SOFTWARE is licensed under Mulan PSL v2.
* can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*
******************************************************************************
*/


/* USER CODE BEGIN Header */ 

/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MF_CONFIG_H
#define __MF_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#if defined(FM33LG0XX)
#include "fm33lg0xx_fl.h"
#elif defined(FM33LC0XX)
#include "fm33lc0xx_fl.h"
#endif

/* Exported functions prototypes ---------------------------------------------*/

// void MF_Gpio_Init(void);
// void MF_LPUART0_Interrupt_Init(void);
// void MF_LPUART0_Init(void);
// void MF_LPUART1_Interrupt_Init(void);
// void MF_LPUART1_Init(void);
// void MF_SPI1_Init(void);
// void MF_UART0_Interrupt_Init(void);
// void MF_UART0_Init(void);
// void MF_UART1_Interrupt_Init(void);
// void MF_UART1_Init(void);
// void MF_UART4_Interrupt_Init(void);
// void MF_UART4_Init(void);
// void MF_NVIC_Init(void);
// void BSTIM32_Setup(void);

void MF_Clock_Init(void);
void MF_Config_Init(void);
void Error_Handler(void);
void MF_RCC_XTLF_IO_Init(void);
void MF_I2C_Init(void);
void MF_UART5_Interrupt_Init(void);
void MF_UART5_Init(void);
void MF_ADC_Common_Init(void);
void MF_ADC_Interrupt_Init(void);
void MF_ADC_Init(void);
void MF_IWDT_Init(void);
void MF_RTC_1Second_Interrupt_Init(void);
void MF_RTC_1Second_Init(void);
void MF_BSTIM32_10mS_Interrupt_Init(void);
void MF_BSTIM32_10mS_Init(void);

void GPTIM0_Start(void);
void GPTIM1_Start(void);

void LPTIM32_250mS_Start(void);

extern void AT45DB041_Initial(void); 
extern unsigned long int EEprom_Byte4Read(unsigned long int addr);
extern void EEprom_Byte4Write(unsigned long int addr,unsigned long int data);
extern unsigned char EEprom_Byte1Write(unsigned int addr,unsigned char data);
extern unsigned char EEprom_Byte1Read(unsigned int addr);
extern void MF_CH340_To_LPUART1_Init(void);
extern void MF_CH340_To_LPUART1_Interrupt_Init(void);
extern void MF_NET_To_UART0_Init(void);
/* Private defines -----------------------------------------------------------*/

#ifndef NVIC_PRIORITYGROUP_0

#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif


#ifdef __cplusplus
}
#endif

#endif /* __MF_CONFIG_H */

/*************************(C) COPYRIGHT Fudan Microelectronics **** END OF FILE*************************/
