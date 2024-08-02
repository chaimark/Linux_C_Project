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

/* Includes ------------------------------------------------------------------*/
#include "mf_config.h"
#include "Define.h"

/**
 * @brief  GPIO Initialization function
 * @param  void
 * @retval None
 */
void MF_GPIO_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;

    // PA0 V50_PWR_CTL  PA4_EC20_PWR_KEY_MCU  PA6_V3.3_PWR_CTL  PA8_UP_NET  PA9_29302_CTL  PA10_MCU_RS4852 PA11_USB_482 PA12_USB_LoRa PA15_EC20_WAKEUP_IN
    GPIO_InitStruct.pin = FL_GPIO_PIN_0 | FL_GPIO_PIN_4 | FL_GPIO_PIN_6 | FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12 | FL_GPIO_PIN_15;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_0 | FL_GPIO_PIN_4 | FL_GPIO_PIN_6 | FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12 | FL_GPIO_PIN_15);

    // PA1_EC20_STATUS  PA5_NET_RST  PA7_NET_CFG
    GPIO_InitStruct.pin = FL_GPIO_PIN_1 | FL_GPIO_PIN_5 | FL_GPIO_PIN_7;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PB0 RS4851_USB PB1_NULL PB4_CH340_MCU  PB5_LED_4G   PB7_EC20_RST_N_MCU    PB10_LED_RS485   PB11_LED_LoRa  PB12_RS4851_MCU
    GPIO_InitStruct.pin = FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_4 | FL_GPIO_PIN_5 | FL_GPIO_PIN_7 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_4 | FL_GPIO_PIN_5 | FL_GPIO_PIN_7 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12);

    // PB6_EC20_AP_READY
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PB8_II_SCK   PB9_II_SDA
    GPIO_InitStruct.pin = FL_GPIO_PIN_8 | FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_8 | FL_GPIO_PIN_9);

    // PC0_LED_Heart_MCU   PC1_MBUS_MCU   PC4_CH340_MBUS  PC6_FH_MOSI   PC7_FH_SCLK   PC8_FH_RST  PC9_FH_CS  PC10_MBUS_ERR   PC11_LED_NET_MCU   PC12_LED_COM_MCU
    GPIO_InitStruct.pin = FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_4 | FL_GPIO_PIN_6 | FL_GPIO_PIN_7 | FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_4 | FL_GPIO_PIN_6 | FL_GPIO_PIN_7 | FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12);

    // PC5_FH_MISO
    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //PD3_EC20_MCU PD4 MBUS_CTL  PD11_LED_SET_MCU
    GPIO_InitStruct.pin = FL_GPIO_PIN_3 | FL_GPIO_PIN_4;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOD, FL_GPIO_PIN_3 | FL_GPIO_PIN_4);

    //PD2_AD_OUT PD11_MBUS_AD   
    GPIO_InitStruct.pin = FL_GPIO_PIN_2 | FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // PD5_TIME_SCL   PD6_TIME_SDA
    GPIO_InitStruct.pin = FL_GPIO_PIN_5 | FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOD, FL_GPIO_PIN_5 | FL_GPIO_PIN_6);
}

/**
 * @brief  RCC_XTLF_IO Initialization function
 * @param  void
 * @retval None
 */
void MF_RCC_XTLF_IO_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
 * @brief  I2C Initialization function
 * @param  void
 * @retval None
 */
void MF_I2C_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_I2C_MasterMode_InitTypeDef I2C_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    I2C_InitStruct.clockSource = FL_RCC_I2C_CLK_SOURCE_RCHF;
    I2C_InitStruct.baudRate = 400000;
    FL_I2C_MasterMode_Init(I2C, &I2C_InitStruct);
}

void MF_SPI1_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;

    FL_SPI_InitTypeDef SPI1_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_8;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    SPI1_InitStruct.transferMode = FL_SPI_TRANSFER_MODE_FULL_DUPLEX;
    SPI1_InitStruct.mode = FL_SPI_WORK_MODE_MASTER;
    SPI1_InitStruct.dataWidth = FL_SPI_DATA_WIDTH_8B;
    SPI1_InitStruct.clockPolarity = FL_SPI_POLARITY_NORMAL;
    SPI1_InitStruct.clockPhase = FL_SPI_PHASE_EDGE1;
    SPI1_InitStruct.softControl = FL_ENABLE;
    SPI1_InitStruct.baudRate = FL_SPI_BAUDRATE_DIV8;
    SPI1_InitStruct.bitOrder = FL_SPI_BIT_ORDER_MSB_FIRST;

    FL_SPI_Init(SPI1, &SPI1_InitStruct);
}

/**
 * @brief  UART0 Initialization function
 * @param  void
 * @retval None
 */
void MF_EC20_To_UART0_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_UART_InitTypeDef UART0_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    UART0_InitStruct.clockSrc = FL_RCC_UART0_CLK_SOURCE_APB1CLK;
    UART0_InitStruct.baudRate = 115200;
    UART0_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART0_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    UART0_InitStruct.parity = FL_UART_PARITY_NONE;
    UART0_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;
    FL_UART_Init(UART0, &UART0_InitStruct);
}

void MF_EC20_To_UART0_Interrupt_Init(void) {
    FL_UART_ClearFlag_RXBuffFull(UART0);
    FL_UART_EnableIT_RXBuffFull(UART0);
}

void MF_NET_To_UART0_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_UART_InitTypeDef UART0_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    UART0_InitStruct.clockSrc = FL_RCC_UART0_CLK_SOURCE_APB1CLK;
    UART0_InitStruct.baudRate = UART0_BAND;
    UART0_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART0_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    UART0_InitStruct.parity = FL_UART_PARITY_NONE;
    UART0_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;
    FL_UART_Init(UART0, &UART0_InitStruct);
}

void MF_NET_To_UART0_Interrupt_Init(void) {
    FL_UART_ClearFlag_RXBuffFull(UART0);
    FL_UART_EnableIT_RXBuffFull(UART0);
}

void MF_MBUS_To_UART1_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_UART_InitTypeDef UART1_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    UART1_InitStruct.clockSrc = FL_RCC_UART1_CLK_SOURCE_APB1CLK;
    if (MBUS_UART1_BaudRate == 1200)
        UART1_InitStruct.baudRate = 1200;
    else if (MBUS_UART1_BaudRate == 2400)
        UART1_InitStruct.baudRate = 2400;
    else UART1_InitStruct.baudRate = 2400;
    UART1_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART1_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    if (MBUS_UART1_Parity == 0)
        UART1_InitStruct.parity = FL_UART_PARITY_NONE;
    else if (MBUS_UART1_Parity == 1)
        UART1_InitStruct.parity = FL_UART_PARITY_ODD;
    else if (MBUS_UART1_Parity == 2)
        UART1_InitStruct.parity = FL_UART_PARITY_EVEN;
    else UART1_InitStruct.parity = FL_UART_PARITY_EVEN;
    UART1_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;
    FL_UART_Init(UART1, &UART1_InitStruct);
}

void MF_MBUS_To_UART1_Interrupt_Init(void) {
    FL_UART_ClearFlag_RXBuffFull(UART1);
    FL_UART_EnableIT_RXBuffFull(UART1);
}

void MF_RS4851_To_UART4_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_UART_InitTypeDef UART4_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    UART4_InitStruct.clockSrc = FL_RCC_UART0_CLK_SOURCE_APB1CLK;
    if (RS4851_UART4_BaudRate == 1200)
        UART4_InitStruct.baudRate = 1200;
    else if (RS4851_UART4_BaudRate == 2400)
        UART4_InitStruct.baudRate = 2400;
    else if (RS4851_UART4_BaudRate == 4800)
        UART4_InitStruct.baudRate = 4800;
    else if (RS4851_UART4_BaudRate == 9600)
        UART4_InitStruct.baudRate = 9600;
    else UART4_InitStruct.baudRate = 9600;
    UART4_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART4_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    if (RS4851_UART4_Parity == 0)
        UART4_InitStruct.parity = FL_UART_PARITY_NONE;
    else if (RS4851_UART4_Parity == 1)
        UART4_InitStruct.parity = FL_UART_PARITY_ODD;
    else if (RS4851_UART4_Parity == 2)
        UART4_InitStruct.parity = FL_UART_PARITY_EVEN;
    else UART4_InitStruct.parity = FL_UART_PARITY_NONE;
    UART4_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;
    FL_UART_Init(UART4, &UART4_InitStruct);
}

void MF_RS4851_To_UART4_Interrupt_Init(void) {
    FL_UART_ClearFlag_RXBuffFull(UART4);
    FL_UART_EnableIT_RXBuffFull(UART4);
}

void MF_WIFI_To_UART5_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_UART_InitTypeDef UART5_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    UART5_InitStruct.clockSrc = NULL;
    UART5_InitStruct.baudRate = 115200;
    UART5_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART5_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    UART5_InitStruct.parity = FL_UART_PARITY_NONE;
    UART5_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;
    FL_UART_Init(UART5, &UART5_InitStruct);
}

void MF_WIFI_To_UART5_Interrupt_Init(void) {
    FL_UART_ClearFlag_RXBuffFull(UART5);
    FL_UART_EnableIT_RXBuffFull(UART5);
}

void MF_RS4852_To_LPUART0_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_LPUART_InitTypeDef LPUART0_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LPUART0_InitStruct.clockSrc = FL_RCC_LPUART_CLK_SOURCE_LSCLK;
    if (RS4852_LPUART0_BaudRate == 1200)
        LPUART0_InitStruct.baudRate = FL_LPUART_BAUDRATE_1200;
    else if (RS4852_LPUART0_BaudRate == 2400)
        LPUART0_InitStruct.baudRate = FL_LPUART_BAUDRATE_2400;
    else if (RS4852_LPUART0_BaudRate == 4800)
        LPUART0_InitStruct.baudRate = FL_LPUART_BAUDRATE_4800;
    else if (RS4852_LPUART0_BaudRate == 9600)
        LPUART0_InitStruct.baudRate = FL_LPUART_BAUDRATE_9600;
    else LPUART0_InitStruct.baudRate = FL_LPUART_BAUDRATE_9600;
    LPUART0_InitStruct.dataWidth = FL_LPUART_DATA_WIDTH_8B;
    LPUART0_InitStruct.stopBits = FL_LPUART_STOP_BIT_WIDTH_1B;
    if (RS4852_LPUART0_Parity == 0)
        LPUART0_InitStruct.parity = FL_LPUART_PARITY_NONE;
    else if (RS4852_LPUART0_Parity == 1)
        LPUART0_InitStruct.parity = FL_LPUART_PARITY_ODD;
    else if (RS4852_LPUART0_Parity == 2)
        LPUART0_InitStruct.parity = FL_LPUART_PARITY_EVEN;
    else LPUART0_InitStruct.parity = FL_LPUART_PARITY_NONE;
    LPUART0_InitStruct.transferDirection = FL_LPUART_DIRECTION_TX_RX;
    FL_LPUART_Init(LPUART0, &LPUART0_InitStruct);
}

void MF_RS4852_To_LPUART0_Interrupt_Init(void) {
    FL_LPUART_ClearFlag_RXBuffFull(LPUART0);
    FL_LPUART_EnableIT_RXBuffFull(LPUART0);
}

void MF_CH340_To_LPUART1_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    FL_LPUART_InitTypeDef LPUART1_InitStruct;
    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    LPUART1_InitStruct.clockSrc = FL_RCC_LPUART_CLK_SOURCE_LSCLK;
    LPUART1_InitStruct.baudRate = FL_LPUART_BAUDRATE_2400;
    LPUART1_InitStruct.dataWidth = FL_LPUART_DATA_WIDTH_8B;
    LPUART1_InitStruct.stopBits = FL_LPUART_STOP_BIT_WIDTH_1B;
    LPUART1_InitStruct.parity = FL_LPUART_PARITY_EVEN;
    LPUART1_InitStruct.transferDirection = FL_LPUART_DIRECTION_TX_RX;
    FL_LPUART_Init(LPUART1, &LPUART1_InitStruct);
}

void MF_CH340_To_LPUART1_Interrupt_Init(void) {
    FL_LPUART_ClearFlag_RXBuffFull(LPUART1);
    FL_LPUART_EnableIT_RXBuffFull(LPUART1);
}

/**
 * @brief  ADC_Common Initialization function
 * @param  void
 * @retval None
 */
void MF_ADC_Common_Init(void) {
    FL_ADC_CommonInitTypeDef Common_InitStruct;

    Common_InitStruct.clockSource = FL_RCC_ADC_CLK_SOURCE_RCHF;
    Common_InitStruct.clockPrescaler = FL_RCC_ADC_PSC_DIV8;

    FL_ADC_CommonInit(&Common_InitStruct);
}

/**
 * @brief  ADC Initialization function
 * @param  void
 * @retval None
 */
void MF_ADC_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;

    FL_ADC_InitTypeDef defaultInitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_2 | FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    defaultInitStruct.conversionMode = FL_ADC_CONV_MODE_SINGLE;
    defaultInitStruct.autoMode = FL_ADC_SINGLE_CONV_MODE_AUTO;
    defaultInitStruct.waitMode = FL_ENABLE;
    defaultInitStruct.overrunMode = FL_ENABLE;
    defaultInitStruct.scanDirection = FL_ADC_SEQ_SCAN_DIR_BACKWARD;
    defaultInitStruct.externalTrigConv = FL_ADC_TRIGGER_EDGE_NONE;
    defaultInitStruct.triggerSource = FL_ADC_TRGI_PA8;
    defaultInitStruct.fastChannelTime = FL_ADC_FAST_CH_SAMPLING_TIME_4_ADCCLK;
    defaultInitStruct.lowChannelTime = FL_ADC_SLOW_CH_SAMPLING_TIME_192_ADCCLK;
    defaultInitStruct.oversamplingMode = FL_ENABLE;
    defaultInitStruct.overSampingMultiplier = FL_ADC_OVERSAMPLING_MUL_16X;
    defaultInitStruct.oversamplingShift = FL_ADC_OVERSAMPLING_SHIFT_4B;

    FL_ADC_Init(ADC, &defaultInitStruct);
}

/**
 * @brief  ADC Interrupt Initialization function
 * @param  void
 * @retval None
 */
void MF_ADC_Interrupt_Init(void) {
    FL_ADC_ClearFlag_EndOfSequence(ADC);
    FL_ADC_EnableIT_EndOfSequence(ADC);

    FL_ADC_ClearFlag_EndOfConversion(ADC);
    FL_ADC_EnableIT_EndOfConversion(ADC);
}

/**
 * @brief  IWDT Initialization function
 * @param  void
 * @retval None
 */
void MF_IWDT_Init(void) {
    FL_IWDT_InitTypeDef IWDT_InitStruct;

    IWDT_InitStruct.overflowPeriod = FL_IWDT_PERIOD_4000MS;
    IWDT_InitStruct.iwdtWindows = 0;

    FL_IWDT_Init(IWDT, &IWDT_InitStruct);
}

/**
 * @brief  RTC Initialization function
 * @param  void
 * @retval None
 */
void MF_RTC_1Second_Init(void) {
    FL_RTC_InitTypeDef RTC_InitStruct;
    RTC_InitStruct.year = 0x24;
    RTC_InitStruct.month = 0x09;
    RTC_InitStruct.day = 0x15;
    RTC_InitStruct.week = 0x1;
    RTC_InitStruct.hour = 0x00;
    RTC_InitStruct.minute = 0x01;
    RTC_InitStruct.second = 0x01;
    FL_RTC_Init(RTC, &RTC_InitStruct);
}

/**
 * @brief  RTC Interrupt Initialization function
 * @param  void
 * @retval None
 */
void MF_RTC_1Second_Interrupt_Init(void) {
    FL_RTC_ClearFlag_Second(RTC);
    FL_RTC_EnableIT_Second(RTC);
}

/**
 * @brief  BSTIM32 Initialization function
 * @param  void
 * @retval None
 */
void MF_BSTIM32_10mS_Init(void) {
    FL_BSTIM32_InitTypeDef BSTIM32_InitStruct;
    BSTIM32_InitStruct.prescaler = 799;
    BSTIM32_InitStruct.autoReload = 799;
    BSTIM32_InitStruct.autoReloadState = FL_ENABLE;
    BSTIM32_InitStruct.clockSource = FL_RCC_BSTIM32_CLK_SOURCE_APB2CLK;
    FL_BSTIM32_Init(BSTIM32, &BSTIM32_InitStruct);
}

/**
 * @brief  BSTIM32 Interrupt Initialization function
 * @param  void
 * @retval None
 */
void MF_BSTIM32_10mS_Interrupt_Init(void) {
    FL_BSTIM32_ClearFlag_Update(BSTIM32);
    FL_BSTIM32_EnableIT_Update(BSTIM32);
}

/**
 * @brief  GPTIM0_TimerBase Initialization function
 * @param  void
 * @retval None
 */
void MF_GPTIM0_TimerBase_100mS_Init(void) // 100ms
{
    /*IO CONFIG*/
    FL_GPTIM_InitTypeDef TimerBaseInitStruct;
    TimerBaseInitStruct.prescaler = 6400 - 1;
    TimerBaseInitStruct.counterMode = FL_GPTIM_COUNTER_DIR_UP;
    TimerBaseInitStruct.autoReload = 1000 - 1; // 0xFFFFU;
    TimerBaseInitStruct.autoReloadState = FL_DISABLE;
    TimerBaseInitStruct.clockDivision = FL_GPTIM_CLK_DIVISION_DIV1;
    FL_GPTIM_Init(GPTIM0, &TimerBaseInitStruct);
    FL_GPTIM_EnableIT_Update(GPTIM0);
    GPTIM0_Start();
}

/**
 * @brief  GPTIM0_TimerBase Initialization function
 * @param  void
 * @retval None
 */
void MF_GPTIM1_TimerBase_100mS_Init(void) // 100ms
{
    /*IO CONFIG*/
    FL_GPTIM_InitTypeDef TimerBaseInitStruct;
    TimerBaseInitStruct.prescaler = 6400 - 1;
    TimerBaseInitStruct.counterMode = FL_GPTIM_COUNTER_DIR_UP;
    TimerBaseInitStruct.autoReload = 1000 - 1; // 0xFFFFU;
    TimerBaseInitStruct.autoReloadState = FL_DISABLE;
    TimerBaseInitStruct.clockDivision = FL_GPTIM_CLK_DIVISION_DIV1;
    FL_GPTIM_Init(GPTIM1, &TimerBaseInitStruct);
    FL_GPTIM_EnableIT_Update(GPTIM1);
    GPTIM1_Start();
}

/**
 * @brief  LPTIM32 Initialization function
 * @param  void
 * @retval None
 */
void MF_LPTIM32_Init(void) {
    /*IO CONFIG*/
    FL_LPTIM32_InitTypeDef defaultInitStruct;
    defaultInitStruct.clockSource = FL_RCC_LPTIM32_CLK_SOURCE_LSCLK;
    defaultInitStruct.prescalerClockSource = FL_LPTIM32_CLK_SOURCE_INTERNAL;
    defaultInitStruct.prescaler = FL_LPTIM32_PSC_DIV1;
    defaultInitStruct.autoReload = 8192 - 1; // 32768-1;
    defaultInitStruct.mode = FL_LPTIM32_OPERATION_MODE_NORMAL;
    defaultInitStruct.onePulseMode = FL_LPTIM32_ONE_PULSE_MODE_CONTINUOUS;
    defaultInitStruct.countEdge = FL_LPTIM32_ETR_COUNT_EDGE_RISING;
    defaultInitStruct.triggerEdge = FL_LPTIM32_ETR_TRIGGER_EDGE_RISING;
    FL_LPTIM32_Init(LPTIM32, &defaultInitStruct);
    FL_LPTIM32_EnableIT_Update(LPTIM32);
    LPTIM32_250mS_Start();
}

/**
 * @brief  NVIC Initialization function
 * @param  void
 * @retval None
 */
void MF_NVIC_RTC_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, RTC_IRQn);
}

void MF_NVIC_BSTIM_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, BSTIM_IRQn);
}

void MF_NVIC_GPTIM0_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, GPTIM0_IRQn);
}

void MF_NVIC_GPTIM1_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, GPTIM1_IRQn);
}

void MF_NVIC_LPTIM_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, LPTIM_IRQn);
}

void MF_NVIC_UART0_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x0002;
    FL_NVIC_Init(&InterruptConfigStruct, UART0_IRQn);
}

void MF_NVIC_UART1_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, UART1_IRQn);
}

void MF_NVIC_UART4_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, UART4_IRQn);
}

void MF_NVIC_UART5_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x0002;
    FL_NVIC_Init(&InterruptConfigStruct, UART5_IRQn);
}

void MF_NVIC_LPUART0_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x0002;
    FL_NVIC_Init(&InterruptConfigStruct, LPUART0_IRQn);
}

void MF_NVIC_LPUART1_Init(void) {
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, LPUART1_IRQn);
}
/**
 * @brief  Clock Initialization.
 * @retval None
 */
void MF_Clock_Init(void) {
    /* Initial XTLF */
    MF_RCC_XTLF_IO_Init();
    FL_RCC_XTLF_SetWorkCurrent(FL_RCC_XTLF_WORK_CURRENT_450NA); /* 设置工作电流为最大以便快速起振 */
    FL_RCC_XTLF_Enable();

    /* Initial Low Speed Clock */
    FL_RCC_SetLSCLKClockSource(FL_RCC_LSCLK_CLK_SOURCE_XTLF);
    FL_RCC_DisableLSCLKAutoSwitch();

    /* Initial RCHF */
    FL_RCC_RCHF_SetFrequency(FL_RCC_RCHF_FREQUENCY_8MHZ);
    FL_RCC_RCHF_Enable();

   /* Initial PLL */
    FL_RCC_PLL_Disable();
    FL_RCC_PLL_SetClockSource(FL_RCC_PLL_CLK_SOURCE_RCHF);
    FL_RCC_PLL_SetPrescaler(FL_RCC_PLL_PSC_DIV8);
    FL_RCC_PLL_WriteMultiplier(63); /* 输出64MHz */
    FL_RCC_PLL_SetOutputMultiplier(FL_RCC_PLL_OUTPUT_X1);
    FL_RCC_PLL_Enable();
    while (FL_RCC_IsActiveFlag_PLLReady() != FL_SET);

    /* Initial System Clock */
    FL_FLASH_SetReadWait(FLASH, FL_FLASH_READ_WAIT_2CYCLE);	   /* 设置FLASH读等待为 2 个周期 */
    FL_RCC_SetSystemClockSource(FL_RCC_SYSTEM_CLK_SOURCE_PLL); /* 设置系统主时钟为 PLL */
    FL_RCC_SetAHBPrescaler(FL_RCC_AHBCLK_PSC_DIV1);
    FL_RCC_SetAPB1Prescaler(FL_RCC_APB1CLK_PSC_DIV1);
    FL_RCC_SetAPB2Prescaler(FL_RCC_APB2CLK_PSC_DIV1);

    SystemCoreClockUpdate();
}

void MF_Config_Init(void) {
    /* Initial GPIO */
    MF_GPIO_Init();
    AT45DB041_Initial();
    /* Initial ADC */
    MF_ADC_Common_Init();
    MF_ADC_Init();
    /* Initial IWDT */
    MF_IWDT_Init();
    /* Initial RTC */
    MF_RTC_1Second_Init();
    MF_RTC_1Second_Interrupt_Init();
    MF_NVIC_RTC_Init();
	  /* Initial BSTIM32 */
    MF_BSTIM32_10mS_Init();
    MF_BSTIM32_10mS_Interrupt_Init();
	  MF_NVIC_BSTIM_Init();
    MF_GPTIM0_TimerBase_100mS_Init();
    MF_NVIC_GPTIM0_Init();
    MF_GPTIM1_TimerBase_100mS_Init();
    MF_NVIC_GPTIM1_Init();
    MF_LPTIM32_Init();
    MF_NVIC_LPTIM_Init();
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t * file, uint32_t line) {
}
#endif /* USE_FULL_ASSERT */

/*************************(C) COPYRIGHT Fudan Microelectronics **** END OF FILE*************************/
