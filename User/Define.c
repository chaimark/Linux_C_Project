#include "Define.h"
int UART0_BAND = 115200;

unsigned int Current_MBUS_MV_Value = 0;
unsigned int Current_MBUS_MA_Value = 0;
const unsigned char HasRX8025T = 0;
const unsigned char SOFT_VERSION = 12;  // 12:V1.2

unsigned int   MBUS_UART1_BaudRate = 2400;
unsigned char  MBUS_UART1_Parity = 2;
unsigned int   RS4851_UART4_BaudRate = 9600;
unsigned char  RS4851_UART4_Parity = 0;
unsigned int   RS4852_LPUART0_BaudRate = 9600;
unsigned char  RS4852_LPUART0_Parity = 0;

#define OPENNET // 开启网路片选
#ifndef OPENNET
unsigned char  UP_Mode_EC20_ON = 1;
unsigned char  UP_Mode_NET_ON = 0;
#else
unsigned char  UP_Mode_EC20_ON = 0;
unsigned char  UP_Mode_NET_ON = 1;
#endif
unsigned char  UP_Mode_MBUSTOUSB_ON = 0;

unsigned char  UP_Mode_WIR_ON = 0;
unsigned char  DUBG_Mode_WIFI_ON = 1;
unsigned char  DOWN_Mode_MBUS_ON = 1;
unsigned char  DOWN_Mode_RS4851_ON = 1;
unsigned char  DOWN_Mode_RS4852_ON = 1;
unsigned char  DOWN_Mode_WIR_ON = 0;
unsigned char  DEFINE_Mode_SETUSB_ON = 1;

//PB9
void EEprom_SDA_IN(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void EEprom_SDA_OUT(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_9);
}

void RX8025T_SDA_SET_IN(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void RX8025T_SDA_SET_OUT(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOD, FL_GPIO_PIN_6);
}
